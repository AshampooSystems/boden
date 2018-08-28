import sys

class BauerGlobals:
    def __init__(self):
        if sys.platform == "win32":
            self.platformList = [ 
                ("winuwp", "Universal Windows app (Windows 10 and later)" ),
                ("win32", "Classic Windows desktop program (despite the 32 in the name, this also includes 64 bit Windows desktop programs)") ]
        
        if sys.platform == "linux2":
            self.platformList = [ ("linux", "Linux" ) ]

        if sys.platform == "darwin":
            self.platformList = [ 
                ("mac", "Apple Mac OS (formerly OSX)" ),
                ("ios", "iPhone, iPad" ) ]

        self.platformList += [ ("android", "Android devices" ),
               ("webems", """\
Compiles the C++ code to a Javascript-based web app or Javascript
    library that can be run either in a web browser or on a Node.js system.
    The resulting JS code is pure Javascript. No native components or
    plugins are needed to execute it.""") ];

        self.platformMap = {};
        for platformName, platformInfo in self.platformList:
            self.platformMap[platformName] = platformInfo;