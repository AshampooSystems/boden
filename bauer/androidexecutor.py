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

        self.androidBuildApiVersion = "28"
        self.androidBuildToolsVersion = "28.0.2"
        self.androidEmulatorApiVersion = "28"

    def buildTarget(self, configuration, args, target):
        androidAbi = self.getAndroidABIFromArch(configuration.arch)
        androidHome = self.getAndroidHome()
        buildDir = self.buildFolder.getBuildDir(configuration)

        if configuration.buildsystem == "AndroidStudio":
            self.buildTargetAndroidStudio(configuration, args, target, androidAbi, androidHome, buildDir)
        else:
            self.buildTargetMake(configuration, args, target)

    def buildTargetMake(self, configuration, args, target):
        buildExecutor = BuildExecutor(self.generatorInfo, self.sourceDirectory, self.buildFolder)
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

        self.prepareAndroidEnvironment(configuration, args.accept_terms)

        buildDir = self.buildFolder.getBuildDir(configuration)

        if configuration.buildsystem == "AndroidStudio":
            self.prepareAndroidStudio(platformState, configuration, androidAbi, androidHome, buildDir)
        else:
            self.prepareMake(platformState, configuration, args, androidAbi, androidHome, buildDir)

    def prepareMake(self, platformState, configuration, args, androidAbi, androidHome, cmakeBuildDir):
        self.cmake.open(self.sourceDirectory, cmakeBuildDir, "Unix Makefiles")

        android_abi_arg = self.getAndroidABIFromArch(configuration.arch)
        if not android_abi_arg:
            raise error.InvalidArchitectureError("No target architecture specified. The architecture parameter is required for makefile build systems.")

        cmakeArguments = [
            "-DCMAKE_TOOLCHAIN_FILE=%s/ndk-bundle/build/cmake/android.toolchain.cmake" % (androidHome),
            "-DANDROID_ABI=%s" % (android_abi_arg),
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

        self.gradle.stop()

        tmpCMakeFolder = os.path.join(buildDir, "tmp-cmake-gen")

        makefile_android_abi = self.getAndroidABIFromArch(configuration.arch)
        if not makefile_android_abi:
            # If we target multiple architectures at the same time, we simply use x86 for the temporary
            # Makefile project we generate here (since makefiles support only one architecture).
            # Note that the makefile is only used temporarily to detect the available projects and is never
            # used to build anything.
            makefile_android_abi = "x86"

        self.cmake.open(self.sourceDirectory, tmpCMakeFolder, "Unix Makefiles")

        cmakeArguments = [ 
            "-DCMAKE_TOOLCHAIN_FILE=%s/ndk-bundle/build/cmake/android.toolchain.cmake" % (androidHome), 
            "-DCMAKE_SYSTEM_NAME=Android", 
            "-DANDROID_ABI=%s" % (makefile_android_abi),
            "-DANDROID_NATIVE_API_LEVEL=%s" % ( self.androidBuildApiVersion ),
            "-DBAUER_RUN=Yes" ]

        if sys.platform == 'win32':
            cmakeArguments += ["-DCMAKE_MAKE_PROGRAM=%s/ndk-bundle/prebuilt/windows-x86_64/bin/make.exe" % (androidHome)]

        self.logger.debug("Starting configure ...")
        self.logger.debug(" Arguments: %s", cmakeArguments)
        self.logger.debug(" Generator: %s", "Unix Makefiles")

        self.cmake.configure(cmakeArguments)

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

        # Use external CMake for building native code (supported as of AndroidStudio 3.2)
        generator = AndroidStudioProjectGenerator(self.gradle, self.cmake, buildDir, self.androidBuildApiVersion)
       
        for project in projects:
            self.logger.debug("Generating project %s with targets: %s", project["name"], project["targetNames"])
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

            break

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
            if sys.platform.startswith("linux"):
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

            if sys.platform == "win32":
                android_home_dir = os.path.expanduser("~/AppData/Local/Android/SDK")

                if os.path.exists(android_home_dir):
                    self.logger.info("Android home directory automatically detected as: %s", android_home_dir)
                else:
                    android_home_dir = None

        if not android_home_dir:                    
            raise Exception("ANDROID_HOME environment variable is not set. Please point it to the root of the android SDK installation.")

        return android_home_dir.replace('\\', '/')

    def getAndroidABIFromArch(self, arch):
        if arch=="std":
            return None
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

    def tryDetectAndroidCmakeComponentName(self, sdkManagerPath):

        command = '"%s" --list' % (sdkManagerPath)
        try:
            output = subprocess.check_output( command, shell=True, env=self.getToolEnv(), universal_newlines=True )
        except:
            self.logger.warning("Failed to get Android SDK module list")
            return None

        last_cmake_component_name = None
        for line in output.splitlines():
            line = line.strip()
            if line.startswith("cmake;"):
                last_cmake_component_name = line.partition(" ")[0]

        return last_cmake_component_name



    def prepareAndroidEnvironment(self, configuration, accept_terms):
        self.logger.info("Preparing android environment...")
        androidAbi = self.getAndroidABIFromArch(configuration.arch)
        androidHome = self.getAndroidHome()
        sdkManagerPath = self.getBuildToolPath(androidHome, "tools/bin/sdkmanager")

        if accept_terms:
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

        cmakeComponentName = self.tryDetectAndroidCmakeComponentName(sdkManagerPath);
        if cmakeComponentName:
            sdkManagerCommand += ' "%s"' % cmakeComponentName

        try:
            subprocess.check_call( sdkManagerCommand, shell=True, env=self.getToolEnv() )
        except:
            self.logger.warning("Failed getting emulator, you will not be able to 'run' this configuration")

        self.logger.info("Done updating packages.")


