#! /usr/bin/python

import sys;
import subprocess;
import os.path;
import shutil;
import json;
import argparse;
import traceback;
import time;

EXIT_PROGRAM_ARGUMENT_ERROR = 1;
EXIT_CMAKE_PROBLEM = 10;
EXIT_TOOL_FAILED = 11;
EXIT_INCORRECT_CALL = 12;



targetList = [ ("windows-universal", "Universal Windows app (Windows 10 and later)" ),
               ("windows-store", "Windows Store app (Windows 8.1 and later)" ),
               ("windows-classic", "Classic Windows desktop program"),               
               ("dotnet", ".NET program" ),
               ("linux", "Linux" ),
               ("osx", "Mac OSX" ),
               ("ios", "iPhone, iPad" ),
               ("android", "Android devices" ),
               ("web", """\
Compiles the C++ code to a Javascript-based web app or Javascript
    library that can be run either in a web browser or on a Node.js system.
    The resulting JS code is pure Javascript. No native components or
    plugins are needed to execute it.""") ];

targetMap = {};
for targetName, targetInfo in targetList:
    targetMap[targetName] = targetInfo;



def deleteDirTree(dirPath):

    for i in range(10):
        try:
            shutil.rmtree(dirPath);
            break;
        except Exception:
            if i==9:
                raise;

            time.sleep(1);



class ErrorWithExitCode(Exception):
    def __init__(self, exitCode, errorMessage):
        Exception.__init__(self, errorMessage);
        self.exitCode = exitCode;


class ProgramArgumentError(ErrorWithExitCode):
    def __init__(self, errorMessage):        
        if not errorMessage:
            errorMessage = "program argument error";

        ErrorWithExitCode.__init__(self, EXIT_PROGRAM_ARGUMENT_ERROR, errorMessage);
        self.errorMessage = errorMessage;


class CMakeProblemError(ErrorWithExitCode):
    def __init__(self, e):        
        ErrorWithExitCode.__init__(self, EXIT_CMAKE_PROBLEM, "There was a problem calling cmake. CMake is required and must be installed to run this.");


class ToolFailedError(ErrorWithExitCode):
    def __init__(self, toolName, toolExitCode):                
        ErrorWithExitCode.__init__(self, EXIT_TOOL_FAILED, "%s failed with exit code %d" % (toolName, toolExitCode) );
        self.toolName = toolName;
        self.toolExitCode = toolExitCode;


class InvalidTargetNameError(ProgramArgumentError):
    def __init__(self, targetName):
        ProgramArgumentError.__init__(self, "Invalid target name: '%s'" % targetName);


class InvalidConfigNameError(ProgramArgumentError):
    def __init__(self, targetName):
        ProgramArgumentError.__init__(self, "Invalid config name: '%s'" % configName);



class IncorrectCallError(ErrorWithExitCode):
    def __init__(self, errorMessage):
        ErrorWithExitCode.__init__(self, EXIT_INCORRECT_CALL, errorMessage);


def getStatePath(dir):
    return os.path.join(dir, ".generateProjects.state");


def loadState(dir):
    state = {};

    p = getStatePath(dir);
    if os.path.exists(p):
        with open(p, "rb") as f:
            state = json.loads( f.read().decode("utf-8") );

    return state;


def storeState(dir, state):
    p = getStatePath(dir);
    with open(p, "wb") as f:
        f.write( json.dumps( state ).encode("utf-8") );



def changePython2ToPython(dirPath):
    prefix = "#!/usr/bin/env python2\n";
    # note that the replacement prefix MUST be the same size!
    replacementPrefix = prefix.replace("python2", "python ");

    prefix = prefix.encode("utf-8");
    replacementPrefix = replacementPrefix.encode("utf-8");

    for name in os.listdir(dirPath):
        if name!="." and name!="..":
            itemPath = os.path.join(dirPath, name);

            if os.path.isdir(itemPath):
                if name!="include":
                    changePython2ToPython(itemPath);

            else:
                title, extension = os.path.splitext(name);
                if not extension or extension==".py":

                    # could be a python file.
                    with file(itemPath, "r+") as f:
                        data = f.read(len(prefix));
                        if data==prefix:
                            f.seek(0);
                            f.write(replacementPrefix);

                            print("Updated %s" % itemPath);

                        f.close();




