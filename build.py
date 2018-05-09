#! /usr/bin/python

from __future__ import print_function

import sys;
import subprocess;
import os.path;
import shutil;
import json;
import argparse;
import traceback;
import time;
import tempfile;
import zipfile
import stat



EXIT_PROGRAM_ARGUMENT_ERROR = 1;
EXIT_CMAKE_PROBLEM = 10;
EXIT_TOOL_FAILED = 11;
EXIT_INCORRECT_CALL = 12;
EXIT_PREPARED_STATE_ERROR = 13;



platformList = [ ("winuwp", "Universal Windows app (Windows 10 and later)" ),
               ("win32", "Classic Windows desktop program (despite the 32 in the name, this also includes 64 bit Windows desktop programs)"),               
               ("dotnet", ".NET program" ),
               ("linux", "Linux" ),
               ("mac", "Apple Mac OS (formerly OSX)" ),
               ("ios", "iPhone, iPad" ),
               ("android", "Android devices" ),
               ("webems", """\
Compiles the C++ code to a Javascript-based web app or Javascript
    library that can be run either in a web browser or on a Node.js system.
    The resulting JS code is pure Javascript. No native components or
    plugins are needed to execute it.""") ];

platformMap = {};
for platformName, platformInfo in platformList:
    platformMap[platformName] = platformInfo;



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



class PreparedStateError(ErrorWithExitCode):
    def __init__(self, errorMessage):        
        if not errorMessage:
            errorMessage = "prepared state error";

        ErrorWithExitCode.__init__(self, EXIT_PREPARED_STATE_ERROR, errorMessage);
        self.errorMessage = errorMessage;




class ProgramArgumentError(ErrorWithExitCode):
    def __init__(self, errorMessage):        
        if not errorMessage:
            errorMessage = "program argument error";

        ErrorWithExitCode.__init__(self, EXIT_PROGRAM_ARGUMENT_ERROR, errorMessage);
        self.errorMessage = errorMessage;


class CMakeProblemError(ErrorWithExitCode):
    def __init__(self, e, errorOutput=None):        
        ErrorWithExitCode.__init__(self, EXIT_CMAKE_PROBLEM, "There was a problem calling cmake. CMake is required and must be installed to run this." + ( (" Error output: "+errorOutput) if errorOutput else "" ) )


class ToolFailedError(ErrorWithExitCode):
    def __init__(self, toolName, toolExitCode):                
        ErrorWithExitCode.__init__(self, EXIT_TOOL_FAILED, "%s failed with exit code %d" % (toolName, toolExitCode) );
        self.toolName = toolName;
        self.toolExitCode = toolExitCode;


class InvalidPlatformNameError(ProgramArgumentError):
    def __init__(self, platformName):
        ProgramArgumentError.__init__(self, "Invalid platform name: '%s'" % platformName);



class InvalidArchitectureError(ProgramArgumentError):
    def __init__(self, arch):
        ProgramArgumentError.__init__(self, "Invalid architecture name: '%s'" % arch);


class InvalidConfigNameError(ProgramArgumentError):
    def __init__(self, platformName):
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

                            print("Updated %s" % itemPath, file=sys.stderr);

                        f.close();




class GeneratorInfo(object):
    def __init__(self):

        self.generatorHelpList = [];
        self.generatorNames = [];    
        self.generatorAliasMap = {};

        self._cmakeError = None

        errOutput = ""
        try:    
            errOutFile, errOutFilePath = tempfile.mkstemp()            
            try:
                cmakeHelp = subprocess.check_output("cmake --help", shell=True, universal_newlines=True, stderr=errOutFile );
            finally:
                os.close(errOutFile)
                try:
                    with open(errOutFilePath, "r") as f:
                        errOutput = f.read().strip();
                except:
                    pass
                os.remove(errOutFilePath)          

        except subprocess.CalledProcessError as e:            
            self._cmakeError = CMakeProblemError(e, errOutput);


        
        if self._cmakeError is None:
            cmakeHelp = cmakeHelp.strip();

            # the generator list is at the end of the output. And the entries are all indented.

            for line in reversed( cmakeHelp.splitlines() ):
                if not line or not line.startswith(" "):
                    break;

                self.generatorHelpList.append(line);

            self.generatorHelpList.reverse();

            
            for line in self.generatorHelpList:
                if line.startswith("  ") and len(line)>2 and line[2]!=" ":
                    line = line.strip();

                    name, sep, rest = line.partition(" = ");
                    if sep:
                        name = name.strip();
                        if name:
                            self.generatorNames.append(name);

        
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


    def ensureHaveCmake(self):
        if self._cmakeError is not None:
            raise self._cmakeError


def getPlatformHelp():
    platformHelp = "";
    for platformName, platformInfo in platformList:
        platformHelp+="  %s: %s\n" % (platformName, platformInfo)

    return platformHelp;

def getMainDir():
    return os.path.dirname( os.path.abspath(__file__) );

def getBaseBuildDir():
    return os.path.join(getMainDir(), "build");

def getPlatformBuildDir(platformName, arch):

    platformBuildDirName = platformName;
    if arch and arch!="std":
        platformBuildDirName += "_"+arch;

    return os.path.join( getBaseBuildDir(), platformBuildDirName );



def splitBuildDirName(name):

    platform, sep, arch = name.partition("_");

    if platform not in platformMap:
        platform = None;
        arch = None;

    else:
        if not arch:
            arch = "std";

    return (platform, arch);



def getCMakeDir():
    return os.path.join( getMainDir(), "cmake");


def isSingleConfigBuildSystem(buildSystem):

    if buildSystem=="AndroidStudio":
        return False;

    generatorName = generatorInfo.generatorAliasMap.get(buildSystem, buildSystem);    

    generatorName = generatorName.lower();

    if "makefile" in generatorName or "ninja" in generatorName:
        return True;

    else:
        return False;


def parseVersionOutput(out, expectedVersionComponents, preferLast):

    out = out.strip();

    firstLine = list(out.splitlines())[0];


    selectedVer = None;

    for word in firstLine.split():

        word = word.replace("-", ".");

        comps = word.split(".");

        ver = [];

        for c in comps:
            try:
                ver.append( int(c) );
            except:
                break;

        if len(ver)>=expectedVersionComponents:
            selectedVer = ver;

            if not preferLast:
                break;



    if selectedVer==None:
        return None;
    else:
        return tuple(selectedVer);



def getGCCVersion():
    try:
        out = subprocess.check_output("gcc --version", shell=True);

        return parseVersionOutput(out, 3, preferLast=True );

    except:
        return None;


def getClangVersion():
    try:
        out = subprocess.check_output("clang --version", shell=True);

        return parseVersionOutput(out, 2, preferLast=False );

    except:
        return None;



