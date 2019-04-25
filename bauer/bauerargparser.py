import argparse
import sys,os
import error
import logging


class EnvDefault(argparse.Action):
    def __init__(self, option_strings, required=False, default=None, **kwargs):
        envvar = "BAUER_" + option_strings[-1].upper().strip('-').replace('-', '_')

        if not default and envvar:
            if envvar in os.environ:
                default = os.environ[envvar]
                if 'nargs' in kwargs and kwargs['nargs'] == '+':
                    default = [default]

        if required and default:
            required = False
        super(EnvDefault, self).__init__(default=default, required=required, option_strings=option_strings,
                                         **kwargs)

    def __call__(self, parser, namespace, values, option_string=None):
        setattr(namespace, self.dest, values)


class HelpOptionUsed(Exception):
    def __init__(self):
        Exception.__init__(self, "Help option used.");

class BauerArgParser():
    def __init__(self, bauerGlobals, generatorInfo):
        self.logger = logging.getLogger(__name__)
        
        self.parser = None

        self.generatorInfo = generatorInfo
        self.bauerGlobals = bauerGlobals

    def parse_args(self):
        options =self.parser.parse_args()

        if options.command == None:
          self.parser.print_help()
          return None

        if options.command == "manual":
          print(self.getManual())
          return None

        self.logger.debug("Bauer options: %s", options)

        return options

    def buildBauerArguments(self, args, **kwargs):
          prog = args[0]
          self.parser = argparse.ArgumentParser(
            prog=prog,
            description="Use '%s COMMAND --help' to get help for a specific command." % prog,
            formatter_class=argparse.RawDescriptionHelpFormatter);
          self.buildCommandParsers()

    def setBaseParser(self, parser):
          self.parser = parser

    def buildGlobalArguments(self, parsers):
        for parser in parsers:
          parser.add_argument('-d', '--enable-debug-output', dest='debug', action='store_true', help="Enable debug output")
          parser.add_argument('--accept-terms', action='store_true', help="Automatically accept license terms for third party components. If this is not specified then an interactive prompt is displayed when the components are needed.")
    
    def addBaseConfigurationArguments(self, parsers, platforms=None, require=False):
        for parser in parsers:
          if platforms == None:
            platforms = self.bauerGlobals.platformMap.keys()

          configGroup = parser.add_argument_group('Compile target')
          configGroup.add_argument('-p', "--platform", action=EnvDefault, help="The target platform", choices=platforms, required=require );
          configGroup.add_argument('-b', "--build-system", action=EnvDefault, help="The cmake generator", required=require );
          configGroup.add_argument('-c', "--config", action=EnvDefault, choices=["Debug", "Release"], required=require );
          configGroup.add_argument('-a', "--arch", action=EnvDefault, help="The target architecture ( default: 'std' )", required=require );
          configGroup.add_argument('-D', "--cmake-option", action='append', help="Allows to set additional cmake options")

          buildFolderGroup = parser.add_argument_group('Build folder', "(optional)")
          buildFolderGroup.add_argument("--build-folder", action=EnvDefault, help="The buildfolder root (default: ./build)")

    def addConfigurationArguments(self, parsers, platforms=None):
        self.addBaseConfigurationArguments(parsers, platforms, False)

        for parser in parsers:
          macGroup = parser.add_argument_group('Mac OSX specific', "(optional)")
          macGroup.add_argument("--macos-sdk-path", action=EnvDefault, help="The Mac OSX SDK Path")
          macGroup.add_argument("--macos-min-version", action=EnvDefault, help="The target minimum Mac OSX version")

          packageGroup = parser.add_argument_group('Packaging', "(optional)")
          packageGroup.add_argument("--package-generator", action=EnvDefault, help="The CPack Generator" )
          packageGroup.add_argument("--package-folder", action=EnvDefault, help="The CPack package output folder")


    def addBuildArguments(self, parsers):
        for parser in parsers:
          parser.add_argument('-j', '--jobs', action=EnvDefault, help="Number of concurrent jobs" );

    def addSimulatorArguments(self, parser):
        parser.add_argument("--run-output-file", action=EnvDefault, help="Output file to store stdout" );

    def addIOSSimulatorArguments(self, parser):
        parser.add_argument("--ios-device-type", action=EnvDefault, help="IOS Device type")
        parser.add_argument("--ios-simulator-os", action=EnvDefault, help="IOS Simulator SDK version")
        parser.add_argument("--ios-device-id", action=EnvDefault, help="IOS Simulator Device id (overrides --ios-device-type and --ios-simulator-os")

    def addAndroidSimulatorArguments(self, parser):
        parser.add_argument("--run-android-fetch-output-from", action=EnvDefault, help="?" );

    def addParams(self, parser):
        parser.add_argument("params", nargs="*", help="Parameters to be passed to the executable being run " );

    def addKeychainArgument(self, parsers, **kwargs):
        for parser in parsers:
            parser.add_argument('-k', '--keychain', action=EnvDefault, help='path to the keychain', **kwargs)

    def addPasswordArgument(self, parsers, **kwargs):
        for parser in parsers:
            parser.add_argument('--password', action=EnvDefault, help='The password for the keychain', **kwargs)

    def buildCommandParsers(self):
        subs = self.parser.add_subparsers(
          title=None,
          metavar="COMMAND",
          help='The command to execute.',
          dest='command')

        command_info = [
          ('prepare', 'Prepares boden build', self.getPrepareEpilog()),
          ('build', 'Builds boden', self.getBuildEpilog()),
          ('clean', 'Clears build files', self.getCleanEpilog()),
          ('distclean', 'Clears build files and project files', self.getDistCleanEpilog()),
          ('run', "Executes one of bodens targets", self.getRunEpilog()),
          ('package', "Packages boden for release", self.getPackageEpilog()),
          ('copy', "Copy a folder into the build folder", self.getCopyEpilog()),
          ('open', "Open the project files", None),
          ('manual', "Shows futher information", None),
          ('new', "Builds a template project", self.getNewEpilog()),
          ('doc', "Builds and opens the documentation", None)
        ]

        if sys.platform == 'darwin':
          command_info += [
            ('codesign', 'Sign libraries and executables', None)
          ]

        command_map = {}
        for command, desc, epilog in command_info:
          command_map[command] = subs.add_parser(
            command,
            description=desc,
            help=desc,
            epilog=epilog,
            prog=self.parser.prog+" "+command,
            formatter_class=argparse.RawDescriptionHelpFormatter)

        prepare = command_map["prepare"]
        build = command_map["build"]
        clean = command_map["clean"]
        distclean = command_map["distclean"]
        run = command_map["run"]
        package = command_map["package"]
        copy = command_map["copy"]
        open_project = command_map["open"]
        
        doc = command_map["doc"]
        doc.add_argument('-a', '--addr', help="Address to serve pages on", default="127.0.0.1:8000")

        new = command_map["new"]
        new.add_argument('-f', '--project-folder', help="Where to create the project", default=".")
        new.add_argument('-n', '--project-name', help="Name of the project", default="Untitled")

        if sys.platform == 'darwin':
          sign = command_map["codesign"]
          sign.add_argument('-i', '--identity', action=EnvDefault, help="The Identity to pass to codesign", required=True, nargs="+")
          self.addKeychainArgument([sign])
          self.addPasswordArgument([sign])
          self.addConfigurationArguments([sign], platforms=['ios', 'mac'])

        self.addBaseConfigurationArguments( [ copy ], platforms=None, require=False )
        self.addConfigurationArguments( [ prepare, build, clean, distclean, run, package, open_project ])
        self.addBuildArguments( [ build, clean, distclean, run, package ])

        simGroup = run.add_argument_group("Simulator", "(optional)")

        self.addSimulatorArguments(simGroup)
        self.addIOSSimulatorArguments(simGroup)
        self.addAndroidSimulatorArguments(simGroup)

        self.addParams(run)
        list(map(lambda p: p.add_argument('-t', "--target", help="The target to build/run" ), [prepare, run, build, open_project]))

        copy.add_argument('-f', '--folder', help="Source folder to copy", required=True );

        info = command_map["manual"]

        self.buildGlobalArguments([ self.parser, prepare, build, clean, distclean, run, package, open_project, doc ])

    def getPlatformHelp(self):
        platformHelp = "";
        for platformName, platformInfo in self.bauerGlobals.platformList:
            platformHelp+="  %s: %s\n" % (platformName, platformInfo)

        return platformHelp;

    def getPrepareEpilog(self):
      return """Pepares the project files for the specified TARGET (see below). BUILDSYSTEM
specifies the build system or IDE you would like to use (see below for
possible values).

ARCH is the name of the architecture / ABI to build for.
"std" is supported for all platforms and all build systems. It causes the
build to be done for the "standard" architecture(s). Which architectures that
will be depends on the build system and platform.

If ARCH is not specified then "std" is used.

prepare can be called multiple times with different platforms to set
up the project files for multiple platforms.

When you first execute the prepare command you must specify both the
TARGET and BUILDSYSTEM parameters. On latter calls you can omit BUILDSYSTEM or
both TARGET and BUILDSYSTEM.

When prepare is called without parameters then it refreshes the project
files for all platforms for which project files have been previously prepared.

When platform is specified and BUILDSYSTEM is omitted then only the project files
for the specified platform are refreshed.

You can also call prepare with a different BUILDSYSTEM for a platform that is
already prepared. That will remove the existing project files and build
files and switch to a different build system for this platform."""

    def getBuildEpilog(self):
      return """Builds the specified configuration of the specified platform for the specified
architecture (ARCH).

If TARGET is omitted then all prepared platforms are built.

If CONFIG is omitted then all configurations are built.

If ARCH is omitted then all prepared architectures for the platform are built."""

    def getCleanEpilog(self):
      return """Removes all intermediate and output files that are generated during building.
The project files remain.

The parameters TARGET, CONFIG and ARCH work exactly the same as with the
'build' command."""

    def getDistCleanEpilog(self):
      return """Like 'clean' but also removes the project files. This undoes everything
that 'build' and 'prepare' do.
If you want to use this platform again afterwards then you have to call
'prepare' again.

If TARGET is omitted then all prepared platforms are distcleaned.

If ARCH is omitted then all architectures for the selected platform(s) are
distcleaned."""

    def getRunEpilog(self):
      return """Runs the executable from the specified target.

PARAMS are one or more parameters to be passed to the executable.
On Android you can use the "{DATA_DIR}" placeholder in the parameters.
It will be replaced with the path of the application's data directory inside the emulator.

Note the double -- before "run" in the commandline. This is necessary to separate the
parameters for build.py from those that are intended for the executed target."""

    def getPackageEpilog(self):
        return """"""

    def getCopyEpilog(self):
        return """"""

    def getNewEpilog(self):
        return """"""

    def getManual(self):
        return """Usage: build.py COMMAND [PARAMS]

COMMAND can be one of the following:

--- Command: prepare ---

build.py prepare [--platform TARGET] [--buildsystem BUILDSYSTEM] [--arch ARCH]

%s


--- Command: build ---

build.py build [--platform TARGET] [--config CONFIG] [--arch ARCH]

%s


--- Command: clean ---

build.py clean [--platform TARGET] [--config CONFIG] [--arch ARCH]

%s


--- Command: distclean ---

build.py distclean [--platform TARGET] [--arch ARCH]

%s


--- Command: run ---

build.py [OPTIONS] -- run PARAMS

%s


--- Command: package ---

build.py package [--platform TARGET] [--config CONFIG] [--arch ARCH]

%s


--- Parameter values ---

TARGET values:

%s

CONFIG values:

  Debug: debug build
  Release: release build

ARCH values:

  Supported values depend on the build system and platform. If a platform is not
  listed then it only supports the "std" architecture.

  windows with Visual Studio build system:
    std: 32 bit program (x86)
    x64: 64 bit program (x64)
    arm: program for ARM processors

  ios:
    std: normal iOS app (combined 32 and 64 bit binary)

  android:  
    std: generates a multi-architecture project, targetting all modern ABIs
    armeabi-v7a: ARMv7 based devices with hardware FPU instructions
    arm64-v8a: ARMv8 AArch64 instruction set - only for NDK r10 and newer
    x86: IA-32 instruction set
    x86_64: Intel64 instruction set (r1) - only for NDK r10 and newer
   

BUILDSYSTEM values:

%s
  AndroidStudio (for android platform only)

%s

IMPORTANT: Remember to enclose the build system names that consist of multiple
words in quotation marks!

""" % ( self.getPrepareEpilog(), self.getBuildEpilog(), self.getCleanEpilog(), self.getDistCleanEpilog(), self.getRunEpilog(), self.getPackageEpilog(), self.getPlatformHelp(), "\n".join(self.generatorInfo.generatorHelpList), self.generatorInfo.generatorAliasHelp );