class GeneratorInfo(object):
    def __init__(self):
        try:
            cmakeHelp = subprocess.check_output("cmake --help", shell=True, universal_newlines=True);

        except subprocess.CalledProcessError as e:
            raise CMakeProblemError(e);

        cmakeHelp = cmakeHelp.strip();

        # the generator list is at the end of the output. And the entries are all indented.
        self.generatorHelpList = [];
        for line in reversed( cmakeHelp.splitlines() ):
            if not line or not line.startswith(" "):
                break;

            self.generatorHelpList.append(line);

        self.generatorHelpList.reverse();

        self.generatorNames = [];    
        for line in self.generatorHelpList:
            if line.startswith("  ") and len(line)>2 and line[2]!=" ":
                line = line.strip();

                name, sep, rest = line.partition(" = ");
                if sep:
                    name = name.strip();
                    if name:
                        self.generatorNames.append(name);

        self.generatorAliasMap = {};
        vsPrefix = "Visual Studio ";
        for name in self.generatorNames:        
            if name.startswith(vsPrefix):
                words = name[len(vsPrefix):].strip().split();
                if len(words)>=2:
                    try:
                        internalVersion = int(words[0]);
                        yearVersion = int(words[1]);

                        self.generatorAliasMap[ "vs"+words[1] ] = vsPrefix+words[0]+" "+words[1];
                    except Exception as e:
                        # ignore exceptions. The generator string does not have the expected format.
                        pass;


        self.generatorAliasMap["make"] = "Unix Makefiles"
        self.generatorAliasMap["nmake"] = "NMake Makefiles";
        self.generatorAliasMap["msysmake"] = "MSYS Makefiles";
        self.generatorAliasMap["mingwmake"] = "MinGW Makefiles";

        if "CodeBlocks - Unix Makefiles" in self.generatorNames:
            self.generatorAliasMap["codeblocks"] = "CodeBlocks - Unix Makefiles"


        if "CodeLite - Unix Makefiles" in self.generatorNames:
            self.generatorAliasMap["codelite"] = "CodeLite - Unix Makefiles"
            

        self.generatorAliasHelp = "Aliases for build system names:\n";
        for aliasName in sorted( self.generatorAliasMap.keys() ):
            self.generatorAliasHelp += "\n%s = %s" % (aliasName, self.generatorAliasMap[aliasName]);




def getTargetHelp():
    targetHelp = "";
    for targetName, targetInfo in targetList:
        targetHelp+="  %s: %s\n" % (targetName, targetInfo)

    return targetHelp;

def getMainDir():
    return os.path.dirname( os.path.abspath(__file__) );

def getBaseBuildDir():
    return os.path.join(getMainDir(), "build");

def getTargetBuildDir(targetName, arch):

    targetBuildDirName = targetName;
    if arch and arch!="std":
        targetBuildDirName += "_"+arch;

    return os.path.join( getBaseBuildDir(), targetBuildDirName );



def splitBuildDirName(name):

    target, sep, arch = name.partition("_");

    if target not in targetMap:
        target = None;
        arch = None;

    else:
        if not arch:
            arch = "std";

    return (target, arch);



def getCMakeDir():
    return os.path.join( getMainDir(), "cmake");


def isSingleConfigBuildSystem(buildSystem):
    generatorName = generatorInfo.generatorAliasMap.get(buildSystem, buildSystem);    

    generatorName = generatorName.lower();

    if "makefile" in generatorName or "ninja" in generatorName:
        return True;

    else:
        return False;