def commandPrepare(commandArgs):

    platformAndArchList = getPlatformAndArchListForCommand(commandArgs);

    if len(platformAndArchList)==0:

        if commandArgs.platform:
            arch = commandArgs.arch;
            if not arch:
                arch = "std";

            platformAndArchList = [ (commandArgs.platform, "std") ];

        else:
            raise IncorrectCallError("--platform PLATFORM must be specified when prepare is first called.");


    for platform, arch in platformAndArchList:

        buildSystem = commandArgs.build_system;

        if platform not in platformMap:
            raise InvalidPlatformNameError(platform);

        if not arch:
            arch = "std";

        platformBuildDir = getPlatformBuildDir(platform, arch);

        platformState = loadState(platformBuildDir);
        oldBuildSystem = platformState.get("buildSystem", "");

        if buildSystem and oldBuildSystem and buildSystem!=oldBuildSystem:

            # user wants to switch toolset. We must delete the platform build dir.
            print("Build system does not match the one used when the projects for this platform were first prepared. Cleaning existing build files.", file=sys.stderr);

            shutil.rmtree(platformBuildDir);
            oldBuildSystem = "";

        if not buildSystem:

            if oldBuildSystem:
                # use old build system
                buildSystem = oldBuildSystem;

            else:
                # if there is only one choice for the platform then use that.
                # Otherwise issue an error
                if platform=="android":
                    buildSystem = "AndroidStudio";

                else:
                    raise IncorrectCallError("--build-system BUILDSYSTEM must be specified when prepare is first called for a platform.");
            
        if platform=="android" and buildSystem!="AndroidStudio":
            raise IncorrectCallError("--build-system parameter must be 'AndroidStudio' for the android platform.");


        platformState["buildSystem"] = buildSystem;

        singleConfig = isSingleConfigBuildSystem(buildSystem);
        platformState["singleConfigBuildSystem"] = singleConfig;

        if not os.path.isdir(platformBuildDir):
            os.makedirs(platformBuildDir);
        storeState(platformBuildDir, platformState);

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

            msg = "%s platform %s for arch '%s'" % (verb, platform, arch);            
            if config:
                msg += " and config %s " % config;
            msg += " (build system: '%s')..." %  buildSystem;

            print(msg, file=sys.stderr);

            prepareFunc = prepareCmake;
            if platform=="android":
                prepareFunc = prepareAndroid;

            prepareFunc(platform, config, arch, platformBuildDir, buildSystem);


def download_file(url, file_path):

    if sys.version_info[0]>=3:
        import urllib.request

        urllib.request.urlretrieve(
                        url,
                        file_path)

    else:

        import urllib2

        resp = urllib2.urlopen(url)

        with open(file_path, "wb") as f:
            shutil.copyfileobj(resp, f)




_gradle_path = None

def get_gradle_path():

    global _gradle_path

    if _gradle_path is None:            

        # first try to call an installed gradle version.

        outFile, outFilePath = tempfile.mkstemp()
        try:
            result = subprocess.call("gradle --version", shell=True, stdout=outFile, stderr=subprocess.STDOUT)
        finally:
            os.close(outFile)
            os.remove(outFilePath)

        if result==0:
            _gradle_path = "gradle"

        else:

            # no system gradle

            main_dir = getMainDir();

            gradle_base_dir = os.path.join(main_dir, "3rdparty_build", "gradle")

            gradle_version_name = "gradle-4.1"

            gradle_path = os.path.join(gradle_base_dir, gradle_version_name, "bin", "gradle");
            if sys.platform == "win32":
                gradle_path += ".bat"

            if not os.path.exists(gradle_path):

                if not os.path.isdir(gradle_base_dir):
                    os.makedirs(gradle_base_dir)

                print("Downloading gradle...", file=sys.stderr)

                gradle_zip_file_name = gradle_version_name+"-bin.zip"

                gradle_download_file = os.path.join(gradle_base_dir, gradle_zip_file_name)

                download_file(
                    "https://services.gradle.org/distributions/"+gradle_zip_file_name,
                    gradle_download_file)

                print("Extracting gradle...", file=sys.stderr)

                zipf = zipfile.ZipFile(gradle_download_file, 'r')
                zipf.extractall(gradle_base_dir)
                zipf.close()

                os.remove(gradle_download_file)

                if sys.platform!="win32":
                    os.chmod(gradle_path, stat.S_IXUSR|stat.S_IRUSR)

                if not os.path.exists(gradle_path):
                    raise Exception("Gradle executable not found after fresh download. Expected here: "+gradle_path)

            _gradle_path = gradle_path

    return _gradle_path



