import error
import logging
import os
import shutil

from generatorstate import GeneratorState
from buildexecutor import BuildExecutor
from buildconfiguration import BuildConfiguration
from desktoprunner import DesktopRunner
from androidexecutor import AndroidExecutor
from androidrunner import AndroidRunner
from emscriptenrunner import EmscriptenRunner
from iosrunner import IOSRunner

class CommandProcessor:
    def __init__(self, bauerGlobals, generatorInfo, emscriptenInfo, args, sourceFolder, buildFolder):
        self.args = args
        self.logger = logging.getLogger(__name__)
        self.bauerGlobals = bauerGlobals
        self.generatorInfo = generatorInfo
        self.buildFolder = buildFolder
        self.buildExecutor = BuildExecutor(generatorInfo, emscriptenInfo, sourceFolder, buildFolder)
        self.androidExecutor = AndroidExecutor(self.buildExecutor, generatorInfo, sourceFolder, buildFolder)
        self.emscriptenInfo = emscriptenInfo

    def process(self):
        command = self.args.command;

        self.logger.debug('Starting command: %s', command)

        selectedConfigurations = self.buildFolder.getBuildConfigurationsForCommand();
        self.logger.debug("Selected platform list:")
        for config in selectedConfigurations:
            self.logger.debug("* %s", config)

        if len(selectedConfigurations)==0:
            if not self.args.platform:
                raise error.IncorrectCallError("--platform PLATFORM must be specified when prepare is first called.");
            if not self.args.build_system:
                raise error.IncorrectCallError("--build-system BUILDSYSTEM must be specified when prepare is first called.");

        for configuration in selectedConfigurations:
            if configuration.platform not in self.bauerGlobals.platformMap:
                raise error.InvalidPlatformNameError(configuration.platform);

            buildDirectory = self.buildFolder.getBuildDir(configuration)
    
            with GeneratorState(buildDirectory) as platformState:
                if not "build-configuration" in platformState.state or BuildConfiguration(*platformState.state["build-configuration"]) != configuration:
                    if os.path.exists(buildDirectory):
                        self.logger.info("Build system does not match the one used when the projects for this platform were first prepared. Cleaning existing build files.");
                        if "build-configuration" in platformState.state:
                            self.logger.debug("Old config: ", BuildConfiguration(*platformState.state["build-configuration"]))
                        else:
                            self.logger.debug("(No previous state found)")
                        self.logger.debug("New config: %s", configuration)
                        shutil.rmtree(buildDirectory);

                platformState.state["build-configuration"] = configuration;
        
                if command=="prepare":
                    self.prepare(configuration, platformState);

                elif command=="build":
                    self.prepare(configuration, platformState);
                    self.build(configuration);

                elif command=="clean":
                    self.clean(configuration);

                elif command=="distclean":
                    self.distClean(buildDirectory);

                elif command=="builddeps":
                    self.buildDeps(configuration);

                elif command=="run":
                    self.prepare(configuration, platformState);
                    #self.build(configuration);
                    self.run(configuration);
                else:
                    raise error.ProgramArgumentError("Invalid command: '%s'" % command);


    def prepare(self, configuration, platformState):
        buildDirectory = self.buildFolder.getBuildDir(configuration)
        self.logger.info("Preparing %s", buildDirectory)

        if not os.path.isdir(buildDirectory):
            os.makedirs(buildDirectory);

        if configuration.platform == "android":
            self.androidExecutor.prepare(platformState, configuration)
        else:
            self.buildExecutor.prepare(platformState, configuration)

    def build(self, configuration):
        if configuration.platform == "android":
            self.androidExecutor.build(configuration, self.args)
        else:
            self.buildExecutor.build(configuration, self.args)

    def clean(self, configuration):
        if configuration.platform == "android":
            self.androidExecutor.clean(configuration)
        else:
            self.buildExecutor.clean(configuration, self.args)

    def distClean(self, buildDirectory):
        self.logger.info("Cleaning %s" % (buildDirectory))
        if os.path.isdir(buildDirectory):
            shutil.rmtree(buildDirectory);

    def buildDeps(self):
        pass

    def run(self, configuration):
        if not self.args.module:
            raise error.ProgramArgumentError("Please specify a module name with --module MODULE")
        
        if not self.args.platform:
            raise error.ProgramArgumentError("Please specify the platform name with --platform PLATFORM")
        
        if not self.args.config:
            raise error.ProgramArgumentError("Please specify the configuration name with --config CONFIG")

        if configuration.platform == "ios":
            iosRunner = IOSRunner(self.buildFolder, self.buildExecutor.cmake)
            exitCode = iosRunner.run(configuration, self.args)

        elif configuration.platform == "webems":
            emscriptenRunner = EmscriptenRunner(self.buildFolder, self.buildExecutor.cmake, self.emscriptenInfo)
            exitCode = emscriptenRunner.run(configuration, self.args)

        elif configuration.platform == "android":
            androidRunner = AndroidRunner(self.buildFolder, self.androidExecutor)
            exitCode = androidRunner.run(configuration, self.args)

        else:
            appRunner = DesktopRunner(self.buildExecutor.cmake, configuration, self.args)
            exitCode = appRunner.run()

        if exitCode != 0:
            raise error.ErrorWithExitCode( "Application failed with exit code: 0x{:02x}".format(exitCode), exitCode)

