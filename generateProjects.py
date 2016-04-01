#! /usr/bin/python

import sys;
import subprocess;
import os.path;
import shutil;
import json;


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


def main():

    print("");

    try:
        cmakeHelp = subprocess.check_output("cmake --help", shell=True, universal_newlines=True);

    except subprocess.CalledProcessError as e:
        print("There was a problem calling cmake. Cmake is required and must be installed to run this.\n")
        return 3;

    cmakeHelp = cmakeHelp.strip();


    # the generator list is at the end of the output. And the entries are all indented.
    generatorHelpList = [];
    for line in reversed( cmakeHelp.splitlines() ):
        if not line or not line.startswith(" "):
            break;

        generatorHelpList.append(line);

    generatorHelpList.reverse();

    generatorNames = [];    
    for line in generatorHelpList:
        if line.startswith("  ") and len(line)>2 and line[2]!=" ":
            line = line.strip();

            name, sep, rest = line.partition(" = ");
            if sep:
                name = name.strip();
                if name:
                    generatorNames.append(name);

    generatorAliasMap = {};
    vsPrefix = "Visual Studio ";
    for name in generatorNames:        
        if name.startswith(vsPrefix):
            words = name[len(vsPrefix):].strip().split();
            if len(words)>=2:
                try:
                    internalVersion = int(words[0]);
                    yearVersion = int(words[1]);

                    generatorAliasMap[ "vs"+words[1] ] = vsPrefix+words[0]+" "+words[1];
                except Exception as e:
                    # ignore exceptions. The generator string does not have the expected format.
                    pass;


    generatorAliasMap["make"] = "Unix Makefiles"
    generatorAliasMap["nmake"] = "NMake Makefiles";
    generatorAliasMap["msysmake"] = "MSYS Makefiles";
    generatorAliasMap["mingwmake"] = "MinGW Makefiles";

    if "CodeBlocks - Unix Makefiles" in generatorNames:
        generatorAliasMap["codeblocks"] = "CodeBlocks - Unix Makefiles"


    if "CodeLite - Unix Makefiles" in generatorNames:
        generatorAliasMap["codelite"] = "CodeLite - Unix Makefiles"
        

    generatorAliasHelp = "Aliases for toolset names:\n";
    for aliasName in sorted( generatorAliasMap.keys() ):
        generatorAliasHelp += "\n%s = %s" % (aliasName, generatorAliasMap[aliasName]);


    targetInfoList = [  ("windows", "", [""]),
                        ("linux",     "", [""]),
                        ("osx",     "", [""]),
                        ("ios",     "", ["", "sim64", "sim32"]),
                        ("android", "", [""]),
                        ("web",     "", [""]),
                    ];

    targetHelp = "";
    targetMap = {};
    for target, info, subTargetList in targetInfoList:
        targetHelp+="  "+target;
        if info:
            targetHelp+="  [%s]" % info;
        targetHelp+="\n";

        targetMap[target] = subTargetList;


    if len(sys.argv)<2 or len(sys.argv)>3:

        print("""Usage: generateProjects.py TARGETNAME [TOOLSET]

TARGETNAME can be one of the following:

%s
TOOLSET is the build toolset / IDE you want to use. This MUST be specified when
generateProjects is first called. It can be omitted on latter calls.
The toolset is passed to CMAKE as the generator name and can be one of the
following values on your system:

%s

%s

  RESTRICTIONS:
    ios target: only the Xcode toolset is supported
    web target: only the Makefile toolsets are supported
    android target: only the Makefile toolsets are supported

  IMPORTANT: Remember to enclose the toolset names that consist of multiple
  words in quotation marks!

You can also specify "-" for TARGETNAME and/or TOOLSET to use the value from
the previous execution of generateProjects.py.

  """ % ( targetHelp, "\n".join(generatorHelpList), generatorAliasHelp ));
        return 1;

    targetName = sys.argv[1].lower();
    if len(sys.argv)>=3:
        toolsetName = sys.argv[2];
    else:
        toolsetName = "-";

    myPath = os.path.abspath(__file__);
    mainDir = os.path.dirname(myPath);

    state = loadState(mainDir);

    if targetName=="-":
        if "lastTarget" not in state:
            print("No previous target stored. Specify the target explictly.")
            return 4;            
        targetName = state["lastTarget"];


    if targetName not in targetMap:
        print("Invalid target name.")
        return 1;

    state["lastTarget"] = targetName;

    storeState(mainDir, state);

    
    buildDir = os.path.join(mainDir, "build");
    cmakeDir = os.path.join(mainDir, "cmake");

    if not os.path.isdir(buildDir):
        os.makedirs(buildDir);


    targetBuildDir = os.path.join(buildDir, targetName);
    if os.path.isdir(targetBuildDir):
        targetState = loadState(targetBuildDir);
    else:
        targetState = {};

    toolsetNameFromTargetState = targetState.get("toolset", None);

    if toolsetName=="-":
        if not toolsetNameFromTargetState:
            print("No previous toolset stored. Specify the toolset option explictly.\n")
            return 4; 
        toolsetName = toolsetNameFromTargetState;

    else:
        if toolsetNameFromTargetState and toolsetName!=toolsetNameFromTargetState:
            print("Toolset does not match the one used when the projects for this target were first created. Cleaning existing build files.");

            shutil.rmdir(targetBuildDir);

    if not os.path.isdir(targetBuildDir):
        os.makedirs(targetBuildDir);
        
    targetState["toolset"] = toolsetName;

    storeState( targetBuildDir, state);
    
    fullToolsetName = generatorAliasMap.get(toolsetName, toolsetName);    

    fixedConfigList = [];

    if "makefile" in fullToolsetName.lower() or "ninja" in fullToolsetName.lower():
        # per-configuration generator. I.e. the build type (debug/release) is fixed.
        fixedConfigList = ["Debug", "Release"];
    
    else:
        # multi-configuration generator. No need to specify build type at config time.
        fixedConfigList = [""];


    subTargetList = targetMap[targetName];

    for fixedConfig in fixedConfigList:

        for subTarget in subTargetList:

            toolsetBuildDir = os.path.join(buildDir, targetName);

            subDirNameParts = [];

            if fixedConfig:
                subDirNameParts.append(fixedConfig);

            if subTarget:
                subDirNameParts.append(subTarget);                

            if len(subDirNameParts)>0:
                subDirName = "-".join(subDirNameParts);            
                toolsetBuildDir = os.path.join(toolsetBuildDir, subDirName);

            if not os.path.isdir(toolsetBuildDir):
                os.makedirs(toolsetBuildDir);

            args = ["-G", fullToolsetName, cmakeDir];

            toolChainFileName = None;
            toolChainFilePath = None;

            envSetupPrefix = "";

            if targetName=="ios":

                if subTarget=="":
                    platform = "OS";

                elif subTarget=="sim32":
                    platform = "SIMULATOR";

                elif subTarget=="sim64":
                    platform = "SIMULATOR64";

                else:
                    assert("bad subtarget");

                args.extend( [    "-DIOS_PLATFORM="+platform ] );

                toolChainFileName = "iOS.cmake";

            elif targetName=="web":

                # prepare the emscripten SDK (if not yet prepared)
                emsdkDir = os.path.join(mainDir, "3rdparty_build", "emsdk");

                if not os.path.isdir(emsdkDir):
                    print("Setting up emscripten...")

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

            elif targetName=="android":
                toolChainFileName = "android.cmake";


            if toolChainFileName:
                toolChainFilePath = os.path.join(cmakeDir, toolChainFileName);               

            if toolChainFilePath:
                if not os.path.isfile(toolChainFilePath):
                    print("Required CMake toolchain file not found: "+toolChainFilePath);
                    return 5;

                args.extend( ["-DCMAKE_TOOLCHAIN_FILE="+toolChainFilePath] );


            if fixedConfig:
                args.extend( ["-DCMAKE_BUILD_TYPE="+fixedConfig ] );

            # we do not validate the toolset name
            commandLine = "cmake";
            for a in args:
                commandLine += ' "%s"' % (a);

            commandLine = envSetupPrefix+commandLine;


            print("## Calling CMake:\n  "+commandLine+"\n");

            exitCode = subprocess.call(commandLine, cwd=toolsetBuildDir, shell=True);
            if exitCode!=0:
                print("\ncmake failed with exit code %d\n" % (exitCode));
                return 2;

            print("");


exitCode = main();
if exitCode!=0:
    sys.exit(exitCode);