class AndroidStudioProjectGenerator(object):

    def __init__(self, platformBuildDir):
        self._projectDir = platformBuildDir;

        
    def getGradleDependency(self):
        return "classpath 'com.android.tools.build:gradle:3.0.1'"



    def generateTopLevelProject(self, moduleNameList):
        if not os.path.isdir(self._projectDir):
            os.makedirs(self._projectDir);

        # the underlying commandline build system for android is gradle.
        # Gradle uses a launcher script (called the gradle wrapper)
        # that enforces the use of a specific desired gradle version.
        # The launcher script will automatically download the correct
        # version if needed and use that.
        # Any version of gradle can generate the wrapper for any desired
        # version.
        # So now we need to generate the gradle wrapper for "our" version.
        # Right now we use gradle 4.1

        # Unfortunately gradle has the problem that it will try to load the build.gradle files if
        # it finds them in the build directory, even if we only want to generate the wrapper.
        # And loading the build.gradle may fail if the currently installed default
        # gradle version is incorrect.
        # So to avoid this problem we generate the wrapper in a temporary directory and then move it to the desired location.



        gradle_temp_dir = tempfile.mkdtemp();
        try:

            gradle_path = get_gradle_path()
        
            subprocess.check_call(
                '"%s" wrapper --gradle-distribution-url "https://services.gradle.org/distributions/gradle-4.1-all.zip"' % (gradle_path),
                shell=True,
                cwd=gradle_temp_dir);

            for name in os.listdir(gradle_temp_dir):
                source_path = os.path.join( gradle_temp_dir, name)
                dest_path = os.path.join( self._projectDir, name)
                if os.path.isdir(dest_path):
                    shutil.rmtree(dest_path)
                elif os.path.exists(dest_path):
                    os.remove(dest_path)

                shutil.move( source_path, dest_path )

        finally:
            shutil.rmtree(gradle_temp_dir)        


        with open( os.path.join(self._projectDir, "build.gradle"), "w" ) as f:
            f.write("""\
// Top-level build file where you can add configuration options common to all sub-projects/modules.

buildscript {
    
    repositories {
        google()
        jcenter()
    }
    dependencies {
        $$GradleDependency$$
        

        // NOTE: Do not place your application dependencies here; they belong
        // in the individual module build.gradle files
    }
}

allprojects {
    repositories {
        google()
        jcenter()
    }
}

task clean(type: Delete) {
    delete rootProject.buildDir
}

    """.replace("$$GradleDependency$$", self.getGradleDependency()) )

        includeParams = "";
        for moduleName in moduleNameList:
            if includeParams:
                includeParams += ", ";
            includeParams += "':%s'" % moduleName

        with open( os.path.join(self._projectDir, "settings.gradle"), "w" ) as f:
            f.write("""\
    include $$IncludeParams$$

    """.replace("$$IncludeParams$$", includeParams) )




    def generateModule(self, projectModuleName, packageId, moduleName, additionalSourceModuleNames, userFriendlyModuleName, dependencyList, isLibrary):        

        moduleDir = os.path.join(self._projectDir, projectModuleName);
        if not os.path.isdir(moduleDir):
            os.makedirs(moduleDir);

        with open( os.path.join(moduleDir, "build.gradle"), "w" ) as f:
            f.write( self.getModuleBuildGradleCode(projectModuleName, packageId, moduleName, additionalSourceModuleNames, dependencyList, isLibrary ) )

        #with open( os.path.join(moduleDir, "CMakeLists.txt"), "w" ) as f:
        #    f.write( self.getModuleCMakeListsCode(packageId, moduleName, additionalSourceModuleNames, dependencyList, isLibrary ) )
            

        srcMainDir = os.path.join(moduleDir, "src", "main");
        if not os.path.isdir(srcMainDir):
            os.makedirs(srcMainDir);

        with open( os.path.join(srcMainDir, "AndroidManifest.xml"), "w" ) as f:
            f.write( self.getModuleAndroidManifest(packageId, moduleName, isLibrary ) )

        resDir = os.path.join(srcMainDir, "res");
        if not os.path.isdir(resDir):
            os.makedirs(resDir);

        valuesDir = os.path.join(resDir, "values");
        if not os.path.isdir(valuesDir):
            os.makedirs(valuesDir);

        with open( os.path.join(valuesDir, "strings.xml"), "w" ) as f:
            f.write("""\
<?xml version="1.0" encoding="utf-8"?>
<resources>
    <string name="app_name">$$UserFriendlyModuleName$$</string>
</resources>
""".replace("$$UserFriendlyModuleName$$", userFriendlyModuleName) )



    def getModuleAndroidManifest(self, packageId, moduleName, isLibrary):

        code = """\
<?xml version="1.0" encoding="utf-8"?>
<manifest xmlns:android="http://schemas.android.com/apk/res/android"
    package="$$PackageId$$">

          """;

        if not isLibrary:
            code += """
            <!-- project files generated manually with android studio
                would also have the following <application> attributed:
                android:icon="@mipmap/ic_launcher"
                android:roundIcon="@mipmap/ic_launcher_round"
                android:theme="@style/AppTheme"
                android:supportsRtl="true" -->
    <application
        android:allowBackup="true"
        android:label="@string/app_name" >
        <activity android:name="io.boden.android.NativeRootActivity"
                  android:label="@string/app_name"
                  android:configChanges="mcc|mnc|locale|touchscreen|keyboard|keyboardHidden|navigation|screenLayout|fontScale|uiMode|orientation|screenSize|smallestScreenSize|layoutDirection">

            <meta-data android:name="io.boden.android.lib_name"
              android:value="$$ModuleName$$" />

          <intent-filter>
            <action android:name="android.intent.action.MAIN" />
            <category android:name="android.intent.category.LAUNCHER" />
          </intent-filter>
        </activity>         
    </application>
      """;

        code += """    
    </manifest>
    """;

        return code.replace("$$PackageId$$", packageId) \
            .replace("$$ModuleName$$", moduleName)



    def getModuleBuildGradleCode(self, projectModuleName, packageId, moduleName, additionalSourceModuleNames, dependencyList, isLibrary):


        if isLibrary:
            pluginName = "com.android.library";
            appIdCode = ""; # libraries do not have an application id
        else:
            pluginName = "com.android.application";
            appIdCode = "applicationId = '%s'" % packageId

        moduleDependencyCode = "";
        for dep in dependencyList:            
            moduleDependencyCode += "    compile project(':%s')\n" % dep;

        cmakeTargets = '"%s"' % (moduleName);
        
        excludeSourceDirCode = "";

        excludeEntries = set();

        srcDir = os.path.join(self._projectDir, "..", "..", moduleName, "src" );
        for name in os.listdir( srcDir):
            if os.path.isdir( os.path.join(srcDir, name) ) and name not in ("android", "java", "pthread", "test"):
                excludeEntries.add("**/%s/**" % name);


        includeBdnDir = os.path.join(self._projectDir, "..", "..", moduleName, "include", "bdn" );
        if os.path.isdir(includeBdnDir):
            for name in os.listdir( includeBdnDir):
                if os.path.isdir( os.path.join(includeBdnDir, name) ) and name not in ("android", "java", "pthread", "test"):
                    excludeEntries.add("**/%s/**" % name);

        
        for entry in excludeEntries:
            excludeSourceDirCode += '                exclude "%s"\n' % (entry);

        srcDirCode = "                srcDirs = [ ";

        sourceModuleNames = [moduleName];
        sourceModuleNames.extend( additionalSourceModuleNames );

        srcDirList = [];
        for sourceModuleName in sourceModuleNames:
            srcDirList.append( "../../../%s/src" % sourceModuleName )
            srcDirList.append( "../../../%s/include" % sourceModuleName );

        for index, srcDir in enumerate(srcDirList):
            if index!=0:
                srcDirCode+=", "
            srcDirCode += "'%s'" % srcDir;

        srcDirCode += " ]";



        return """
apply plugin: '$$PluginName$$'

android {
    compileSdkVersion 26
    defaultConfig {
        $$AppIdCode$$
        minSdkVersion 16
        targetSdkVersion 26
        versionCode 1
        versionName "1.0"
        externalNativeBuild {
            cmake {
                targets $$CmakeTargets$$
                arguments "-DANDROID_STL=c++_static", "-DANDROID_CPP_FEATURES=rtti exceptions"
                cppFlags "-std=c++11 -frtti -fexceptions"     
                abiFilters 'x86' //, 'x86_64', 'armeabi', 'armeabi-v7a', 'arm64-v8a'
            }
        }
    }
    buildTypes {
        release {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'
        }
    }
    externalNativeBuild {
        cmake {            
            path "../../../cmake/CMakeLists.txt"
        }
    }


    sourceSets {
        main {
            java {
                srcDir '../../../$$ModuleName$$/java'
            }
            jni {
                srcDirs = ['../../../$$ModuleName$$/src',
                           '../../../$$ModuleName$$/include' ]
            }
        }
    }
}

dependencies {
    implementation fileTree(dir: 'libs', include: ['*.jar'])
    implementation 'com.android.support:appcompat-v7:26.1.0'
    implementation 'com.android.support.constraint:constraint-layout:1.0.2'

$$ModuleDependencyCode$$
}


""" .replace("$$AppIdCode$$", appIdCode) \
    .replace("$$PluginName$$", pluginName) \
    .replace("$$CmakeTargets$$", cmakeTargets) \
    .replace("$$ModuleName$$", moduleName) \
    .replace("$$SrcDirCode$$", srcDirCode) \
    .replace("$$ExcludeSourceDirCode$$", excludeSourceDirCode) \
    .replace("$$ModuleDependencyCode$$", moduleDependencyCode)


    def makeCMakePath(self, path):
        # we want the path to be relative to the CMakeLists file.
        # Note that the project dir is one level below the CMakeLists parent
        # dir, so we add another .. path component
        return os.path.join(  "..", os.path.relpath(path, self._projectDir) )


    def makeFileListForCMake(self, dirPathList, extensionList):
        fileList = []

        toDoDirPathList = dirPathList[:]
        while len(toDoDirPathList)!=0:
            dirPath = toDoDirPathList[0]
            del toDoDirPathList[0]

            if os.path.isdir(dirPath):

                for itemName in os.listdir( dirPath ):                    

                    itemPath = os.path.join(dirPath, itemName) 

                    if not os.path.isdir(itemPath):
                        ext = os.path.splitext(itemName)[1].lower()
                        if ext in extensionList:
                            relItemPath = self.makeCMakePath( itemPath )
                            fileList.append(relItemPath)

        return fileList


    def getModuleCMakeListsCode(self, packageId, moduleName, additionalSourceModuleNames, dependencyList, isLibrary):

        srcDirList = []
        headerDirList = []
        includeDirList = []
        for srcModuleName in [moduleName]+additionalSourceModuleNames:

            baseSrcDir = os.path.join(self._projectDir, "..", "..", srcModuleName, "src" )
            srcDirList.append( baseSrcDir )

            baseIncludeDir = os.path.join(self._projectDir, "..", "..", srcModuleName, "include" )
            includeDirList.append( baseIncludeDir )

            baseHeaderDir = os.path.join(baseIncludeDir, "bdn")
            headerDirList.append( baseHeaderDir )

            for name in ("android", "java", "pthread", "test"):
                srcDirList.append( os.path.join(baseSrcDir, name) )
                headerDirList.append( os.path.join(baseHeaderDir, name) )


        for depModuleName in dependencyList:
            baseIncludeDir = os.path.join(self._projectDir, "..", "..", depModuleName, "include" )
            includeDirList.append( baseIncludeDir )


        srcFileList = self.makeFileListForCMake(srcDirList, [".cpp", ".c", ".cxx"]  )
        headerFileList = self.makeFileListForCMake(headerDirList, [".h", ".hpp" ] )

        srcFileCode = "\n  ".join(srcFileList)
        headerFileCode = "\n  ".join(headerFileList)

        includeDirCode = ""
        for includeDir in includeDirList:
            if os.path.exists(includeDir):
                includeDirCode += "\n  "+self.makeCMakePath( includeDir )


        # Note that AndroidStudio 3.0 (and at least early versions of 3.1) has
        # a bug that it does not show header files in the tree on the left, unless
        # a .cpp file with the same name is in the same directory.
        # Listing header files in CMakeLists as source files does not help.
        # See doc_input/android_studio_header_files_bug.md for more information.


        return """ \
# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

add_definitions( -DUNICODE -D_UNICODE )

include_directories( $$IncludeDirCode$$ )

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
             $$ModuleName$$

             # Sets the library as a shared library.
             SHARED

  $$SrcFileCode$$ )

set_property(TARGET $$ModuleName$$ APPEND PROPERTY COMPILE_DEFINITIONS $<$<CONFIG:Debug>:BDN_DEBUG>)


# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              ANDROID_LOG_LIB

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       $$ModuleName$$

                       # Links the target library to the log library
                       # included in the NDK.
                       ${ANDROID_LOG_LIB} )

""" .replace("$$ModuleName$$", moduleName) \
    .replace("$$SrcFileCode$$", srcFileCode) \
    .replace("$$IncludeDirCode$$", includeDirCode )





