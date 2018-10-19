import subprocess, tempfile, os
import error

from distutils.spawn import find_executable

class GeneratorInfo(object):
    def __init__(self):
        self.cmakeExecutable = find_executable('cmake')

        if not self.cmakeExecutable or not os.path.exists(self.cmakeExecutable):
            raise Exception("Couldn't find cmake executable. Please download and install it from www.cmake.org/download")

        self.generatorHelpList = [];
        self.generatorNames = [];    
        self.generatorAliasMap = {};

        self._cmakeError = None

        errOutput = ""
        try:    
            errOutFile, errOutFilePath = tempfile.mkstemp()            
            try:
                cmakeHelp = subprocess.check_output("\"" + self.cmakeExecutable + "\" --help", shell=True, universal_newlines=True, stderr=errOutFile );
            finally:
                os.close(errOutFile)
                try:
                    with open(errOutFilePath, "r") as f:
                        errOutput = f.read().strip();
                except:
                    pass
                os.remove(errOutFilePath)          

        except subprocess.CalledProcessError as e:            
            self._cmakeError = error.CMakeProblemError(e, errOutput);
        
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

    def getFolderNameForBuildSystem(self, buildSystem):
        return folderName.replace('_', ' ')

    def getBuildSystemForFolderName(self, folderName):
        return folderName.replace(' ', '_')

    def isSingleConfigBuildSystem(self, buildSystem):

        if buildSystem=="AndroidStudio":
            return False;

        generatorName = self.getCMakeGeneratorName(buildSystem);    

        generatorName = generatorName.lower();

        if "makefile" in generatorName or "ninja" in generatorName:
            return True;
        else:
            return False;

    def getCMakeGeneratorName(self,generatorName):
        return self.generatorAliasMap.get(generatorName, generatorName)
