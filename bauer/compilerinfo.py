import logging
import subprocess
import os
import sys

class CompilerInfo:
    def __init__(self):
        self.logger = logging.getLogger(__name__)
        self.gccVersion = self.getGCCVersion()
        self.clangVersion = self.getClangVersion()

        self.logger.info("Detected GCC version %s" % (repr(self.gccVersion)));
        self.logger.info("Detected Clang version %s" % (repr(self.clangVersion)));

    def getGCCVersion(self):
        try:
            out = subprocess.check_output("gcc --version", shell=True);
            return self.parseVersionOutput(out, 3, preferLast=True );
        except:
            return None;


    def getClangVersion(self):
        try:
            out = subprocess.check_output("clang --version", shell=True);
            return self.parseVersionOutput(out, 2, preferLast=False );
        except:
            return None;

    def parseVersionOutput(self, out, expectedVersionComponents, preferLast):
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