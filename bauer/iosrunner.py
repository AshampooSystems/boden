import logging
import os
import sys
import subprocess
import random
import time

import error
from iosinfo import IOSInfo

class IOSRunner:
    def __init__(self, buildFolder, cmake):
        self.logger = logging.getLogger(__name__)

        self.buildFolder = buildFolder
        self.cmake = cmake

        self.iosInfo = IOSInfo()
        self.ios_simulator_device_type = None
        self.ios_simulator_os          = None

    def run(self, configuration, args):
        self.ios_simulator_device_type = self.iosInfo.getSelectedDeviceType(args)
        self.ios_simulator_os = self.iosInfo.getSelectedOS(args)

        self.logger.debug("IOS Device type:  %s", self.ios_simulator_device_type)
        self.logger.debug("IOS Simulator OS: %s", self.ios_simulator_os)

        cmakeTargetToRun = self.cmake.executableTarget(args.config, args.module)
        artifactToRun = self.cmake.executableArtifactPath(cmakeTargetToRun)
        artifactToRun = artifactToRun.replace("${EFFECTIVE_PLATFORM_NAME}", "-iphonesimulator")


        self.logger.debug("Executable: %s", artifactToRun)

        if not artifactToRun:
            raise error.ProgramArgumentError("Couldn't find path to exectuable for Module %s" % args.module)

        if not os.path.exists(artifactToRun):
            raise error.ProgramArgumentError("exectuable for Module %s does not exists at: %s" % (args.module, artifactToRun))
    
        bundlePath = self.getBundlePathFromExecutable(artifactToRun)

        bundleId = self.getBundleIdentifier(bundlePath)
        self.logger.debug("Bundle Identifier: %s", bundleId)
        
        simulatorId = None
        try:
            simulatorId = self.createSimulatorDevice()

            self.logger.debug("Simulator Id: %s", simulatorId)

            self.bootSimulator(simulatorId)
            self.installApp(simulatorId, bundlePath)
            processId = self.startApp(simulatorId, bundleId, args)
            self.waitForAppToExit(simulatorId, processId, bundleId)
        finally:
            if simulatorId:
                self.shutdownSimulator(simulatorId)

        return 0
     
    def createSimulatorDevice(self):
        simulatorName = "bdnTestSim-" + str(random.getrandbits(32))

        self.logger.debug("Simulator name: %s", simulatorName)

        arguments = ["xcrun", "simctl", "create", simulatorName, 
            self.ios_simulator_device_type, 
            self.ios_simulator_os]

        simulatorId = subprocess.check_output(" ".join(arguments), shell=True).strip().decode(encoding='utf-8')

        if not simulatorId or " " in simulatorId or "\n" in simulatorId:
            raise Exception("Invalid simulator device ID returned.")

        return simulatorId

    def bootSimulator(self, simulatorId):
        self.logger.info("Booting simulator ...")
        subprocess.check_call("open -a Simulator", shell=True)

        # note that this will fail if the simulator is already booted or is currently booting up.
        # That is ok.
        subprocess.call("xcrun simctl boot " + simulatorId, shell=True)

        self.waitForSimulatorStatus(simulatorId, "booted", 600)

    def installApp(self, simulatorId, bundlePath):
        self.logger.info("Installing Application in simulator ...")
        subprocess.check_output('xcrun simctl install "%s" "%s"' % (simulatorId, bundlePath), shell=True)

    def startApp(self, simulatorId, bundleId, args):
        self.logger.info("Starting Application ...")

        # --console connects the app's stdout and stderr to ours and blocks indefinitely
        stdoutOptions = []
        if args.run_output_file:
            abs_stdout_path = os.path.abspath(args.run_output_file);
            if os.path.exists(abs_stdout_path):
                os.remove(abs_stdout_path)
            
            self.logger.debug("Redirecting Applications stdout to: %s", abs_stdout_path)

            stdoutOptions = [ '"--stdout=%s"' % abs_stdout_path ]

        arguments = ["xcrun",  "simctl", "launch" ] + stdoutOptions + [simulatorId, bundleId] + args.params

        commandLine = ' '.join('"{0}"'.format(arg) for arg in arguments)

        resultLine = subprocess.check_output(commandLine, shell=True).decode(encoding='utf-8').strip()

        before, sep, processId = resultLine.rpartition(":")                    
        if not sep:
            raise Exception("Got an unexpected response from launch command: "+result_line)
        processId = processId.strip()

        self.logger.debug("Process id inside simulator is: %s", processId);

        return processId

    def waitForAppToExit(self, simulatorId, processId, bundleId):
        self.logger.info("Waiting for simulated process %s to exit ...", processId)

        while True:
            processListOutput = subprocess.check_output('xcrun simctl spawn "%s" launchctl list' % simulatorId, shell=True).decode(encoding='utf-8')

            foundProcess = False
            for line in processListOutput.splitlines():

                words = line.split()

                if words[0]==processId and bundleId in str(line):
                    foundProcess = True
                    break

            if not foundProcess:
                self.logger.info("Process inside simulator has exited.")
                break

            time.sleep(2)

        # note that we could also poll the process list and wait for the app's process
        # to terminate. However, that does not happen automatically, unless the app crashes
        # or is terminated because of user interaction. So right now there is no good use for that.
        # Note that we could get the process info with:
        # xcrun simctl spawn SIM_ID launchctl list
        # we would get output like this: 11538  0   UIKitApplication:BUNDLE_ID[0x8cd3][PROCESS_ID]
        # If the process is gone then the entry is not in the list.

        #predicate = "'processID == %s'" % process_id
        #predicate = "'processImagePath contains %s'" % bundle_id
        #log_stream_commandline = "xcrun simctl spawn '%s' log stream --level=debug --predicate %s" % (sim_id, predicate)
        #log_stream_process = subprocess.Popen(log_stream_commandline, shell=True)

        #log_stream_process.wait()

    def shutdownSimulator(self, simulatorId):
        self.logger.info("Shutting down simulator");
        subprocess.call('xcrun simctl shutdown "%s"' % simulatorId, shell=True );
        # note that shutdown automatically waits until the simulator has finished shutting down

        self.logger.info("Deleting simulator device.");
        subprocess.call('xcrun simctl delete "%s"' % simulatorId, shell=True)

    def waitForSimulatorStatus(self, simulatorId, wait_for_status, timeout_seconds):
        timeout_time = time.time()+timeout_seconds
        while True:

            status = self.getSimulatorStatus(simulatorId)
            if not status:
                raise Exception("Unable to get simulator status.")

            self.logger.debug("Simulator status: %s", status);

            if status==wait_for_status:
                break

            if time.time() > timeout_time:
                raise Exception("Simulator has not reached desired status in %d seconds - timing out." % timeout_seconds)

            time.sleep(1)

    def getSimulatorStatus(self, simulatorId):
        output = subprocess.check_output("xcrun simctl list", shell=True).decode(encoding='utf-8')

        search_for = "("+simulatorId+")"

        for line in output.splitlines():
            if search_for in line:
                before, sep, status = line.rpartition("(")
                if sep:
                    status, sep, after = status.partition(")")
                    if sep and status:
                        return status.lower()

        return None

    def getBundlePathFromExecutable(self, executablePath):
        bundlePath = os.path.abspath(os.path.join( executablePath, ".."))
        return bundlePath

    def getBundleIdentifier(self, bundlePath):
        plistPath = os.path.abspath(os.path.join( bundlePath, "Info.plist"))

        bundleId = subprocess.check_output('defaults read "%s" CFBundleIdentifier' % (plistPath), shell=True).decode(encoding='utf-8').strip();
        if not bundleId:
            raise Exception("Unable to extract bundle id from app bundle.")

        return bundleId