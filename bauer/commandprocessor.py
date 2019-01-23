import error
import logging
import os, sys
import shutil
import subprocess
from distutils.spawn import find_executable

from generatorstate import GeneratorState
from buildexecutor import BuildExecutor
from buildconfiguration import BuildConfiguration
from desktoprunner import DesktopRunner
from androidexecutor import AndroidExecutor
from androidrunner import AndroidRunner
from iosrunner import IOSRunner
from codesigner import CodeSigner



class CommandProcessor:
    def __init__(self, bauerGlobals, generatorInfo, args, rootPath, sourceFolder, buildFolder):
        self.args = args
        self.logger = logging.getLogger(__name__)
        self.bauerGlobals = bauerGlobals
        self.generatorInfo = generatorInfo
        self.buildFolder = buildFolder
        self.rootPath = rootPath
        self.sourceFolder = sourceFolder
        self.buildExecutor = BuildExecutor(generatorInfo, rootPath, sourceFolder, buildFolder)
        self.androidExecutor = AndroidExecutor(self.buildExecutor, generatorInfo, sourceFolder, buildFolder)

        self.defaultLadder = {
            ('android', None) : ('android', 'AndroidStudio'),
            (None, 'AndroidStudio') : ('android', 'AndroidStudio'),
            ('ios', None) : ('ios', 'Xcode'),
            (None, 'Xcode') : ('ios', 'Xcode'),
            ('mac', None) : ('mac', 'Xcode')
        }

        if sys.platform == 'darwin':
            self.defaultLadder[(None, None)] = ('ios', 'Xcode')
        else:
            self.defaultLadder[(None, None)] = ('android', 'AndroidStudio')

    def process(self):
        command = self.args.command;

        self.logger.debug('Starting command: %s', command)

        selectedConfigurations = self.buildFolder.getBuildConfigurationsForCommand();
        self.logger.debug("Selected platform list:")
        for config in selectedConfigurations:
            self.logger.debug("* %s", config)

        if len(selectedConfigurations)==0:
            defaultLadderKey = (self.args.platform, self.args.build_system)
            architecture = self.args.arch
            configuration = self.args.config
            if not architecture:
                architecture = "std"

            if defaultLadderKey in self.defaultLadder:
                defaults = self.defaultLadder[defaultLadderKey]

                self.logger.info("Defaulting to: %s - %s" % (defaults[0], defaults[1]))
                selectedConfigurations = [
                    BuildConfiguration(platform=defaults[0], arch=architecture, buildsystem=defaults[1], config=configuration)
                ]
            else:
                raise error.IncorrectCallError("Could not determine default platform / buildsystem ( and configuration ). Please specifiy one via -p, -b (and -c)")

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
                elif command=="package":
                    self.prepare(configuration, platformState);
                    self.package(configuration);
                elif command=="codesign":
                    self.prepare(configuration, platformState);
                    self.codesign(configuration)
                elif command=="copy":
                    self.copy(buildDirectory)
                elif command=="open":
                    self.prepare(configuration, platformState)
                    self.open(configuration, buildDirectory)
                else:
                    raise error.ProgramArgumentError("Invalid command: '%s'" % command);


    def prepare(self, configuration, platformState):
        buildDirectory = self.buildFolder.getBuildDir(configuration)
        self.logger.info("Preparing %s", buildDirectory)
        
        if not os.path.isdir(buildDirectory):
            os.makedirs(buildDirectory);

        if configuration.platform == "android":
            self.androidExecutor.prepare(platformState, configuration, self.args)
        else:
            self.buildExecutor.prepare(platformState, configuration, self.args)

    def build(self, configuration):
        if configuration.platform == "android":
            self.androidExecutor.build(configuration, self.args)
        else:
            self.buildExecutor.build(configuration, self.args)

    def package(self, configuration):
        if configuration.platform == "android":
            self.androidExecutor.package(configuration, self.args)
        else:
            self.buildExecutor.package(configuration, self.args)

    def clean(self, configuration):
        if configuration.platform == "android":
            self.androidExecutor.clean(configuration)
        else:
            self.buildExecutor.clean(configuration, self.args)

    def distClean(self, buildDirectory):
        self.logger.info("Cleaning %s" % (buildDirectory))
        if os.path.isdir(buildDirectory):
            shutil.rmtree(buildDirectory);

    def codesign(self, configuration):
        codeSigner = CodeSigner(self.buildExecutor.cmake.codeModel)
        codeSigner.sign(self.args)

    def open(self, configuration, buildDirectory):
        cmake = self.buildExecutor.cmake if not configuration.platform == "android" else self.androidExecutor.cmake

        if configuration.buildsystem == 'Xcode':
            for cmakeConfig in cmake.codeModel['configurations']:
                project_file_name = os.path.join(buildDirectory, cmakeConfig['main-project']['name'] + ".xcodeproj")
                self.logger.debug("Starting: %s", project_file_name)
                self.bauerGlobals.open_file(project_file_name)
                return

        elif configuration.buildsystem == 'AndroidStudio':
            self.logger.debug("Looking for 'studio'")

            if sys.platform == "win32":
                defaultWindowsPath = "C:\\Program Files\\Android\\Android Studio\\bin\\studio64.exe"
                if os.path.exists(defaultWindowsPath):
                    subprocess.Popen([defaultWindowsPath, buildDirectory])
                    return
                else:
                    self.logger.warning("Couldn't find Android Studio, opening project folder")
            else:
                studio_path = find_executable('studio')
                self.logger.debug("Path: %s", studio_path)
                
                if studio_path == None:
                    self.logger.warning("Couldn't find 'studio', please install via Android Studio => Tools => Create Command-line launcher")
                else:
                    subprocess.Popen([studio_path, buildDirectory])
                    return

        self.logger.debug("Trying to open %s", buildDirectory)
        self.bauerGlobals.open_file(buildDirectory)

    def buildDeps(self):
        pass

    def run(self, configuration):
        if not self.args.target:
            raise error.ProgramArgumentError("Please specify a module name with --module MODULE")
        
        if not self.args.platform:
            raise error.ProgramArgumentError("Please specify the platform name with --platform PLATFORM")
        
        if not self.args.config:
            raise error.ProgramArgumentError("Please specify the configuration name with --config CONFIG")

        if configuration.platform == "ios":
            if configuration.arch == "device":
                raise error.ProgramArgumentError("Can't run on ios devices yet")
            if configuration.arch == "std" and configuration.buildsystem == 'Xcode':
                raise error.ProgramArgumentError("Can't run on ios devices yet, specifiy architecture 'simulator' to run.")

            iosRunner = IOSRunner(self.buildExecutor.cmake)
            exitCode = iosRunner.run(configuration, self.args)

        elif configuration.platform == "android":
            if configuration.buildsystem == "AndroidStudio":
                androidRunner = AndroidRunner(self.buildFolder, self.androidExecutor)
                exitCode = androidRunner.run(configuration, self.args)
            else:
                self.logger.critical("Only AndroidStudio configurations can be run")
                exit(1)

        else:
            appRunner = DesktopRunner(self.buildExecutor.cmake, configuration, self.args)
            exitCode = appRunner.run()

        if exitCode != 0:
            raise error.ErrorWithExitCode( "Application failed with exit code: 0x{:02x}".format(exitCode), exitCode)

    def copy(self, buildDirectory):
        destFolder = os.path.join(buildDirectory, os.path.basename(self.args.folder))
        self.logger.info("Copying %s into %s", self.args.folder, destFolder)
        shutil.copytree(self.args.folder, destFolder)

