import os
import tempfile
import shutil
import subprocess
import logging


class AndroidStudioProjectGenerator(object):
    def __init__(self, gradle, cmake, platformBuildDir, androidBuildApiVersion):
        self.logger = logging.getLogger(__name__)
        self._projectDir = platformBuildDir;
        self.gradle = gradle
        self.cmake = cmake
        self.androidBuildApiVersion = androidBuildApiVersion

    def getGradleDependency(self):
        return "classpath 'com.android.tools.build:gradle:3.2.0'"

    def generateTopLevelProject(self, moduleNameList):
        if not os.path.isdir(self._projectDir):
            os.makedirs(self._projectDir);

        self.dependencies = self.cmake.cache["BAUER_ANDROID_DEPENDENCIES"].split(';')

        self.logger.debug("Dependencies: %s" % self.dependencies)

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

            gradle_path = self.gradle.getGradlePath()
        
            subprocess.check_call(
                #'"%s" wrapper --gradle-distribution-url "https://services.gradle.org/distributions/gradle-4.10-all.zip"' % (gradle_path),
                '"%s" wrapper --gradle-version=4.10.2' % (gradle_path),
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




    def generateModule(self, packageId, cmakeTargetName, targetSourceDirectory, userFriendlyTargetName, dependencyList, isLibrary, android_abi, rootCMakeFile):        

        moduleDir = os.path.join(self._projectDir, cmakeTargetName);
        if not os.path.isdir(moduleDir):
            os.makedirs(moduleDir);

        with open( os.path.join(moduleDir, "build.gradle"), "w" ) as f:
            code = self.getModuleBuildGradleCode(
                packageId = packageId, 
                cmakeTargetName = cmakeTargetName, 
                targetSourceDirectory = targetSourceDirectory, 
                dependencyList = dependencyList, 
                isLibrary = isLibrary, 
                android_abi = android_abi,
                rootCMakeFile = rootCMakeFile)

            f.write(code)
  
        inBuildSourceDir = os.path.join(moduleDir, "src", "main");
        if not os.path.isdir(inBuildSourceDir):
            os.makedirs(inBuildSourceDir);

        with open( os.path.join(inBuildSourceDir, "AndroidManifest.xml"), "w" ) as f:
            f.write( self.getModuleAndroidManifest(packageId, cmakeTargetName, isLibrary ) )

        resDir = os.path.join(inBuildSourceDir, "res");
        if not os.path.isdir(resDir):
            os.makedirs(resDir);

        valuesDir = os.path.join(resDir, "values");
        if not os.path.isdir(valuesDir):
            os.makedirs(valuesDir);

        with open( os.path.join(valuesDir, "strings.xml"), "w" ) as f:
            f.write("""\
<?xml version="1.0" encoding="utf-8"?>
<resources>
    <string name="app_name">$$UserFriendlyTargetName$$</string>
</resources>
""".replace("$$UserFriendlyTargetName$$", userFriendlyTargetName) )



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

    <uses-permission android:name="android.permission.INTERNET" />

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


    def getModuleBuildGradleCode(self, packageId, cmakeTargetName, targetSourceDirectory, dependencyList, isLibrary, android_abi, rootCMakeFile):

        if isLibrary:
            pluginName = "com.android.library";
            appIdCode = ""; # libraries do not have an application id
        else:
            pluginName = "com.android.application";
            appIdCode = "applicationId = '%s'" % packageId

        moduleDependencyCode = "";
        for dep in dependencyList:            
            moduleDependencyCode += "    implementation project(':%s')\n" % dep;

        cmakeTargets = '"%s"' % (cmakeTargetName);

        if android_abi:
            abiFilterStatement = "abiFilters '%s'" % android_abi
        else:
            abiFilterStatement = ""

        cmakeVersion = self.cmake.globalSettings["capabilities"]["version"]["string"]

        dependencies = ""
        for dep in self.dependencies:
            dependencies += "    implementation '%s'\n" % (dep)
        
        return """
apply plugin: '$$PluginName$$'

android {
    compileSdkVersion $$BuildSdkVersion$$
    defaultConfig {
        $$AppIdCode$$
        minSdkVersion 23
        targetSdkVersion $$BuildSdkVersion$$
        versionCode 1
        versionName "1.0"
        externalNativeBuild {
            cmake {
                targets $$CmakeTargets$$
                arguments "-DANDROID_STL=c++_static", "-DANDROID_CPP_FEATURES=rtti exceptions"
                $$AbiFilter$$
                cppFlags "-std=c++17 -frtti -fexceptions"                     
            }
        }
    }
    buildTypes {
        defaultConfig {
            minifyEnabled false
            proguardFiles getDefaultProguardFile('proguard-android.txt'), 'proguard-rules.pro'
        }
    }
    externalNativeBuild {
        cmake {            
            path "$$RootCMakeFile$$"
            version "$$CmakeVersion$$"
        }
    }


    sourceSets {
        main {
            java {
                srcDir '$$TargetSourceDirectory$$/java'
            }
            jni {
                srcDirs = ['$$TargetSourceDirectory$$/src',
                           '$$TargetSourceDirectory$$/include' ]
            }
        }
    }
}

dependencies {
    implementation fileTree(dir: 'libs', include: ['*.jar'])

$$AndroidDependencies$$

$$ModuleDependencyCode$$
}


""" .replace("$$AppIdCode$$", appIdCode) \
    .replace("$$BuildSdkVersion$$", self.androidBuildApiVersion) \
    .replace("$$AbiFilter$$", abiFilterStatement) \
    .replace("$$PluginName$$", pluginName) \
    .replace("$$CmakeTargets$$", cmakeTargets) \
    .replace("$$ModuleDependencyCode$$", moduleDependencyCode) \
    .replace("$$TargetSourceDirectory$$", targetSourceDirectory) \
    .replace("$$RootCMakeFile$$", rootCMakeFile) \
    .replace("$$CmakeVersion$$", cmakeVersion) \
    .replace("$$AndroidDependencies$$", dependencies)


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

