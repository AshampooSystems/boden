import os
import json

class GeneratorState:
    def __init__(self, directory):
        self.directory = directory
        self.state = {};

    def __enter__(self):
        self.loadState()
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        if not exc_type:
            self.storeState()


    def getStatePath(self):
        return os.path.join(self.directory, ".generateProjects.state");
    
    
    def loadState(self):
        p = self.getStatePath();
        if os.path.exists(p):
            with open(p, "rb") as f:
                try:
                    self.state = json.loads( f.read().decode("utf-8") );
                except:
                    pass
   
    def storeState(self):
        p = self.getStatePath();
        if os.path.exists(self.directory):
            with open(p, "wb") as f:
                f.write( json.dumps( self.state ).encode("utf-8") );