def prepareAndroid(platform, config, arch, platformBuildDir, buildSystem):

    # The way Android Studio builds projects with native code has changed
    # quite a bit over time. It used to be very messy, requiring experimental
    # plugins to work properly.
    # Since AndroidStudio 3.0 this has changed and there is a "standard" way
    # to do this. AndroidStudio now includes a custom, forked CMake
    # that actually works - and this builds the native code side.
    # We only support the new style projects now (see git history if
    # you want to know what it used to look like).


    gen = AndroidStudioProjectGenerator(platformBuildDir);

    gen.generateTopLevelProject(["boden", "app", "testboden", "testbodenui", "testbodentiming"]);
    gen.generateModule("boden", "io.boden.android.boden", "boden", [], "Boden", [], True)
    gen.generateModule("app", "io.boden.android.uidemo", "uidemo", [], "UIDemo", ["boden"], False)
    gen.generateModule("testboden", "io.boden.android.testboden", "testboden", ["testboden_common"], "TestBoden", ["boden"], False)
    gen.generateModule("testbodenui", "io.boden.android.testbodenui", "testbodenui", ["testboden_common"], "TestBodenUI", ["boden"], False)
    gen.generateModule("testbodentiming", "io.boden.android.testbodentiming", "testbodentiming", ["testboden_common"], "TestBodenTiming", ["boden"], False)

    
def getEmscriptenSdkBaseDir():
    emsdkDir = os.environ.get("EMSDK_BASE_DIR")
    if not emsdkDir or not os.path.exists(emsdkDir):
        # use local emsdk installation
        emsdkDir = os.path.join(getMainDir(), "3rdparty_build", "emsdk");

    return emsdkDir

