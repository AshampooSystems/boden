import sys, os, subprocess

class BauerGlobals:
    def __init__(self):
        self.platformList = []
        
        if sys.platform == "darwin":
            self.platformList = [ 
                ("mac", "Apple Mac OS (formerly OSX)" ),
                ("ios", "iPhone, iPad" ) ]

        self.platformList += [ ("android", "Android devices" ) ]

        self.platformMap = {}
        for platformName, platformInfo in self.platformList:
            self.platformMap[platformName] = platformInfo;

    def open_file(self, filename):
        if sys.platform == "win32":
            os.startfile(filename)
        else:
            opener ="open" if sys.platform == "darwin" else "xdg-open"
            subprocess.call([opener, filename])
