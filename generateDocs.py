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


doxymagPath = os.path.join(baseDir, "..", "doxymag", "src", "doxymag", "doxymag.py")

if not os.path.exists(doxymagPath):
	raise Exception("Doxymag not found. Expected here: "+doxymagPath);

inPath = os.path.join(xmlDir, "index.xml")
outDir = os.path.join(baseDir, "doc", "doxymag" )

commandLine = 'python3 "%s" -i "%s" -o "%s"' % (doxymagPath, inPath, outDir);

print(commandLine);

if os.path.isdir(outDir):
	shutil.rmtree(outDir);
os.makedirs(outDir);

subprocess.check_call(commandLine, shell=True );