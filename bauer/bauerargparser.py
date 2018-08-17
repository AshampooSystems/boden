import argparse
import sys
import error
import logging



class HelpOptionUsed(Exception):
    def __init__(self):
        Exception.__init__(self, "Help option used.");


class BauerArgParser(argparse.ArgumentParser):
    def __init__(self, bauerGlobals, generatorInfo, *args, **kwargs):

        self.generatorInfo = generatorInfo
        self.bauerGlobals = bauerGlobals

        argparse.ArgumentParser.__init__(self, usage = self.getUsage(), *args, **kwargs);
        self.add_argument("command", choices=["prepare", "build", "clean", "distclean", "builddeps", "run"] );

        self.add_argument("--platform" );
        self.add_argument("--build-system" );
        self.add_argument("--config", choices=["Debug", "Release"] );
        self.add_argument("--arch" );

        self.add_argument("--module" );

        self.add_argument("--run-output-file" );
        self.add_argument("--run-android-fetch-output-from" );

        self.add_argument("--ios-device-type")
        self.add_argument("--ios-simulator-os")

        self.add_argument('-j', '--jobs', dest='multi' );

        self.add_argument('-d', '--enable-debug-output', dest='debug', action='store_true')

        self.add_argument("--build-folder")
        
        self.add_argument("params", nargs="*" );


    def exit(self, status=0, message=None):
        if status==0:
            # user has used the "help" option.
            raise HelpOptionUsed();
        raise error.ProgramArgumentError(message);

    def error(self, message=None):
        raise error.ProgramArgumentError(message);

    def getPlatformHelp(self):
        platformHelp = "";
        for platformName, platformInfo in self.bauerGlobals.platformList:
            platformHelp+="  %s: %s\n" % (platformName, platformInfo)

        return platformHelp;


    def print_help(self):
        logging.info(self.getUsage());

    def getUsage(self):
        return """Usage: build.py COMMAND [PARAMS]

COMMAND can be one of the following:

--- Command: prepare ---

build.py prepare [--platform TARGET] [--buildsystem BUILDSYSTEM] [--arch ARCH]

Pepares the project files for the specified TARGET (see below). BUILDSYSTEM
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
files and switch to a different build system for this platform.


--- Command: build ---

build.py build [--platform TARGET] [--config CONFIG] [--arch ARCH]

Builds the specified configuration of the specified platform for the specified
architecture (ARCH).

If TARGET is omitted then all prepared platforms are built.

If CONFIG is omitted then all configurations are built.

If ARCH is omitted then all prepared architectures for the platform are built.


--- Command: clean ---

build.py clean [--platform TARGET] [--config CONFIG] [--arch ARCH]

Removes all intermediate and output files that are generated during building.
The project files remain.

The parameters TARGET, CONFIG and ARCH work exactly the same as with the
'build' command.



--- Command: distclean ---

build.py distclean [--platform TARGET] [--arch ARCH]

Like 'clean' but also removes the project files. This undoes everything
that 'build' and 'prepare' do.
If you want to use this platform again afterwards then you have to call
'prepare' again.

If TARGET is omitted then all prepared platforms are distcleaned.

If ARCH is omitted then all architectures for the selected platform(s) are
distcleaned.


--- Command: run ---

build.py [OPTIONS] -- run PARAMS

Runs the executable from the specified module.

PARAMS are one or more parameters to be passed to the executable.
On Android you can use the "{DATA_DIR}" placeholder in the parameters.
It will be replaced with the path of the application's data directory inside the emulator.

Note the double -- before "run" in the commandline. This is necessary to separate the
parameters for build.py from those that are intended for the executed module.

Options:

--build-folder folder     (optional) The base folder to build in

--module MODULE           (REQUIRED) MODULE is the name of the executable to run.
     
--platform TARGET         (optional) The target platform to run. If omitted then the module is run for all
                          prepared platforms (one after the other)
     
     
--config CONFIG           (optional) the configuration to run. If omitted then all configurations are run
                          (one after the other).
     
--arch arch               (optional) the target architecture for which the module should be run. If omitted
                          then all prepared architectures for the platform are run (one after the other).
     
--run-output-file         (optional) path to a file in which the module's output data is stored.
                          For most platform the output data is what is written to stdout.
                          On android this MUST be combined with --run-android-fetch-output-from.

--run-android-fetch-output-from
                          (optional) An android-only option. Specifies the path of a file in the android
                          file system that contains the output data of the app.
                          When the run command is complete then this is printed to stdout, unless
                          --run-output-file specifies the path of a file where the output should be written.
                          You can use "{DATA_DIR}" as a place holder for the application's data directory.

-j, --jobs n              (optional) Build concurrently with <n> threads

-d, --enable-debug-output (optional) Enable debug output

--ios-device-type         (optional) IOS Device type, e.g. "iPhone X"
--ios-simulator-os        (optional) IOS Simulator OS Version, e.g. "iOS 11.1"


--- Command: builddeps ---

Builds the necessary dependencies from the 3rdparty directory.


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
    std: at the time of this writing the same as x86_64 (but might
      change in future)
    armeabi: ARMv5TE based CPU with software floating point operations
    armeabi-v7a: ARMv7 based devices with hardware FPU instructions
      (VFPv3_D16)
    armeabi-v7a with NEON: same as armeabi-v7a, but sets NEON as floating-
      point unit
    armeabi-v7a with VFPV3: same as armeabi-v7a, but sets VFPv3_D32 as
      floating-point unit
    armeabi-v6 with VFP: tuned for ARMv6 processors having VFP
    x86: IA-32 instruction set
    mips: MIPS32 instruction set
    arm64-v8a: ARMv8 AArch64 instruction set - only for NDK r10 and newer
    x86_64: Intel64 instruction set (r1) - only for NDK r10 and newer
    mips64: MIPS64 instruction set (r6) - only for NDK r10 and newer
    

BUILDSYSTEM values:

%s
  AndroidStudio (for android platform only)

%s

RESTRICTIONS:
  ios platform: only the Xcode build system is supported
  webems platform: only the Makefile build systems are supported
  android platform: only the AndroidStudio build system is supported

IMPORTANT: Remember to enclose the build system names that consist of multiple
words in quotation marks!

""" % ( self.getPlatformHelp(), "\n".join(self.generatorInfo.generatorHelpList), self.generatorInfo.generatorAliasHelp );