def commandPrepare(commandArgs):

    targetAndArchList = getTargetAndArchListForCommand(commandArgs);

    if len(targetAndArchList)==0:

        if commandArgs.target:
            arch = commandArgs.arch;
            if not arch:
                arch = "std";

            targetAndArchList = [ (commandArgs.target, "std") ];

        else:
            raise IncorrectCallError("TARGET must be specified when prepare is first called.");


    for target, arch in targetAndArchList:

        buildSystem = commandArgs.buildsystem;

        if target not in targetMap:
            raise InvalidTargetNameError(target);

        if not arch:
            arch = "std";

        targetBuildDir = getTargetBuildDir(target, arch);

        targetState = loadState(targetBuildDir);
        oldBuildSystem = targetState.get("buildSystem", "");

        if buildSystem and oldBuildSystem and buildSystem!=oldBuildSystem:

            # user wants to switch toolset. We must delete the target build dir.
            print("Build system does not match the one used when the projects for this target were first prepared. Cleaning existing build files.");

            shutil.rmtree(targetBuildDir);
            oldBuildSystem = "";

        if not buildSystem:

            # use default for target.
            if not oldBuildSystem:
                raise IncorrectCallError("BUILDSYSTEM must be specified when prepare is first called for a target.");

            buildSystem = oldBuildSystem;

        targetState["buildSystem"] = buildSystem;

        singleConfig = isSingleConfigBuildSystem(buildSystem);
        targetState["singleConfigBuildSystem"] = singleConfig;

        if not os.path.isdir(targetBuildDir):
            os.makedirs(targetBuildDir);
        storeState(targetBuildDir, targetState);

        configList = [];
        if singleConfig:
            configList = ["Debug", "Release"];
        else:
            configList = [""];

        generatorName = generatorInfo.generatorAliasMap.get(buildSystem, buildSystem);

        for config in configList:

            if oldBuildSystem:
                verb = "Refreshing";
            else:
                verb = "Preparing"


            msg = "%s target %s for arch '%s'" % (verb, target, arch);            
            if config:
                msg += " and config %s " % config;
            msg += " (build system: '%s')..." %  buildSystem;

            print(msg);

            cmakeBuildDir = targetBuildDir;
            if config:
                cmakeBuildDir = os.path.join(cmakeBuildDir, config);

            toolChainFileName = None;
            toolChainFilePath = None;

            envSetupPrefix = "";

            cmakeArch = None;

            args = [];
            
            if target.startswith("windows-"):

                if arch!="std":

                    if "Visual Studio" in generatorName:

                        # note: passing the architecture with -A does not work properly.
                        # Cmake cannot find a compiler if we do it that way. does not find a compiler.
                        # So instead we pass it in the generator name.

                        if arch=="x64":
                            #cmakeArch = "x64";
                            generatorName += " Win64"
                            
                        elif arch=="arm":
                            #cmakeArch = "ARM";
                            generatorName += " ARM"                            
                        else:
                            raise InvalidArchitectureError(arch);

                    else:
                        raise InvalidArchitectureError(arch);

                if target=="windows-store":
                    args.extend( ["-DCMAKE_SYSTEM_NAME=WindowsStore", "-DCMAKE_SYSTEM_VERSION=8.1" ] );
                    
                elif target=="windows-universal":
                    args.extend( ["-DCMAKE_SYSTEM_NAME=WindowsStore", "-DCMAKE_SYSTEM_VERSION=10.0" ] );                     

            elif target=="osx":

                if arch!="std":
                    raise InvalidArchitectureError(arch);

            elif target=="ios":

                if arch=="std":
                    platform = "OS";

                elif arch=="sim32":
                    platform = "SIMULATOR";

                elif arch=="sim64":
                    platform = "SIMULATOR64";

                else:
                    raise InvalidArchitectureError(arch);

                cmakeArch = None;

                args.extend( [ "-DIOS_PLATFORM="+platform ] );

                toolChainFileName = "iOS.cmake";

            elif target=="web":

                if arch!="std":
                    raise InvalidArchitectureError(arch);

                # prepare the emscripten SDK (if not yet prepared)
                mainDir = getMainDir();
                emsdkDir = os.path.join(mainDir, "3rdparty_build", "emsdk");

                if not os.path.isdir(emsdkDir):
                    print("Setting up Emscripten SDK. This can take a while...")

                    try:
                        emsdkSourceDir = os.path.join(mainDir, "3rdparty", "emsdk");

                        shutil.copytree(emsdkSourceDir, emsdkDir);

                        emsdkExePath = os.path.join(emsdkDir, "emsdk");

                        subprocess.check_call( '"%s" update' % emsdkExePath, shell=True, cwd=emsdkDir);

                        subprocess.check_call( '"%s" install latest' % emsdkExePath, shell=True, cwd=emsdkDir);

                        subprocess.check_call( '"%s" activate latest' % emsdkExePath, shell=True, cwd=emsdkDir);


                    except:

                        for i in range(30):

                            try:
                                shutil.rmtree(emsdkDir);
                                break;

                            except:                                
                                time.sleep(1);

                        raise;

                    print("Emscripten was successfully set up.");

                if sys.platform=="win32":
                    envSetupPrefix = '"%s" activate latest && ' % emsdkExePath;
                else:
                    envSetupPrefix = "source "+os.path.join(emsdkDir, "emsdk_env.sh") + " && ";


                # the emscripten scrips call python2. However, python is not available
                # under that name on all platforms. So we add an alias
                try:
                    subprocess.check_call("python2 --version", shell="True");
                    havePython2 = True;
                except Exception:
                    havePython2 = False;

                if not havePython2:
                    print("Python2 executable is named just 'python'. Changing references...")
                    
                    # change python2 references to python
                    changePython2ToPython(emsdkDir);


                toolChainFileName = "Emscripten.cmake";

            elif target=="android":
                toolChainFileName = "android.cmake";

                if arch!="std":
                    args.extend( ['-DANDROID_ABI='+arch ] );


            elif target=="dotnet":
                args.extend( ['-DBODEN_TARGET=dotnet' ] );

                if arch!="std":
                    raise InvalidArchitectureError(arch);


            args = ["-G", generatorName, getCMakeDir() ] + args[:];


            if toolChainFileName:
                toolChainFilePath = os.path.join(getCMakeDir(), toolChainFileName);               

            if toolChainFilePath:
                if not os.path.isfile(toolChainFilePath):
                    print("Required CMake toolchain file not found: "+toolChainFilePath);
                    return 5;

                args.extend( ["-DCMAKE_TOOLCHAIN_FILE="+toolChainFilePath] );


            if config:
                args.extend( ["-DCMAKE_BUILD_TYPE="+config ] );

            if cmakeArch:
                args.extend( ["-A "+cmakeArch ] );

            # we do not validate the toolset name
            commandLine = "cmake";
            for a in args:
                commandLine += ' "%s"' % (a);

            commandLine = envSetupPrefix+commandLine;

            if not os.path.isdir(cmakeBuildDir):
                os.makedirs(cmakeBuildDir);


            print("## Calling CMake:\n  "+commandLine+"\n");

            exitCode = subprocess.call(commandLine, cwd=cmakeBuildDir, shell=True);
            if exitCode!=0:
                raise ToolFailedError("cmake", exitCode);


            if target=="dotnet":
                # the project must have an empty calling convention in order to be compilable with /clr:pure.
                # By default it does not contain ANY calling convention entry, which will cause Visual Studio
                # to treat it like /Gd (which causes an error during compilation). There does not seem to be
                # a way to cause CMake to generate the Callingconvention entry, so we have to do it ourselves.

                for name in os.listdir(cmakeBuildDir):
                    if name.endswith(".vcxproj"):
                        filePath = os.path.join(cmakeBuildDir, name);

                        with open( filePath, "rb") as f:
                            data = f.read().decode("utf-8");

                        modified = False;
                        if "<CallingConvention />" not in data and "</ClCompile>" in data:

                            print("Modifying %s (adding required empty CallingConvention entry)..." % name);
                            data = data.replace("</ClCompile>", "  <CallingConvention />\n    </ClCompile>");                            

                            modified = True;

                        if "<CLRSupport>" not in data and "<UseOfMfc>" in data:
                            print("Modifying %s (adding CLRSupport entry)..." % name);
                            data = data.replace("<UseOfMfc>", "<CLRSupport>Pure</CLRSupport>\n    <UseOfMfc>");                            

                            modifed = True;

                        if modified:
                            with open( filePath, "wb") as f:
                                f.write(data.encode("utf-8"));





