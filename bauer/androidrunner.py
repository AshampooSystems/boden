import os
import sys
import subprocess
import logging
import time
import random
import tempfile

from androidstudioprojectgenerator import find_defines, from_defines

class AndroidRunner:
    def __init__(self, buildFolder, buildExecutor):
        self.logger = logging.getLogger(__name__)
        self.buildFolder = buildFolder
        self.buildExecutor = buildExecutor

        self.subprocess_out = None
        if not self.logger.isEnabledFor(logging.DEBUG):
            fnull = open(os.devnull, 'w')
            self.subprocess_out = fnull

        self.androidHome = self.buildExecutor.getAndroidHome()
        self.androidEnvironment = self.buildExecutor.getToolEnv()
        self.avdManagerPath = self.buildExecutor.getBuildToolPath(self.androidHome, "tools/bin/avdmanager")
        self.adbPath = self.buildExecutor.getBuildToolPath(self.androidHome, "platform-tools/adb")
        self.sdkManagerPath = self.buildExecutor.getBuildToolPath(self.androidHome, "tools/bin/sdkmanager")
        self.emulatorPath = self.buildExecutor.getBuildToolPath(self.androidHome, "emulator/emulator")

    def find_target(self, targetName, configuration):
        for config in self.buildExecutor.cmake.codeModel["configurations"]:
            for project in config["projects"]:
                for target in project["targets"]:
                    if target["name"] == targetName:
                        return target

        return None;

    def run(self, configuration, args):

        if args.run_output_file and not args.run_android_fetch_output_from:
            raise Exception("For android, --run-output-file can only be used with --run-android-fetch-output-from.")

        target = self.find_target(args.target, configuration)
        if not target:
            raise "Could not find target %s in cmake codemodel" % (args.target)

        target_defines = find_defines(target)
        packageId = from_defines(target_defines, "ANDROID_APP_ID", None)

        if not packageId:
            raise "Could not find package id for target %s (should be a compile definition called ANDROID_APP_ID)" % (args.target) 

        self.logger.debug("Package Id: %s" %(packageId))

        androidAbi = self.buildExecutor.getAndroidABI(configuration)
        appIdToRun = packageId
        buildDir = self.buildFolder.getBuildDir(configuration)

        moduleNameInFileSystem = args.target
        moduleFilePath = os.path.join(buildDir, moduleNameInFileSystem, "build", "outputs", "apk", args.config.lower(), moduleNameInFileSystem+"-"+args.config.lower()+".apk")

        if not os.path.exists(moduleFilePath):
            raise Exception("APK not found - expected here: "+moduleFilePath)
        
        deviceName = "bdnTestAVD"+str(random.getrandbits(32))

        self.prepareAndroid(androidAbi)
        deviceName = self.createEmulatorDevice(androidAbi, deviceName)

        emulatorProcess = None

        try:
            emulatorProcess = self.bootEmulator(deviceName, androidAbi)

            self.installAppInEmulator(moduleFilePath)
            self.startAppInEmulator(appIdToRun, args)
            self.waitForAppToFinish(appIdToRun)

            self.fetchOutput(args, appIdToRun)

        finally:
            if emulatorProcess != None:
                self.closeEmulator(deviceName, emulatorProcess)

            self.logger.info("Deleting virtual device for emulator...")
            deleteDeviceCommand = '"%s" delete avd --name %s' % (self.avdManagerPath, deviceName)

            subprocess.call(deleteDeviceCommand, stdout=self.subprocess_out, shell=True, env=self.androidEnvironment )

        return 0


    def getEmulatorAbi(self, projectAbi):
        return "x86" if projectAbi is None else projectAbi


    def prepareAndroid(self, androidAbi):
        self.logger.info("Ensuring that all necessary android packages are installed (API Version: %s, ABI: %s)..." % (self.buildExecutor.androidEmulatorApiVersion, androidAbi) )

        emulatorAbi = self.getEmulatorAbi(androidAbi)

        sdkManagerCommand = '"%s" "emulator" "system-images;android-%s;google_apis;%s"' % (
            self.sdkManagerPath,
            self.buildExecutor.androidEmulatorApiVersion,
            emulatorAbi )

        subprocess.check_call( sdkManagerCommand, stdout=self.subprocess_out, shell=True, env=self.androidEnvironment )

        self.logger.info("Done updating packages.")

    def createEmulatorDevice(self, androidAbi, deviceName):

        # create the virtual device that we want to use for the emulator.
        # --force causes an existing device to be overwritten.
        self.logger.info("Creating virtual device %s for emulator...", deviceName)

        emulatorAbi = self.getEmulatorAbi(androidAbi)

        createDeviceCommand = '"%s" create avd --name "%s" --force --abi google_apis/%s --package "system-images;android-%s;google_apis;%s"' % \
            (   self.avdManagerPath,
                deviceName,
                emulatorAbi,
                self.buildExecutor.androidEmulatorApiVersion,
                emulatorAbi )

        # avdmanager will ask us wether we want to create a custom profile. We do not want that,
        # so we pipe a "no" into stdin
        answerFile, answerFilePath = tempfile.mkstemp()
        try:
            os.write(answerFile, "no\n".encode("ascii"))
            os.close(answerFile)

            with open(answerFilePath, "r") as answerFile:
                subprocess.check_call(createDeviceCommand, stdout=self.subprocess_out, shell=True, stdin=answerFile, env=self.androidEnvironment)

        finally:
            os.remove(answerFilePath)

        return deviceName

    def bootEmulator(self, deviceName, androidAbi):
        self.logger.info("Starting emulator...");

        gpuOption = "auto"

        # For some reason, GPU acceleration does not work inside a Parallels VM for linux.
        # "auto" will cause the emulator to exit with an error.
        if sys.platform.startswith("linux"):
            try:
                output = subprocess.check_output("lspci | grep VGA", shell=True, env=self.androidEnvironment, universal_newlines=True)
                if output.find("Parallels")!=-1:
                    self.logger.warning("Disabling GPU acceleration because we are running in a Parallels Desktop Linux VM.");
                    gpuOption = "off"
            except subprocess.CalledProcessError:
                # if the lspci utility is not there then we simply assume that we are not in a parallels VM and do nothing.
                pass

        # Note: the -logcat parameter can be used to cause the android log
        # to be written to the process stdout. For example, "-logcat *:e" prints
        # all messages of any log source that have the "error" log level.
        # However, the logging is very spammy - lots of things that are only
        # informational messages get logged as errors. So it is recommended
        # to only use this parameter for specific log sources (also called "tags")
        # For example "-logcat myapp:w" would enable all messages with log level warning
        # or higher from the log source "myapp".
        startEmulatorCommand = '"%s" -avd %s -gpu %s' % \
            (   self.emulatorPath,
                deviceName,
                gpuOption )

        # the emulator process will not exit. So we just open it without
        # waiting.
        if sys.platform == "win32":
            emulatorProcess = subprocess.Popen(startEmulatorCommand, stdout=self.subprocess_out, shell=False, env=self.androidEnvironment )
        else:
            emulatorProcess = subprocess.Popen(startEmulatorCommand, stdout=self.subprocess_out, shell=True, env=self.androidEnvironment )

        # wait for the emulator  to finish booting (at most 60 seconds)
        self.logger.info("Waiting for android emulator to finish booting...");
        # ARM emulators are REALLY slow. So we need a bigger timeout for them 
        timeoutSeconds = 600
        if androidAbi is None or androidAbi.startswith("x86"):
            timeoutSeconds = 120

        self.logger.debug("Emulator pid: %s", emulatorProcess.pid)

        timeoutTime = time.time()+timeoutSeconds
        while True:
            bootAnimStateCommand = '"%s" shell getprop init.svc.bootanim' % (self.adbPath)

            # at first the command will fail, until the emulator process has initialized.
            # Then we will get a proper output, that will not be "stopped" while we boot.
            # Then, after the boot has completed, we will get "stopped"

            try:
                output = subprocess.check_output(bootAnimStateCommand, stderr=subprocess.STDOUT, shell=True, env=self.androidEnvironment, universal_newlines=True )                          
                stateError = False
            except subprocess.CalledProcessError as e:                            
                output = e.output
                stateError = True

            output = output.strip()


            if output == "" and not stateError:
                # this happens between the "initializing" states and
                # the boot starting-
                emulatorState = "transitioning"

            elif output=="running":
                emulatorState = "booting"

            elif output=="stopped":
                emulatorState = "boot finished"

            elif output.find("no devices/emulators found")!=-1:
                emulatorState = "initializing (no devices yet)"

            elif output.find("device offline")!=-1:
                emulatorState = "initializing (device not yet online)"

            elif output.find("still connecting")!=-1:
                emulatorState = "initializing (device connecting)"

            else:
                raise Exception("Unrecognized boot animation state output: "+output)

            self.logger.debug("Emulator state: " + emulatorState)
            
            if emulatorState=="boot finished":
                # done booting
                break

            # all other states indicate that either the emulator is still initializing
            # or that the boot process has not finished yet.

            if time.time()>=timeoutTime:
                raise Exception("Android emulator did not finish booting within %d seconds. Last state output:\n%s" % (timeout_seconds, output) ) 

            time.sleep(5)

        self.logger.debug("Waiting 5 seconds...")
        time.sleep(5)

        return emulatorProcess

    def installAppInEmulator(self, moduleFilePath):
        self.logger.info("Installing app in emulator...")

        # now install the app in the emulator
        installAppCommand = '"%s" install -t "%s"' % \
            (   self.adbPath,
                moduleFilePath )
        subprocess.check_call(installAppCommand, stdout=self.subprocess_out, shell=True, env=self.androidEnvironment )

        self.logger.debug("Waiting 10 seconds...")
        time.sleep(10)


    def startAppInEmulator(self, appIdToRun, args):
        self.logger.info(" Starting app in emulator...")

        # and run the executable in the emulator
        appDataDirInEmulator = "/data/user/0/%s" % (appIdToRun)

        runAppCommand = '"%s" shell am start -a android.intent.action.MAIN -n %s/io.boden.android.NativeRootActivity' % ( self.adbPath, appIdToRun )

        # we pass the commandline parameters as "extra" to the android app.
        # These can be accessed inside the app via "activity.getIntent().getExtras()".
        # Inside the app the extras object is a bundle (i.e. a string->value map).
        # We add the arguments as a string array with the name "commandline-args"
        # Boden apps automatically look at the extras and try to find their commandline
        # arguments there.
        if len(args.params)>0:
            param_array_string = ""
            for param_index, param in enumerate(args.params):
                # we want commas to end up as \, for the called app.
                # For that we have to escape the backslash and the comma for the shell, so we need
                # a double backslash, followed by backslash comma (so three backslashes and a comma)
                # Because this is also a pythin string literal we need another backslash in fron of
                # each backslash
                param = param.replace(",", "\\\\\\,")
                param = param.replace("\"", "\\\"")
                param = param.replace("\'", "\\\'")

                param = param.replace("{DATA_DIR}", appDataDirInEmulator)

                if param_index>0:
                    param_array_string += "\\,"
                param_array_string += param

            runAppCommand += ' --esa commandline-args "%s"' % param_array_string

        self.logger.debug("runAppCommand: %s", runAppCommand )
        subprocess.check_call(runAppCommand, shell=True, env=self.androidEnvironment )

        self.logger.info("App successfully started.")
        # give the process a chance to appear in the process list inside the emulator
        time.sleep(2)


    def waitForAppToFinish(self, appIdToRun):
        self.logger.info("Waiting for app inside emulator to exit...")

        while True:
            getProcessListCommand = '"%s" shell ps' % ( self.adbPath )
            procListOutput = subprocess.check_output(getProcessListCommand, shell=True, env=self.androidEnvironment, universal_newlines=True )

            found_process = False
            for line in procListOutput.splitlines():
                line_words = line.split()

                if appIdToRun in str(line_words):
                    found_process = True
                    break
            
            if not found_process:
                break
            time.sleep(2)

        self.logger.info("Process inside emulator has exited.")

    def fetchOutput(self, args, appIdToRun):
        appDataDirInEmulator = "/data/user/0/%s" % (appIdToRun)

        # the application has now exited. If the caller wants us to read output data from
        # a file inside the emulator then we do that now.
        if args.run_android_fetch_output_from:
            fromPath = args.run_android_fetch_output_from
            fromPath = fromPath.replace("{DATA_DIR}", appDataDirInEmulator)

            if args.run_output_file:
                readTargetFile = open(args.run_output_file, "w")
                to_desc = args.run_output_file
            else:
                readTargetFile = None
                to_desc = "stdout"

            self.logger.info("Extracting output data from emulator\nFrom: %s\nTo: %s", fromPath, to_desc )

            try:
                # it would be nice if we could use adb pull. However we will get permission
                # denied when we try to access private data.
                # Luckily we can use run-as instead
                #pull_command = '"%s" pull "%s" "%s"' % ( self.adbPath, fromPath, temp_output_path )                                
                readCommand = '"%s" shell run-as "%s" cat "%s"' % ( self.adbPath, appIdToRun, fromPath )
                
                # if the file does not exist then the pull command will fail.
                readExitCode = subprocess.call(readCommand, shell=True, stdout=readTargetFile, env=self.androidEnvironment )

                if readExitCode!=0:
                    self.logger.warning("Output file inside emulator does not exist.")

            finally:
                if readTargetFile is not None:
                    readTargetFile.close()

    def closeEmulator(self, deviceName, emulatorProcess):
        self.logger.warning("Killing emulator")

        killEmulatorCommand = '"%s" -s "%s" emu kill' % ( self.adbPath, "emulator-5554" )
        subprocess.check_call(killEmulatorCommand, stdout=self.subprocess_out, shell=True, env=self.androidEnvironment )

        self.logger.debug("Waiting for emulator to exit...")

        # Make sure the emulator is gone.
        try:
            emulatorProcess.wait(30)
        except:
            emulatorProcess.kill()
