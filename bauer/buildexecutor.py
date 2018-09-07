import logging
import os
import subprocess
import shutil

import error
from cmake import CMake
from compilerinfo import CompilerInfo


class BuildExecutor:
    def __init__(self, generatorInfo, emscriptenInfo, sourceDirectory, buildFolder):
        self.logger = logging.getLogger(__name__)
        self.cmake = CMake()
        self.generatorInfo = generatorInfo
        self.emscriptenInfo = emscriptenInfo
        self.sourceDirectory = sourceDirectory
        self.buildFolder = buildFolder

    def build(self, configuration, args):
        self.buildTarget(configuration, args, args.module)

    def clean(self, configuration, args):
        self.buildTarget(configuration, args, "clean")

    def package(self, configuration, args):
        self.buildTarget(configuration, args, "package")

    def buildTarget(self, configuration, args, target):
        configs = [configuration.config]

        if args.config != None:
            configs = [args.config]

        isSingleConfigBuildSystem = self.generatorInfo.isSingleConfigBuildSystem(configuration.buildsystem)

        if not isSingleConfigBuildSystem and args.config == None:
            configs = []
            for cmakeConfiguration in self.cmake.codeModel["configurations"]:
                configs += [cmakeConfiguration["name"]]

        for config in configs:
            buildDirectory = self.buildFolder.getBuildDir(configuration)
            commandArguments = ["\"%s\"" % self.cmake.cmakeExecutable, "--build", "\"%s\"" % buildDirectory]

            if target:
                commandArguments += ["--target", target];

            if not isSingleConfigBuildSystem:
                commandArguments += ["--config", config];

            if args.jobs != None:
                generatorName = self.generatorInfo.getCMakeGeneratorName(configuration.buildsystem);
                if "Visual Studio" in generatorName:
                    os.environ["CL"] = "/MP" + args.jobs
                elif "Xcode" in generatorName:
                    pass # Can' specify multicore via command line :(                
                else:
                    commandArguments += ["--", "-j%s" % (args.jobs) ]

            commandLine = " ".join(commandArguments)
            self.logger.info("Calling: %s", commandLine)

            exitCode = subprocess.call(commandLine, shell=True, cwd=buildDirectory);
            if exitCode!=0:
                raise error.ToolFailedError(commandLine, exitCode);


    def prepare(self, platformState, configuration, args):
        self.logger.debug("prepare(%s)", configuration)

        cmakeBuildDir = self.buildFolder.getBuildDir(configuration);

        toolChainFileName = None;
        needsCleanBuildDir = False
        cmakeEnvironment = "";
        cmakeArch = configuration.arch;
        cmakeArguments = [];

        self.generatorInfo.ensureHaveCmake()

        generatorName = self.generatorInfo.getCMakeGeneratorName(configuration.buildsystem);

        commandIsInQuote = False;

        if args.package_generator:
            cmakeArguments += ["-DCPACK_GENERATOR=%s" % (args.package_generator)]

        if args.package_folder:
            packageFolder = args.package_folder
            if not os.path.isabs(packageFolder):
                packageFolder = os.path.join(self.buildFolder.getBaseBuildDir(), packageFolder)

            cmakeArguments += ["-DCPACK_OUTPUT_FILE_PREFIX=%s" % (packageFolder)]

        if configuration.platform.startswith("win"):
            generatorName, newArgs = self.generateWindowsCMakeArguments(configuration)
            cmakeArguments += newArgs
            # Architecture has to be set via the generator name
            cmakeArch = None

        elif configuration.platform=="mac":
            if configuration.arch!="std":
                raise error.InvalidArchitectureError(arch);
            
            if args.macos_sdk_path:
                cmakeArguments.extend( [ "-DCMAKE_OSX_SYSROOT=%s" % (args.macos_sdk_path) ] )
            if args.macos_min_version:
                cmakeArguments.extend( [ "-DCMAKE_OSX_DEPLOYMENT_TARGET=%s" % (args.macos_min_version) ] )

        elif configuration.platform=="ios":
            if configuration.arch == "std" or configuration.arch == "simulator":
                cmakeArguments.extend( [ "-DIOS_PLATFORM=SIMULATOR64" ] );
            elif configuration.arch == "device":
                cmakeArguments.extend( [ "-DIOS_PLATFORM=OS" ] );
            else:
                raise error.InvalidArchitectureError(arch);

            toolChainFileName = "ios.toolchain.cmake";

        elif configuration.platform=="webems":
            needsCleanBuildDir, cmakeArguments, toolChainFileName = self.generateWebEMSArguments(platformState, configuration.arch)
            cmakeArguments += cmakeArguments

        elif configuration.platform=="linux":
            # we prefer clang over GCC 4. GCC has a lot more bugs in their standard library.
            compilerInfo = CompilerInfo()

            if compilerInfo.gccVersion is not None and compilerInfo.gccVersion[0]==4 and compilerInfo.clangVersion is not None:
                self.logger.info("Forcing use of clang instead of GCC because of bugs in this GCC version.");
                cmakeEnvironment = {"CC" : "/usr/bin/clang", "CXX" : "/usr/bin/clang++"};
                if configuration.arch != "std":
                    raise error.InvalidArchitectureError(arch);
            else:
                if configuration.arch != "std":
                    if configuration.arch == "clang":
                        cmakeEnvironment = {"CC" : "/usr/bin/clang", "CXX" : "/usr/bin/clang++"};
                    elif configuration.arch == "gcc":
                        cmakeEnvironment = {"CC" : "/usr/bin/gcc", "CXX" : "/usr/bin/g++"};
                    else:
                        raise error.InvalidArchitectureError(configuration.arch);


        if toolChainFileName:
            toolChainFilePath = os.path.join(self.sourceDirectory, "cmake/toolchains", toolChainFileName);               

            if not os.path.isfile(toolChainFilePath):
                self.logger.error("Required CMake toolchain file not found: %s" , toolChainFilePath);
                return 5;

            cmakeArguments += ["-DCMAKE_TOOLCHAIN_FILE="+toolChainFilePath]


        if configuration.config:
            cmakeArguments += ["-DCMAKE_BUILD_TYPE="+configuration.config ]

        if cmakeArch:
            cmakeArguments += ["-A "+cmakeArch ]

        if needsCleanBuildDir:
            shutil.rmtree(cmakeBuildDir)


        self.cmake.open(self.sourceDirectory, cmakeBuildDir, generatorName, extraGeneratorName = "", extraEnv=cmakeEnvironment)

        self.logger.debug("Starting configure ...")
        self.logger.debug(" Source Directory: %s", self.sourceDirectory)
        self.logger.debug(" Output Directory: %s", cmakeBuildDir)
        self.logger.debug(" Arguments: %s", cmakeArguments)
        self.logger.debug(" Generator: %s", generatorName)


        self.cmake.configure(cmakeArguments)

    def generateWebEMSArguments(self, platformState, arch):
        if arch!="std":
            raise error.InvalidArchitectureError(arch);

        cmakeArguments = ""
        needsCleanBuildDir = False

        # store the prepared emscripten version in the build dir.
        # If the version changes then we have to clean it completely.
        versionChanged = True
        if "emsSdkVersion" in platformState.state:
            preparedEmsSdkVersion = platformState.state["emsSdkVersion"]
            if preparedEmsSdkVersion != self.emscriptenInfo.version:
                needsCleanBuildDir = True
                self.logger.info("Project was previously prepared for different Emscripten version(%s vs. %s). Auto-cleaning old build files.", preparedEmsSdkVersion, self.emscriptenInfo.version)

        platformState.state["emsSdkVersion"] = self.emscriptenInfo.version

        self.emscriptenInfo.ensure_emscripten_version_active()

        emsdkDir = self.emscriptenInfo.getEmscriptenSdkBuildDir()
        emsdkExePath = os.path.join(emsdkDir, "emsdk");

        cmakeArguments = ["-DEMSCRIPTEN_ROOT_PATH=%s" % (self.emscriptenInfo.getEmscriptenSdkRootPath()) ]
        #self.emscriptenInfo.getEnvironmentSetupCommandLine()
        toolChainFileName = "emscripten.toolchain.cmake";

        return (needsCleanBuildDir, cmakeArguments, toolChainFileName)


    def generateWindowsCMakeArguments(self, configuration):
        generatorName = self.generatorInfo.getCMakeGeneratorName(configuration.buildsystem)
        newArgs = []

        if configuration.platform=="winuwp":
            newArgs = ["-DCMAKE_SYSTEM_NAME=WindowsStore", "-DCMAKE_SYSTEM_VERSION=10.0.10240.0" ]

        if configuration.arch != "std":
            if "Visual Studio" in generatorName:

                # note: passing the architecture with -A does not work properly.
                # Cmake cannot find a compiler if we do it that way.
                # So instead we pass it in the generator name.
                if configuration.arch == "x64":
                    generatorName += " Win64"
                    
                elif configuration.arch == "arm":
                    generatorName += " ARM"
                else:
                    raise error.InvalidArchitectureError(arch)
            else:
                raise error.InvalidArchitectureError(configuration.arch);           

        return ( generatorName, newArgs )
