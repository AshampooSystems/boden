#!/usr/bin/python

import sys;
import subprocess;
import os.path;
import shutil;

def main():

	print "";

	try:
		cmakeHelp = subprocess.check_output("cmake --help", shell=True);

	except subprocess.CalledProcessError, e:
		print "There was a problem calling cmake. Cmake is required and must be installed to run this."
		return 3;

	cmakeHelp = cmakeHelp.strip();


    # the generator list is at the end of the output. And the entries are all indented.
	generatorHelpList = [];
	for line in reversed( cmakeHelp.splitlines() ):
		if not line or not line.startswith(" "):
			break;

		generatorHelpList.append(line);

	generatorHelpList.reverse();

	targetInfoList = [ 	("windows", "", [""]),
						("osx", 	"", [""]),
						("ios", 	"", ["", "sim64", "sim32"]),
						("android", "", [""]),
						("web", 	"", [""])
					];

	targetHelp = "";
	targetMap = {};
	for target, info, subTargetList in targetInfoList:
		targetHelp+="  "+target;
		if info:
			targetHelp+="  [%s]" % info;
		targetHelp+="\n";

		targetMap[target] = subTargetList;


	if len(sys.argv)!=3:

		print """Usage: generateProject.py TARGETNAME TOOLSET

TARGETNAME can be one of the following:

%s
TOOLSET is the build toolset / IDE you want to use. This is passed to CMAKE
as the generator name and can be one of the following values on your system:

%s

  RESTRICTIONS:
    ios target: only the Xcode toolset is supported
    web target: only the Makefile toolsets are supported
    android target: only the Makefile toolsets are supported

  IMPORTANT: Remember to enclose the toolset names that consist of multiple
  words in quotation marks!

  """ % ( targetHelp, "\n".join(generatorHelpList) );
		return 1;

	targetName = sys.argv[1].lower();
	toolsetName = sys.argv[2];

	if targetName not in targetMap:
		print "Invalid target name."
		return 1;

	myPath = os.path.abspath(__file__);
	mainDir = os.path.dirname(myPath);
	buildDir = os.path.join(mainDir, "build");
	cmakeDir = os.path.join(mainDir, "cmake");

	if not os.path.isdir(buildDir):
		os.makedirs(buildDir);

	fixedConfigList = [];

	if " - " not in toolsetName and ("makefile" in toolsetName.lower() or "ninja" in toolsetName.lower()):
		# per-configuration generator. I.e. the build type (debug/release) is fixed.
		fixedConfigList = ["Debug", "Release"];
	
	else:
		# multi-configuration generator. No need to specify build type at config time.
		fixedConfigList = [""];


	subTargetList = targetMap[targetName];

	for fixedConfig in fixedConfigList:

		for subTarget in subTargetList:

			toolsetBuildDir = os.path.join(buildDir, targetName+"-"+toolsetName.lower().replace(" ", ""));
			if subTarget:
				toolsetBuildDir+="-"+subTarget;
			if fixedConfig:
				toolsetBuildDir+="-"+fixedConfig.lower();			

			if not os.path.isdir(toolsetBuildDir):
				os.makedirs(toolsetBuildDir);

			args = ["-G", toolsetName, cmakeDir];

			if targetName=="ios":

				if subTarget=="":
					platform = "OS";

				elif subTarget=="sim32":
					platform = "SIMULATOR";

				elif subTarget=="sim64":
					platform = "SIMULATOR64";

				else:
					assert("bad subtarget");

				args.extend( [	"-DIOS_PLATFORM="+platform,
								"-DCMAKE_TOOLCHAIN_FILE="+os.path.join(cmakeDir, "iOS.cmake")								
								] );

			elif targetName=="web":
				args.extend( ["-DCMAKE_TOOLCHAIN_FILE="+os.path.join(cmakeDir, "Emscripten.cmake")] );

			elif targetName=="android":
				args.extend( ["-DCMAKE_TOOLCHAIN_FILE="+os.path.join(cmakeDir, "android.cmake")] );

			if fixedConfig:
				args.extend( ["-DCMAKE_BUILD_TYPE="+fixedConfig ] );

			# we do not validate the toolset name
			commandLine = "cmake";
			for a in args:
				commandLine += ' "%s"' % (a);

			print "## Calling CMake:\n  "+commandLine+"\n";

			exitCode = subprocess.call(commandLine, cwd=toolsetBuildDir, shell=True);
			if exitCode!=0:
				print "\ncmake failed with exit code %d\n" % (exitCode);
				return 2;

			print "";


exitCode = main();
if exitCode!=0:
	sys.exit(exitCode);