def prepareCmake(platform, config, arch, platformBuildDir, buildSystem):

    cmakeBuildDir = platformBuildDir;
    if config:
        cmakeBuildDir = os.path.join(cmakeBuildDir, config);

    toolChainFileName = None;
    toolChainFilePath = None;

    envSetupPrefix = "";

    cmakeArch = None;

    args = [];

    generatorInfo.ensureHaveCmake()

    generatorName = generatorInfo.generatorAliasMap.get(buildSystem, buildSystem);

    commandIsInQuote = False;
    
    if platform.startswith("win"):

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

        if platform=="winstore":
            args.extend( ["-DCMAKE_SYSTEM_NAME=WindowsStore", "-DCMAKE_SYSTEM_VERSION=8.1" ] );
            
        elif platform=="winuwp":
            args.extend( ["-DCMAKE_SYSTEM_NAME=WindowsStore", "-DCMAKE_SYSTEM_VERSION=10.0.10240.0" ] );                     

    elif platform=="mac":

        if arch!="std":
            raise InvalidArchitectureError(arch);

    elif platform=="ios":

        if arch!="std":
            raise InvalidArchitectureError(arch);

        cmakeArch = None;

        args.extend( [ "-DIOS_PLATFORM=SIMULATOR" ] );

        toolChainFileName = "iOS.cmake";

    elif platform=="android":
        toolChainFileName = "android.toolchain.cmake";
        if arch!="std":
            args.extend( ['-DANDROID_ABI='+arch ] );

        # API levels and reach as of December 2016
        # Target Level      Reach     Target Android Version
        # 16                97%       4.1 Jelly Bean
        # 19                81%       4.4 Kitkat

        # See https://www.statista.com/statistics/271774/share-of-android-platforms-on-mobile-devices-with-android-os/
        
        args.extend( [ '-DANDROID_NATIVE_API_LEVEL=16', '-DANDROID_STL=c++_static' ] );
        

    elif platform=="webems":

        if arch!="std":
            raise InvalidArchitectureError(arch);

        # prepare the emscripten SDK (if not yet prepared)
        mainDir = getMainDir();
        emsdkDir = getEmscriptenSdkBaseDir()

        emsdkExePath = os.path.join(emsdkDir, "emsdk");

        if not os.path.isdir(emsdkDir):
            print("Setting up Emscripten SDK. This can take a while...", file=sys.stderr);

            try:
                emsdkSourceDir = os.path.join(mainDir, "3rdparty", "emsdk");

                shutil.copytree(emsdkSourceDir, emsdkDir);

                subprocess.check_call( '"%s" update' % emsdkExePath, shell=True, cwd=emsdkDir);

                # subprocess.check_call( '"%s" install sdk-incoming-64bit' % emsdkExePath, shell=True, cwd=emsdkDir);
                # subprocess.check_call( '"%s" activate sdk-incoming-64bit' % emsdkExePath, shell=True, cwd=emsdkDir);
                # subprocess.check_call( '"%s" install sdk-master-64bit' % emsdkExePath, shell=True, cwd=emsdkDir);
                # subprocess.check_call( '"%s" activate sdk-master-64bit' % emsdkExePath, shell=True, cwd=emsdkDir);

                #ver = "1.36.6-64bit"
                ver = "1.37.39-64bit"                

                subprocess.check_call( '"%s" install emscripten-tag-%s' % (emsdkExePath, ver), shell=True, cwd=emsdkDir);
                subprocess.check_call( '"%s" activate emscripten-tag-%s' % (emsdkExePath, ver), shell=True, cwd=emsdkDir);
                subprocess.check_call( '"%s" install clang-tag-e%s' % (emsdkExePath, ver), shell=True, cwd=emsdkDir);
                subprocess.check_call( '"%s" activate clang-tag-e%s' % (emsdkExePath, ver), shell=True, cwd=emsdkDir);

                subprocess.check_call( '"%s" install node-4.1.1-64bit' % emsdkExePath, shell=True, cwd=emsdkDir);
                subprocess.check_call( '"%s" activate node-4.1.1-64bit' % emsdkExePath, shell=True, cwd=emsdkDir);

                #subprocess.check_call( '"%s" install spidermonkey-37.0.1-64bit' % emsdkExePath, shell=True, cwd=emsdkDir);
                #subprocess.check_call( '"%s" activate spidermonkey-37.0.1-64bit' % emsdkExePath, shell=True, cwd=emsdkDir);

                #subprocess.check_call( '"%s" install crunch-1.04' % emsdkExePath, shell=True, cwd=emsdkDir);
                #subprocess.check_call( '"%s" activate crunch-1.04' % emsdkExePath, shell=True, cwd=emsdkDir);


            except:

                for i in range(30):

                    try:
                        shutil.rmtree(emsdkDir);
                        break;

                    except:                                
                        time.sleep(1);

                raise;

            print("Emscripten was successfully set up.", file=sys.stderr);

        if sys.platform=="win32":
            envSetupPrefix = '"%s" activate latest && ' % emsdkExePath;
        else:

            envSetupPrefix = "source "+os.path.join(emsdkDir, "emsdk_env.sh") + " && ";

            if sys.platform=="linux2":
                # if we just call subprocess with "shell=True" then python
                # will use /bin/sh as the shell. And on ubuntu what you get
                # then is an intentionally restricted shell that does not
                # support any of the bash commands (even though it is most
                # likely bash). emsdk_env.sh requires bash, so we have to
                # explicitly execute bash to get this.
                commandIsInQuote = True;
                envSetupPrefix = '/bin/bash -c "' + envSetupPrefix.replace('"', '\\"');

            


        # the emscripten scrips call python2. However, python is not available
        # under that name on all platforms. So we add an alias
        try:
            subprocess.check_call("python2 --version", shell="True");
            havePython2 = True;
        except Exception:
            havePython2 = False;

        if not havePython2:
            print("Python2 executable is named just 'python'. Changing references...", file=sys.stderr);
            
            # change python2 references to python
            changePython2ToPython(emsdkDir);


        toolChainFileName = "Emscripten.cmake";

    elif platform=="dotnet":
        args.extend( ['-DBODEN_PLATFORM=dotnet' ] );

        if arch!="std":
            raise InvalidArchitectureError(arch);


    elif platform=="linux":
        # we prefer clang over GCC 4. GCC has a lot more bugs in their standard library.
        gccVer = getGCCVersion();
        clangVer = getClangVersion();

        print("Detected GCC version %s" % (repr(gccVer)) , file=sys.stderr);
        print("Detected Clang version %s" % (repr(clangVer)) , file=sys.stderr);

        if gccVer is not None and gccVer[0]==4 and clangVer is not None:
            print("Forcing use of clang instead of GCC because of bugs in this GCC version.", file=sys.stderr);
            envSetupPrefix = "env CC=/usr/bin/clang CXX=/usr/bin/clang++ ";



    args = ["-G", generatorName, getCMakeDir() ] + args[:];


    if toolChainFileName:
        toolChainFilePath = os.path.join(getCMakeDir(), toolChainFileName);               

    if toolChainFilePath:
        if not os.path.isfile(toolChainFilePath):
            print("Required CMake toolchain file not found: "+toolChainFilePath, file=sys.stderr);
            return 5;

        args.extend( ["-DCMAKE_TOOLCHAIN_FILE="+toolChainFilePath] );


    if config:
        args.extend( ["-DCMAKE_BUILD_TYPE="+config ] );

    if cmakeArch:
        args.extend( ["-A "+cmakeArch ] );

    # we do not validate the toolset name
    commandLine = "cmake";
    #commandLine = "cmake --debug-output";
    for a in args:
        commandLine += ' "%s"' % (a);

    if commandIsInQuote:
        commandLine = envSetupPrefix + commandLine.replace('"', '\\"').replace("&&", "\\&\\&") + '"'
    else:
        commandLine = envSetupPrefix+commandLine;

    if not os.path.isdir(cmakeBuildDir):
        os.makedirs(cmakeBuildDir);


    print("## Calling CMake:\n  "+commandLine+"\n", file=sys.stderr);

    exitCode = subprocess.call(commandLine, cwd=cmakeBuildDir, shell=True);
    if exitCode!=0:
        #subprocess.call("cat /boden/build/linux/Debug/CMakeFiles/CMakeOutput.log", shell=True);
        raise ToolFailedError("cmake", exitCode);


    if platform=="dotnet":
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

                    print("Modifying %s (adding required empty CallingConvention entry)..." % name, file=sys.stderr);
                    data = data.replace("</ClCompile>", "  <CallingConvention />\n    </ClCompile>");                            

                    modified = True;

                if "<CLRSupport>" not in data and "<UseOfMfc>" in data:
                    print("Modifying %s (adding CLRSupport entry)..." % name, file=sys.stderr);
                    data = data.replace("<UseOfMfc>", "<CLRSupport>Pure</CLRSupport>\n    <UseOfMfc>");                            

                    modifed = True;

                if modified:
                    with open( filePath, "wb") as f:
                        f.write(data.encode("utf-8"));

    elif platform=="winuwp":
        
        # At the time of this writing, an UWP app built with cmake may generate a "cannot open file" exception
        # when it is started in a debugger.
        # 0x00000004: The system cannot open the file (parameters: 0x80073B1F, 0x00000005).
        # 0x80073B1F means "ResourceMap Not Found"
        # This is caused by a bug in cmake that is described here:
        # https://gitlab.kitware.com/cmake/cmake/issues/16106
        # The problem is an incorrect path in the project file. We fix this manually.

        for name in os.listdir(cmakeBuildDir):
            if name.endswith(".vcxproj"):
                filePath = os.path.join(cmakeBuildDir, name);

                with open( filePath, "rb") as f:
                    data = f.read().decode("utf-8");

                title = os.path.splitext( name )[0];

                modified = False;
                if "<ProjectPriFullPath>$(TargetDir)resources.pri</ProjectPriFullPath>" in data:
                    print("Modifying %s (fixing ProjectPriFullPath entry)..." % name, file=sys.stderr);
                    data = data.replace("<ProjectPriFullPath>$(TargetDir)resources.pri</ProjectPriFullPath>", "<ProjectPriFullPath>%s.dir\\resources.pri</ProjectPriFullPath>" % title);                            
                    modified = True;

                if modified:
                    with open( filePath, "wb") as f:
                        f.write(data.encode("utf-8"));






