import logging
import json
import os
import sys
import subprocess

import error

class DesktopRunner:
    def __init__(self, cmake, configuration, args):
        self.logger = logging.getLogger(__name__)

        self.cmake = cmake
        self.configuration = configuration
        self.args = args

    def run(self):
        cmakeTargetToRun = self.cmake.executableTarget(self.args.config, self.args.target)
        artifactToRun = self.cmake.executableArtifactPath(cmakeTargetToRun)

        self.logger.debug("Executable: %s", artifactToRun)

        if not artifactToRun:
            raise error.ProgramArgumentError("Couldn't find path to exectuable for Module %s" % self.args.target)

        self.logger.debug("Path: %s", artifactToRun)

        if sys.platform == "win32":
            return self.runWindowsApplication(artifactToRun)
        else:
            return self.runUnixApplication(artifactToRun)

    def runStandardApplication(self, arguments, runOutputFileHandled = False):
        stdout_file = None
        exitCode = 0

        try:
            call_kwargs = { "shell": True }

            if self.args.run_output_file and not runOutputFileHandled:
                stdout_file = os.open(self.args.run_output_file, os.O_WRONLY|os.O_TRUNC|os.O_CREAT)
                call_kwargs["stdout"] = stdout_file

            arguments += self.args.params

            commandLine = ' '.join('"{0}"'.format(arg) for arg in arguments)

            self.logger.debug("Executing: %s", commandLine)

            exitCode = subprocess.call(commandLine, **call_kwargs);

        finally:
            if stdout_file is not None:
                os.close(stdout_file)

        return exitCode

    def runUnixApplication(self, path):
        return self.runStandardApplication([path])

    def runWindowsApplication(self, path):
        return self.runStandardApplication([path])





