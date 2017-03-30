# /usr/bin/python3

import subprocess
import os.path
import shutil

print("\nRunning doxygen...\n")

baseDir = os.path.dirname( os.path.abspath(__file__) )

xmlDir = os.path.join(baseDir, "doc", "xml")
if os.path.isdir(xmlDir):
	shutil.rmtree(xmlDir);

subprocess.check_call("doxygen", cwd = baseDir);

print("\nRunning doxymag...\n")


additionalPyPath = os.path.join(baseDir, "..", "doxymag", "src")

if not os.path.exists(additionalPyPath):
	raise Exception("Doxymag package base path not found. Expected here: "+additionalPyPath)

inPath = os.path.join(xmlDir, "index.xml")
outDir = os.path.join(baseDir, "doc", "doxymag" )

commandLine = 'python3 -m doxymag.doxymag -i "%s" -o "%s"' % (inPath, outDir)

print(commandLine);

if os.path.isdir(outDir):
	shutil.rmtree(outDir)
os.makedirs(outDir)

e = {}
e.update(os.environ)

pyPath =  e.get("PYTHONPATH", "")
if pyPath:
	pyPath+=";"
pyPath += additionalPyPath;
e["PYTHONPATH"] = pyPath


subprocess.check_call(commandLine, shell=True, env=e )