def getFullToolsetName(toolsetName):
    return generatorInfo.generatorAliasMap.get(toolsetName, toolsetName);    



def getPreparedPlatformsAndArchs():
    buildDir = getBaseBuildDir();

    preparedList = [];

    if os.path.isdir(buildDir):
        for name in os.listdir(buildDir):
            platform, arch = splitBuildDirName(name);
            if platform and platform in platformMap:
                preparedList.append( (platform, arch) );

    return preparedList;


def getPlatformAndArchListForCommand(args):

    preparedPlatformsAndArchs = getPreparedPlatformsAndArchs();

    platformList = [];
    if args.platform:
        platformList.append( args.platform );

    else:
        for preparedPlatform, preparedArch in preparedPlatformsAndArchs:
            platformList.append( preparedPlatform );

    platformAndArchList = [];

    for platform in platformList:
        if args.arch:
            platformAndArchList.append( (platform, args.arch) );

        else:

            for preparedPlatform, preparedArch in preparedPlatformsAndArchs:
                if preparedPlatform==platform:
                    platformAndArchList.append( (platform, preparedArch) );

    return platformAndArchList;



def commandBuildOrClean(command, args):


    for platformName, arch in getPlatformAndArchListForCommand(args):

        if platformName not in platformMap:
            raise InvalidPlatformNameError(platformName);

        platformBuildDir = getPlatformBuildDir(platformName, arch);

        platformState = loadState(platformBuildDir);
        singleConfigBuildSystem = platformState.get("singleConfigBuildSystem", False);

        must_expand_configs = singleConfigBuildSystem or platformName=="android"

        if must_expand_configs and not args.config:
            configList = ["Debug", "Release"];
        else:
            configList = [args.config];

        if command=="clean" and platformName=="android":
            # clean always deletes everything
            configList = [None]

        for config in configList:

            if singleConfigBuildSystem:
                buildDirForConfig = os.path.join(platformBuildDir, config);
            else:
                buildDirForConfig = platformBuildDir;

            if platformName=="android":
                # we need to use gradle for our commandline builds

                gradle_wrapper_path = os.path.join(buildDirForConfig, "gradlew")
                if sys.platform=="win32":
                    gradle_wrapper_path+=".bat"

                if command=="clean":
                    gradle_command = "clean"
                
                else:
                    if config=="Release":
                        gradle_command = "assembleRelease"
                    else:
                        gradle_command = "assembleDebug"

                commandline = '"%s" %s' % (gradle_wrapper_path, gradle_command)
                

                exitCode = subprocess.call(commandline, shell=True, cwd=buildDirForConfig);
                if exitCode!=0:
                    raise ToolFailedError("gradlew "+gradle_command, exitCode);


            else:
                # use cmake                

                commandLine = "cmake --build "+buildDirForConfig;

                if command=="clean":
                    commandLine += " --platform clean";

                if config:
                    commandLine += " --config "+config;

                if config:
                    print("Calling cmake --build for config %s" % config, file=sys.stderr);
                else:
                    print("Calling cmake --build for all configs", file=sys.stderr);

                print(commandLine, file=sys.stderr);

                exitCode = subprocess.call(commandLine, shell=True, cwd=buildDirForConfig);
                if exitCode!=0:
                    raise ToolFailedError("cmake --build", exitCode);



def commandBuild(args):
    commandBuildOrClean("build", args);


def commandClean(args):
    commandBuildOrClean("clean", args);