def getFullToolsetName(toolsetName):
    return generatorInfo.generatorAliasMap.get(toolsetName, toolsetName);    



def getPreparedTargetsAndArchs():
    buildDir = getBaseBuildDir();

    preparedList = [];

    if os.path.isdir(buildDir):
        for name in os.listdir(buildDir):            
            target, arch = splitBuildDirName(name);
            if target and target in targetMap:
                preparedList.append( (target, arch) );

    return preparedList;


def getTargetAndArchListForCommand(args):

    preparedTargetsAndArchs = getPreparedTargetsAndArchs();

    targetList = [];
    if args.target:
        targetList.append( args.target );

    else:
        for preparedTarget, preparedArch in preparedTargetsAndArchs:
            targetList.append( preparedTarget );


    targetAndArchList = [];

    for target in targetList:
        if args.arch:
            targetAndArchList.append( (target, args.arch) );

        else:

            for preparedTarget, preparedArch in preparedTargetsAndArchs:
                if preparedTarget==target:
                    targetAndArchList.append( (target, preparedArch) );


    return targetAndArchList;



def commandBuildOrClean(command, args):

    for targetName, arch in getTargetAndArchListForCommand(args):

        if targetName not in targetMap:
            raise InvalidTargetNameError(targetName);

        targetBuildDir = getTargetBuildDir(targetName, arch);

        targetState = loadState(targetBuildDir);
        singleConfigBuildSystem = targetState.get("singleConfigBuildSystem", False);

        if singleConfigBuildSystem and not args.config:
            configList = ["Debug", "Release"];
        else:
            configList = [args.config];

        for config in configList:

            if singleConfigBuildSystem:
                cmakeBuildDir = os.path.join(targetBuildDir, config);
            else:
                cmakeBuildDir = targetBuildDir;

            commandLine = "cmake --build "+cmakeBuildDir;

            if command=="clean":
                commandLine += " --target clean";

            if config:
                commandLine += " --config "+config;

            if config:
                print("Calling cmake --build for config %s" % config);
            else:
                print("Calling cmake --build for all configs");

            print(commandLine);

            exitCode = subprocess.call(commandLine, shell=True, cwd=cmakeBuildDir);
            if exitCode!=0:
                raise ToolFailedError("cmake --build", exitCode);



