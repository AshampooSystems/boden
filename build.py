import sys;
import os.path;

if len(sys.argv)!=3:
	print("""Usage: build.py TARGET CONFIG

CONFIG can be either Debug or Release.

  """
  sys.exit(1);


targetName = sys.argv[1];
config = sys.argv[2];

if config not in ("Debug", "Release"):
	print("\nInvalid CONFIG parameter. Must be Debug or Release.\n")
	sys.exit(1);
	

myDir = os.path.dirname(__file__);

targetBuildDir = os.path.join(myDir, "build", targetName);

if not os.isdir(targetBuildDir):
	print("\nNo project files found for target '%s'\n" % targetName);
	sys.exit(2);



configDir = os.path.join(myDir, config);

returnCode = subprocess.call("make", cwd=myDir)
if returnCode!=0:
	sys.exit(100+returnCode);