def commandRun(args):

    if not args.module:
        raise ProgramArgumentError("Please specify a module name with --module MODULE")

    if not args.platform:
        raise ProgramArgumentError("Please specify the platform name with --platform PLATFORM")

    if not args.config:
        raise ProgramArgumentError("Please specify the configuration name with --config CONFIG")

    platformAndArchList = getPlatformAndArchListForCommand(args)
    if len(platformAndArchList)==0:
        raise PreparedStateError("No platform-arch pairs found for run command.")


    for platformName, arch in getPlatformAndArchListForCommand(args):

        if platformName not in platformMap:
            raise InvalidPlatformNameError(platformName);

        platformBuildDir = getPlatformBuildDir(platformName, arch);

        platformState = loadState(platformBuildDir);
        singleConfigBuildSystem = platformState.get("singleConfigBuildSystem", False);

        commandIsInQuote = False;

        stdOutFileHandled = False

        if not args.config:
            configList = ["Debug", "Release"];
        else:
            configList = [args.config];

        for config in configList:

            outputDir = os.path.join(platformBuildDir, config);
            if platformName=="ios":
                outputDir += "-iphonesimulator";                

            elif platformName=="android":
                outputDir = platformBuildDir;

            moduleFilePath = os.path.join(outputDir, args.module);


            commandLine = None;

            if platformName=="win32":
                moduleFilePath += ".exe";

            elif platformName=="mac":

                if os.path.exists(moduleFilePath+".app"):
                    moduleFilePath += ".app";
                    commandLine = "open -W "+moduleFilePath;

            elif platformName=="ios":

                if args.stdout_file:
                    raise Exception("Redirecting stdout to a file is currently not implemented for ios.")

                if os.path.exists(moduleFilePath+".app"):
                    moduleFilePath += ".app";
                    commandLine = "open -a Simulator -W --args -SimulateApplication "+moduleFilePath+"/"+args.module;

            elif platformName=="android":

                if args.stdout_file:
                    raise Exception("Redirecting stdout to a file is currently not implemented for android.")

                android_api_version = "26"

                # the android ARM emulator is REALLY slow - almost unusable. So we use x86 by default
                android_abi = "x86"


                moduleFilePath = os.path.join(moduleFilePath, "build", "outputs", "apk", config.lower(), args.module+"-"+config.lower()+".apk")

                if not os.path.exists(moduleFilePath):
                    raise Exception("APK not found - expected here: "+moduleFilePath)

                android_home_dir = os.environ.get("ANDROID_HOME")
                if not android_home_dir:
                    raise Exception("ANDROID_HOME environment variable is not set. Please point it to the root of the android SDK installation.")

                avd_manager_path = os.path.join(android_home_dir, "tools", "bin", "avdmanager")
                if not os.path.exists(avd_manager_path):
                    raise Exception("avdmanager not found. Expected here: "+avd_manager_path)

                emulator_path = os.path.join(android_home_dir, "emulator", "emulator")
                if not os.path.exists(emulator_path):
                    raise Exception("Android emulator executable not found. Expected here: "+emulator_path)

                adb_path = os.path.join(android_home_dir, "platform-tools", "adb")
                if not os.path.exists(adb_path):
                    raise Exception("Android ADB executable not found. Expected here: "+adb_path)

                

                # create the virtual device that we want to use for the emulator.
                # --force causes an existing device to be overwritten.
                print("-- Creating virtual device for emulator...", file=sys.stderr)
                create_device_command = '"%s" create avd --name bdnTestAVD --force --abi google_apis/%s --package "system-images;android-%s;google_apis;%s"' % \
                    (   avd_manager_path,
                        android_abi,
                        android_api_version,
                        android_abi )

                # avdmanager will ask us wether we want to create a custom profile. We do not want that,
                # so we pipe a "no" into stdin
                answer_file, answer_file_path = tempfile.mkstemp()
                try:
                    os.write(answer_file, "no\n".encode("ascii"))
                    os.close(answer_file)

                    with open(answer_file_path, "r") as answer_file:
                        subprocess.check_call(create_device_command, shell=True, stdin=answer_file)

                finally:
                    os.remove(answer_file_path)


                print("-- Starting emulator...", file=sys.stderr);

                gpu_option = "auto"

                # For some reason, GPU acceleration does not work inside a Parallels VM for linux.
                # "auto" will cause the emulator to exit with an error.
                if sys.platform=="linux2":
                    output = subprocess.check_output("lspci | grep VGA", shell=True);
                    if output.find("Parallels")!=-1:
                        print("-- WARNING: Disabling GPU acceleration because we are running in a Parallels Desktop Linux VM.", file=sys.stderr);
                        gpu_option = "off"


                # Note: the -logcat parameter can be used to cause the android log
                # to be written to the process stdout. For example, "-logcat *:e" prints
                # all messages of any log source that have the "error" log level.
                # However, the logging is very spammy - lots of things that are only
                # informational messages get logged as errors. So it is recommended
                # to only use this parameter for specific log sources (also called "tags")
                # For example "-logcat myapp:w" would enable all messages with log level warning
                # or higher from the log source "myapp".
                start_emulator_command = '"%s" -avd bdnTestAVD -gpu %s' % \
                    (   emulator_path,
                        gpu_option )

                # the emulator process will not exit. So we just open it without
                # waiting.
                emulator_process = subprocess.Popen(start_emulator_command, shell=True )

                success = False

                try:

                    # wait for the emulator  to finish booting (at most 60 seconds)
                    print("-- Waiting for android emulator to finish booting...", file=sys.stderr);
                    timeout_seconds = 120
                    timeout_time = time.time()+timeout_seconds
                    while True:

                        boot_anim_state_command = '"%s" shell getprop init.svc.bootanim' % adb_path

                        # at first the command will fail, until the emulator process has initialized.
                        # Then we will get a proper output, that will not be "stopped" while we boot.
                        # Then, after the boot has completed, we will get "stopped"

                        try:
                            output = subprocess.check_output(boot_anim_state_command, stderr=subprocess.STDOUT, shell=True)                            
                            state_error = False
                        except subprocess.CalledProcessError, e:                            
                            output = e.output
                            state_error = True

                        output = str(output)
                        output = output.strip()


                        if output=="" and not state_error:
                            # this happens between the "initializing" states and
                            # the boot starting-
                            emulator_state = "transitioning"

                        elif output=="running":
                            emulator_state = "booting"

                        elif output=="stopped":
                            emulator_state = "boot finished"

                        elif output.find("no devices/emulators found")!=-1:
                            emulator_state = "initializing (no devices yet)"

                        elif output.find("device offline")!=-1:
                            emulator_state = "initializing (device not yet online)"

                        else:
                            raise Exception("Unrecognized boot animation state output: "+output)

                        print("-- Emulator state: "+emulator_state, file=sys.stderr)
                        
                        if emulator_state=="boot finished":
                            # done booting
                            break

                        # all other states indicate that either the emulator is still initializing
                        # or that the boot process has not finished yet.

                        if time.time()>=timeout_time:
                            raise Exception("Android emulator did not finish booting within %d seconds. Last state output:\n%s" % (timeout_seconds, output) ) 

                        time.sleep(5)


                    print("-- Waiting 10 seconds...", file=sys.stderr)
                    time.sleep(10)


                    print("-- Installing app in emulator...", file=sys.stderr)

                    # now install the app in the emulator
                    install_app_command = '"%s" install -t "%s"' % \
                        (   adb_path,
                            moduleFilePath )
                    subprocess.check_call(install_app_command, shell=True)

                    print("-- Waiting 10 seconds...", file=sys.stderr)
                    time.sleep(10)


                    print("-- Starting app in emulator...", file=sys.stderr)

                    # and run the executable in the emulator

                    run_app_command = '"%s" shell am start -a android.intent.action.MAIN -n io.boden.android.%s/io.boden.android.NativeRootActivity' % \
                        (   adb_path,
                            args.module )
                    subprocess.check_call(run_app_command, shell=True)

                    success = True

                    print("-- App successfully started.", file=sys.stderr)
                    print("-- Waiting for emulator to terminate (will not happen automatically).", file=sys.stderr)

                    # we want to block indefinitely until either the emulator is closed or we are terminated
                    # by the user.
                    emulator_process.wait()

                finally:

                    if not success:
                        print("-- Killing emulator", file=sys.stderr)
                        emulator_process.kill()


                # do not do the normal processing
                return

            elif platformName=="webems":
                moduleFilePath += ".html";

                mainDir = getMainDir();
                emsdkDir = getEmscriptenSdkBaseDir()

                emsdkExePath = os.path.join(emsdkDir, "emsdk");

                browserOption = ""; # use default browser by default

                if sys.platform=="win32":
                    commandLine = '"%s" activate latest && ' % emsdkExePath;
                else:
                    commandLine = "source "+os.path.join(emsdkDir, "emsdk_env.sh") + " && ";

                    if sys.platform=="linux2":
                        # if we just call subprocess with "shell=True" then python
                        # will use /bin/sh as the shell. And on ubuntu what you get
                        # then is an intentionally restricted shell that does not
                        # support any of the bash commands (even though it is most
                        # likely bash). emsdk_env.sh requires bash, so we have to
                        # explicitly execute bash to get this.
                        commandIsInQuote = True;
                        commandLine = '/bin/bash -c "' + commandLine.replace('"', '\\"');

                    if sys.platform=="darwin":
                        browserOption = "--browser firefox";


                # find a free port number
                import socket;                
                sock = socket.socket();
                try:
                    sock.bind(('', 0))
                    portNum = sock.getsockname()[1];
                finally:
                    sock.close();

                stdoutOption = ""
                if args.stdout_file:
                    # emrun will append to this file if it exists. So we have to make sure
                    # that it does not exist first
                    if os.path.exists(args.stdout_file):
                        os.remove(args.stdout_file)
                        
                    stdoutOption = '--log_stdout "%s"' % os.path.abspath(args.stdout_file)
                    stdOutFileHandled = True

                # note that we pass the --no_emrun_detect flag. This disables the warning
                # message that the files were not built with --emrun. The warning is generated
                # by a timed check after 10 seconds, which verifies that the JS code has
                # "checked in". However, since we have huge JS files, it may well be that the
                # browser takes longer to download and initialize them. So we disable the warning.

                actualCommand = "emrun --kill_start --kill_exit --no_emrun_detect --port %d %s %s %s" % (portNum, browserOption, stdoutOption, moduleFilePath);

                if commandIsInQuote:
                    commandLine += actualCommand.replace('"', '\\"');
                else:
                    commandLine += actualCommand;
                
            if commandLine is None:
                commandLine = moduleFilePath;

            for p in args.params:                
                argPart = ' "%s"' % (p);
                if commandIsInQuote:                    
                    commandLine += argPart.replace('"', '\\"');
                else:
                    commandLine += argPart;

            print("Calling executable of module %s:" % args.module, file=sys.stderr);

            if commandIsInQuote:
                commandLine += '"'

            print(commandLine, file=sys.stderr);

            stdout_file = None
            try:
                call_kwargs = { "shell": True,
                                "cwd": outputDir
                                }

                if args.stdout_file and not stdOutFileHandled:
                    stdout_file = os.open(args.stdout_file, os.O_WRONLY|os.O_TRUNC|os.O_CREAT)
                    call_kwargs["stdout"] = stdout_file

                exitCode = subprocess.call(commandLine, **call_kwargs);

            finally:
                if stdout_file is not None:
                    os.close(stdout_file)

            if exitCode!=0:
                raise ToolFailedError("run", exitCode);