def commandBuild(args):
    commandBuildOrClean("build", args);


def commandClean(args):
    commandBuildOrClean("clean", args);


def commandDistClean(args):
    for targetName, arch in getTargetAndArchListForCommand(args):
        if targetName not in targetMap:
            raise InvalidTargetNameError(targetName);

        targetBuildDir = getTargetBuildDir(targetName, arch);
        if os.path.isdir(targetBuildDir):
            shutil.rmtree(targetBuildDir);



def getUsage():
    return """Usage: build.py COMMAND [PARAMS]

COMMAND can be one of the following:

--- Command: prepare ---

build.py prepare [--target TARGET] [--buildsystem BUILDSYSTEM] [--arch ARCH]

Pepares the project files for the specified TARGET (see below). BUILDSYSTEM
specifies the build system or IDE you would like to use (see below for
possible values).

ARCH is the name of the architecture / ABI to build for.
"std" is supported for all targets and all build systems. It causes the
build to be done for the "standard" architecture(s). Which architectures that
will be depends on the build system and target.

If ARCH is not specified then "std" is used.

prepare can be called multiple times with different targets to set
up the project files for multiple targets.

When you first execute the prepare command you must specify both the
TARGET and BUILDSYSTEM parameters. On latter calls you can omit BUILDSYSTEM or
both TARGET and BUILDSYSTEM.

When prepare is called without parameters then it refreshes the project
files for all targets for which project files have been previously prepared.

When target is specified and BUILDSYSTEM is omitted then only the project files
for the specified target are refreshed.

You can also call prepare with a different BUILDSYSTEM for a target that is
already prepared. That will remove the existing project files and build
files and switch to a different build system for this target.


--- Command: build ---

build.py build [--target TARGET] [--config CONFIG] [--arch ARCH]

Builds the specified configuration of the specified target for the specified
architecture (ARCH).

If TARGET is omitted then all prepared targets are built.

If CONFIG is omitted then all configurations are built.

If ARCH is omitted then all prepared architectures for the target are built.


--- Command: clean ---

build.py clean [--target TARGET] [--config CONFIG] [--arch ARCH]

Removes all intermediate and output files that are generated during building.
The project files remain.

The parameters TARGET, CONFIG and ARCH work exactly the same as with the
'build' command.


--- Command: distclean ---

build.py distclean [--target TARGET] [--arch ARCH]

Like 'clean' but also removes the project files. This undoes everything
that 'build' and 'prepare' do.
If you want to use this target again afterwards then you have to call
'prepare' again.

If TARGET is omitted then all prepared targets are distcleaned.

If ARCH is omitted then all architectures for the selected target(s) are
distcleaned.


--- Parameter values ---

TARGET values:

%s

CONFIG values:

  Debug: debug build
  Release: release build

ARCH values:

  Supported values depend on the build system and target. If a target is not
  listed then it only supports the "std" architecture.

  windows with Visual Studio build system:
    std: 32 bit program (x86)
    x64: 64 bit program (x64)
    arm: program for ARM processors

  ios:
    std: normal iOS app (combined 32 and 64 bit binary)
    sim32: build for 32 bit simulator
    sim64: build for 64 bit simulator

  android:  
    std: at the time of this writing the same as armeabi-v7a (but might
      change in future
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

%s

RESTRICTIONS:
  ios target: only the Xcode build system is supported
  web target: only the Makefile build systems are supported
  android target: only the Makefile build systems are supported

IMPORTANT: Remember to enclose the build system names that consist of multiple
words in quotation marks!


  """ % ( getTargetHelp(), "\n".join(generatorInfo.generatorHelpList), generatorInfo.generatorAliasHelp );



