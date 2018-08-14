import logging
import sys
import os
import time
import shutil
import subprocess

class EmscriptenInfo:
    def __init__(self, sdkSourceDir, localSdkBuildPath):
        self.baseVersion = "1.38.0"
        self.version = self.baseVersion + "-64bit"
        self.logger = logging.getLogger(__name__)
        self.sdkSourceDir = sdkSourceDir

        emsdkDir = os.environ.get("EMSDK_BASE_DIR")
        if not emsdkDir or not os.path.exists(emsdkDir):
            # use local emsdk installation
            emsdkDir = localSdkBuildPath;

        self.sdkBuildDir = emsdkDir

    def getEmscriptenSdkBuildDir(self):
        return self.sdkBuildDir

    def getEmscriptenSdkRootPath(self):
        return os.path.join(self.sdkBuildDir, "emscripten", "tag-" + self.baseVersion)

    def ensure_emscripten_version_active(self):
        self.logger.info("Checking active emscripten version...")

        self.ensure_emscripten_component_active( "emscripten-tag-" + self.version )
        self.ensure_emscripten_component_active( "clang-tag-e" + self.version )
        self.ensure_emscripten_component_active( "node-4.1.1-64bit" )

        self.logger.info("Emscripten %s is active.", self.version)

        self.changePython()

    def changePython(self):
        # the emscripten scripts call python2. However, python is not available
        # under that name on all platforms. So we add an alias
        havePython2 = False
        try:
            subprocess.check_output("python2 --version", shell="True", stderr=subprocess.STDOUT);
            havePython2 = True;
        except Exception:
            pass

        if not havePython2:
            self.logger.warning("Python2 executable is named just 'python'. Changing references...");
            # change python2 references to python
            self.changePython2ToPython(self.getEmscriptenSdkBuildDir());

    def changePython2ToPython(self, dirPath):
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
                        self.changePython2ToPython(itemPath);

                else:
                    title, extension = os.path.splitext(name);
                    if not extension or extension==".py":

                        # could be a python file.
                        with file(itemPath, "r+") as f:
                            data = f.read(len(prefix));
                            if data==prefix:
                                f.seek(0);
                                f.write(replacementPrefix);

                                self.logger.info("Updated %s" % itemPath);

                            f.close();

    def ensure_emscripten_component_active(self, comp_name):
        emsdkDir = self.getEmscriptenSdkBuildDir()

        emsdkExePath = os.path.join(emsdkDir, "emsdk");

        if not os.path.isdir(emsdkDir):
            self.logger.info("Setting up Emscripten SDK. This can take a while...");

            try:
                self.logger.info("Copy %s to %s", self.sdkSourceDir, emsdkDir);
                shutil.copytree(self.sdkSourceDir, emsdkDir);
                subprocess.check_call( '"%s" update' % emsdkExePath, shell=True, cwd=emsdkDir);

            except:
                self.logger.debug("Something went wrong ...")

                for i in range(30):

                    try:
                        shutil.rmtree(emsdkDir);
                        break;

                    except:                                
                        time.sleep(1);

                raise;


        comp_ok = False
        try:
            result = subprocess.check_output( '"%s" activate %s' % (emsdkExePath, comp_name), shell=True, cwd=emsdkDir, stderr=subprocess.STDOUT);

            # unfortunately activate seems to return exit code 0 even if the component is not installed.
            # So we have to detect failure and success based on the output.

            if "not installed" in result:
                comp_ok = False
            else:
                comp_ok = True

        except:
            comp_ok = False

        if not comp_ok:
            self.logger.info("Emscripten component %s is apparently not installed yet. Installing...", comp_name);
            subprocess.check_call( '"%s" update' % (emsdkExePath), shell=True, cwd=emsdkDir);
            subprocess.check_call( '"%s" install %s' % (emsdkExePath, comp_name), shell=True, cwd=emsdkDir);
            subprocess.check_call( '"%s" activate %s' % (emsdkExePath, comp_name), shell=True, cwd=emsdkDir);