def commandDistClean(args):
    for platformName, arch in getPlatformAndArchListForCommand(args):
        if platformName not in platformMap:
            raise InvalidPlatformNameError(platformName);

        platformBuildDir = getPlatformBuildDir(platformName, arch);
        if os.path.isdir(platformBuildDir):
            shutil.rmtree(platformBuildDir);


def commandBuildDeps(args):

    # build curl
    srcDir = os.path.join( getMainDir(), "3rdparty", "curl");
    buildDir = os.path.join( getMainDir(), "3rdparty_build", "curl");

    if not os.path.isdir(buildDir):
        try:
            os.makedirs( os.path.dirname(buildDir) );

            shutil.copytree( srcDir, buildDir);

        except:

            if os.path.isdir(buildDir):
                deleteDirTree(buildDir);

            raise;

    if sys.platform=="win32":
        winBuildDir = os.path.join(buildDir, "winbuild");
        subprocess.check_call("nmake /f Makefile.vc mode=dll", cwd=winBuildDir, shell=True);

    else:
        subprocess.check_call("./configure", cwd=buildDir, shell=True);
        subprocess.check_call("./make", cwd=buildDir, shell=True);



def getUsage():
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

build.py --module MODULE [--platform TARGET] [--config CONFIG] [--arch ARCH] run -- PARAMS

Runs the specified executable.

MODULE is the name of the executable to run.

PARAMS are the optional commandline parameters to pass to the executable. The parameter list
should be preceded by the separator argument "--" and a space.

If TARGET is omitted then all prepared platforms are run (one after the other).

If CONFIG is omitted then all configurations are run (one after the other).

If ARCH is omitted then all prepared architectures for the platform are run (one after the other).



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
  AndroidStudio (for android platform only)

%s

RESTRICTIONS:
  ios platform: only the Xcode build system is supported
  webems platform: only the Makefile build systems are supported
  android platform: only the AndroidStudio build system is supported

IMPORTANT: Remember to enclose the build system names that consist of multiple
words in quotation marks!


  """ % ( getPlatformHelp(), "\n".join(generatorInfo.generatorHelpList), generatorInfo.generatorAliasHelp );



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
        print(getUsage(), file=sys.stderr);



def main():

    print("", file=sys.stderr);

    try:
        global generatorInfo;
        generatorInfo = GeneratorInfo();

        
        argParser = MyArgParser();
        argParser.add_argument("command", choices=["prepare", "build", "clean", "distclean", "builddeps", "run"] );

        argParser.add_argument("--platform" );
        argParser.add_argument("--build-system" );
        argParser.add_argument("--config", choices=["Debug", "Release"] );
        argParser.add_argument("--arch" );

        argParser.add_argument("--module" );

        argParser.add_argument("--stdout-file" );

        argParser.add_argument("params", nargs="*" );

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

        elif command=="builddeps":
            commandBuildDeps(args);

        elif command=="run":
            commandRun(args);

        else:
            raise ProgramArgumentError("Invalid command: '%s'" % command);

    except HelpOptionUsed:
        return 0;

    except ProgramArgumentError as e:
        print( "\n"+str(e)+"\n" , file=sys.stderr);
        print('Call with --help to get help information.', file=sys.stderr);

        return e.exitCode;

    except ErrorWithExitCode as e:
        print( "\n"+str(e)+"\n" , file=sys.stderr);
        return e.exitCode;


    except Exception as e:        
        traceback.print_exc();
        return 50;



exitCode = main();
if exitCode!=0:
    sys.exit(exitCode);