class HelpOptionUsed(Exception):
    def __init__(self):
        Exception.__init__(self, "Help option used.");


class MyArgParser(argparse.ArgumentParser):
    def __init__(self, *args, **kwargs):
        argparse.ArgumentParser.__init__(self, usage = getUsage(), *args, **kwargs);

    def exit(self, status=0, message=None):
        if status==0:
            # user has used the "help" option.
            raise HelpOptionUsed();
        raise ProgramArgumentError(message);

    def error(self, message=None):
        raise ProgramArgumentError(message);


    def print_help(self):
        print(getUsage());



def main():

    print("");    



    try:
        global generatorInfo;
        generatorInfo = GeneratorInfo();

        
        argParser = MyArgParser();
        argParser.add_argument("command", choices=["prepare", "build", "clean", "distclean"] );

        argParser.add_argument("--target" );
        argParser.add_argument("--buildsystem" );
        argParser.add_argument("--config", choices=["Debug", "Release"] );
        argParser.add_argument("--arch" );

        args = argParser.parse_args();

        command = args.command;

        if command=="prepare":
            commandPrepare(args);

        elif command=="build":
            commandBuild(args);

        elif command=="clean":
            commandClean(args);

        elif command=="distclean":
            commandDistClean(args);

        else:
            raise ProgramArgumentError("Invalid command: '%s'" % command);

    except HelpOptionUsed:
        return 0;

    except ProgramArgumentError as e:
        print( "\n"+str(e)+"\n" );        
        print('Call with --help to get help information.');

        return e.exitCode;

    except ErrorWithExitCode as e:
        print( "\n"+str(e)+"\n" );
        return e.exitCode;


    except Exception as e:        
        traceback.print_exc();
        return 50;



exitCode = main();
if exitCode!=0:
    sys.exit(exitCode);





