import os, sys
import logging
import subprocess
import shutil

from buildexecutor import BuildExecutor
from androidstudioprojectgenerator import AndroidStudioProjectGenerator
from cmake import CMake
from gradle import Gradle

import error

class AndroidExecutor:
    def __init__(self, buildExecutor, generatorInfo, sourceDirectory, buildFolder):
        self.logger = logging.getLogger(__name__)
        self.generatorInfo = generatorInfo
        self.sourceDirectory = sourceDirectory
        self.buildFolder = buildFolder
        self.buildExecutor = buildExecutor
        self.gradle = Gradle(sourceDirectory)
        self.cmake = CMake()

        self.androidBuildApiVersion = "26"
        self.androidBuildToolsVersion = "26.0.2"
        self.androidEmulatorApiVersion = "26"

    def buildTarget(self, configuration, args, target):
        androidAbi = self.getAndroidABIFromArch(configuration.arch)
        androidHome = self.getAndroidHome()
        buildDir = self.buildFolder.getBuildDir(configuration)

        if configuration.buildsystem == "AndroidStudio":
            self.buildTargetAndroidStudio(configuration, args, target, androidAbi, androidHome, buildDir)
        else:
            self.buildTargetMake(configuration, args, target)

    def buildTargetMake(self, configuration, args, target):
        buildExecutor = BuildExecutor(self.generatorInfo, None, self.sourceDirectory, self.buildFolder)
        buildExecutor.buildTarget(configuration, args, target)

    def buildTargetAndroidStudio(self, configuration, args, target, androidAbi, androidHome, buildDir):

        gradlePath = self.gradle.getGradlePath()
        gradleWrapperPath = self.getBuildToolPath(buildDir, "gradlew")

        arguments = ["\"" + gradleWrapperPath + "\""]

        if target == "clean":
            arguments += ["clean"]
        else:
            if args.config=="Release":
                arguments += ["assembleRelease"]
            else:
                arguments += ["assembleDebug"]

        self.logger.debug("Starting: %s", arguments)

        exitCode = subprocess.call(" ".join(arguments), shell=True, cwd=buildDir, env=self.getToolEnv());
        if exitCode!=0:
            raise error.ToolFailedError("%s" %(arguments), exitCode);

    def build(self, configuration, args):
        self.buildTarget(configuration, args, None)

    def clean(self, configuration, args):
        self.buildTarget(configuration, args, "clean")

    def package(self, configuration, args):
        if configuration.buildsystem == "AndroidStudio":
            self.logger.critical("Cannot build packages with Android Studio, use make instead")
        else:
            self.buildTarget(configuration, args, "package")

    def prepare(self, platformState, configuration, args):
        androidAbi = self.getAndroidABIFromArch(configuration.arch)
        androidHome = self.getAndroidHome()

        self.prepareAndroidEnvironment(configuration)

        buildDir = self.buildFolder.getBuildDir(configuration)

        if configuration.buildsystem == "AndroidStudio":
            self.prepareAndroidStudio(platformState, configuration, androidAbi, androidHome, buildDir)
        else:
            self.prepareMake(platformState, configuration, args, androidAbi, androidHome, buildDir)

    def prepareMake(self, platformState, configuration, args, androidAbi, androidHome, cmakeBuildDir):
        self.cmake.open(self.sourceDirectory, cmakeBuildDir, "Unix Makefiles")

        cmakeArguments = [
            "-DCMAKE_TOOLCHAIN_FILE=%s/ndk-bundle/build/cmake/android.toolchain.cmake" % (androidHome),
            "-DANDROID_ABI=%s" % (self.getAndroidABIFromArch(configuration.arch)),
            "-DANDROID_NATIVE_API_LEVEL=%s" % ( self.androidBuildApiVersion ),
            "-DCMAKE_BUILD_TYPE=%s" % (configuration.config),
            "-DBDN_BUILD_TESTS=Off",
            "-DBDN_BUILD_EXAMPLES=Off",
        ]

        if args.package_generator:
            cmakeArguments += ["-DCPACK_GENERATOR=%s" % (args.package_generator)]

        if args.package_folder:
            packageFolder = args.package_folder
            if not os.path.isabs(packageFolder):
                packageFolder = os.path.join(self.buildFolder.getBaseBuildDir(), packageFolder)

            cmakeArguments += ["-DCPACK_OUTPUT_FILE_PREFIX=%s" % (packageFolder)]


        self.logger.warning("Disabling examples and tests, as we cannot build apk's yet.")

        self.logger.debug("Starting configure ...")
        self.logger.debug(" Source Directory: %s", self.sourceDirectory)
        self.logger.debug(" Output Directory: %s", cmakeBuildDir)
        self.logger.debug(" Config: %s", configuration.config)
        self.logger.debug(" Arguments: %s", cmakeArguments)
        self.logger.debug(" Generator: %s", "Unix Makefiles")

        self.cmake.configure(cmakeArguments)

        pass

    def prepareAndroidStudio(self, platformState, configuration, androidAbi, androidHome, buildDir):

        gradlePath = self.gradle.getGradlePath()
        tmpCMakeFolder = os.path.join(buildDir, "tmp-cmake-gen")

        self.cmake.open(self.sourceDirectory, tmpCMakeFolder, "Unix Makefiles")

        arguments = [ "-DCMAKE_TOOLCHAIN_FILE=%s/ndk-bundle/build/cmake/android.toolchain.cmake" % (androidHome), "-DCMAKE_SYSTEM_NAME=Android", "-DBAUER_RUN=Yes" ]

        if sys.platform == 'win32':
            arguments += ["-DCMAKE_MAKE_PROGRAM=%s/ndk-bundle/prebuilt/windows-x86_64/bin/make.exe" % (androidHome)]


        self.cmake.configure(arguments)

        cmakeConfigurations = self.cmake.codeModel["configurations"]
        if len(cmakeConfigurations) != 1:
            raise Exception("Number of configurations is not 1!")

        targetDependencies = self.calculateDependencies(self.cmake.codeModel)

        config = cmakeConfigurations[0]
        projects = []
        for project in config["projects"]:
            self.logger.debug("Found project: %s", project["name"])
            #projects += [project]
            targetNames = []
            targets = []
            for target in project["targets"]:
                if target["type"] == "SHARED_LIBRARY" or target["type"] == "EXECUTABLE" or target["type"] == "STATIC_LIBRARY":
                    self.logger.debug("Found target: %s", target["name"])
                    targetNames += [target["name"]]
                    targets += [target]

            projects += [{"name" : project["name"], "sourceDirectory" : project["sourceDirectory"],"targetNames" : targetNames, "targets" : targets}]

        # The way Android Studio builds projects with native code has changed
        # quite a bit over time. It used to be very messy, requiring experimental
        # plugins to work properly.
        # Since AndroidStudio 3.0 this has changed and there is a "standard" way
        # to do this. AndroidStudio now includes a custom, forked CMake
        # that actually works - and this builds the native code side.
        # We only support the new style projects now (see git history if
        # you want to know what it used to look like).

        generator = AndroidStudioProjectGenerator(self.gradle, buildDir, self.androidBuildApiVersion)

        for project in projects:
            self.logger.debug("Generating project with targets: %s", project["targetNames"])
            generator.generateTopLevelProject(project["targetNames"])

            for target in project["targets"]:
                targetName = target["name"]

                rootCMakeFile = os.path.join(project["sourceDirectory"], "CMakeLists.txt").replace('\\', '/')

                generator.generateModule( 
                    packageId = "io.boden.android.%s" % (targetName), 
                    cmakeTargetName = targetName, 
                    targetSourceDirectory = target["sourceDirectory"], 
                    userFriendlyTargetName = targetName, 
                    dependencyList = targetDependencies[targetName], 
                    isLibrary = target["type"] == "SHARED_LIBRARY", 
                    android_abi = androidAbi,
                    rootCMakeFile = rootCMakeFile)

        # At the time of this writing, Android Studio will not detect when new source files
        # have been added (even if we do a gradle sync, syncs on the cmake file, etc.).
        # To force re-detection of that we delete the .idea folder.
        idea_dir = os.path.join( buildDir, ".idea")
        if os.path.exists(idea_dir):
            self.logger.info("Deleting .idea folder in build dir to force re-detection of files." )
            shutil.rmtree( idea_dir )

    def getToolEnv(self):
        toolEnv = os.environ
        toolEnv["ANDROID_HOME"] = self.getAndroidHome()
        return toolEnv

    def getAndroidHome(self):
        android_home_dir = os.environ.get("ANDROID_HOME")
        if not android_home_dir:
            if sys.platform == "linux2":
                android_home_dir = os.path.expanduser("~/Android/Sdk")
                if os.path.exists(android_home_dir):
                    self.logger.info("Android home directory automatically detected as: %s", android_home_dir)
                else:
                    android_home_dir = None

            if sys.platform == "darwin":
                android_home_dir = os.path.expanduser("~/Library/Android/sdk")

                if os.path.exists(android_home_dir):
                    self.logger.info("Android home directory automatically detected as: %s", android_home_dir)
                else:
                    android_home_dir = None

        if not android_home_dir:                    
            raise Exception("ANDROID_HOME environment variable is not set. Please point it to the root of the android SDK installation.")

        return android_home_dir.replace('\\', '/')

    def getAndroidABIFromArch(self, arch):
        if arch=="std":
            return "x86_64"
        else:
            return arch

    def getAndroidABI(self, configuration):
        return self.getAndroidABIFromArch(configuration.arch)

    def getBuildToolPath(self, androidHome, tool):
        path = os.path.join(androidHome, tool)
        if sys.platform == "win32":
            if os.path.exists(path + ".bat"):
                path += ".bat"
            elif os.path.exists(path + ".exe"):
                path += ".exe"

        if not os.path.exists(path):
            raise Exception("Couldn't find %s" % (tool))

        return path

    def calculateDependencies(self, codeModel):
        cmakeConfigurations = codeModel["configurations"]
        if len(cmakeConfigurations) != 1:
            raise Exception("Number of configurations is not 1!")

        config = cmakeConfigurations[0]
        projects = []
        artifacts = {}

        for project in config["projects"]:
            for target in project["targets"]:
                if "artifacts" in target and target["type"] == "SHARED_LIBRARY" or target["type"] == "STATIC_LIBRARY":
                    artifacts[target["name"]] = []
                    for artifact in target["artifacts"]:
                        artifacts[target["name"]] += [os.path.basename(artifact)]

        dependencies = {}

        for project in config["projects"]:
            for target in project["targets"]:
                dependencies[target["name"]] = []
                if "linkLibraries" in target:
                    for depname, artifactList in artifacts.items():
                        for artifact in artifactList:
                            if artifact in target["linkLibraries"]:
                              dependencies[target["name"]] += [ depname ]

        return dependencies


    def prepareAndroidEnvironment(self, configuration):
        androidAbi = self.getAndroidABIFromArch(configuration.arch)
        androidHome = self.getAndroidHome()
        sdkManagerPath = self.getBuildToolPath(androidHome, "tools/bin/sdkmanager")

        self.logger.info("Ensuring that all android license agreements are accepted ...")

        licenseCall = subprocess.Popen( '"%s" --licenses' % sdkManagerPath, shell=True, env=self.getToolEnv(), stdin=subprocess.PIPE )

        licenseInputData = ""
        for i in range(100):
            licenseInputData += "y\n"
        licenseCall.communicate(licenseInputData.encode('utf-8'))

        self.logger.info("Done updating licenses.")
        
        self.logger.info("Ensuring that all necessary android packages are installed...")

        sdkManagerCommand = '"%s" "platform-tools" "ndk-bundle" "extras;android;m2repository" "extras;google;m2repository" "build-tools;%s" "platforms;android-%s"' % (
            sdkManagerPath,
            self.androidBuildToolsVersion,
            self.androidBuildApiVersion )

        # the following package is needed for emulation only. But we install it during preparation anyway
        # to ensure that it actually exists (otherwise we would only find out that we cannot emulate
        # this combination after we have built everything)
        sdkManagerCommand += ' "system-images;android-%s;google_apis;%s"' % (self.androidEmulatorApiVersion, androidAbi)
        try:
            subprocess.check_call( sdkManagerCommand, shell=True, env=self.getToolEnv() )
        except:
            self.logger.warning("Failed getting emulator, you will not be able to 'run' this configuration")

        self.logger.info("Done updating packages.")


