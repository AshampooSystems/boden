import socket
import logging
import sys
import os

from desktoprunner import DesktopRunner

class EmscriptenRunner:
    def __init__(self, buildFolder, cmake, emscriptenInfo):
        self.logger = logging.getLogger(__name__)
        self.buildFolder = buildFolder
        self.cmake = cmake
        self.emscriptenInfo = emscriptenInfo


    def run(self, configuration, args):
        cmakeTargetToRun = self.cmake.executableTarget(args.config, args.module)
        artifactToRun = self.cmake.executableArtifactPath(cmakeTargetToRun)

        if not artifactToRun:
            raise error.ProgramArgumentError("Couldn't find path to exectuable for Module %s" % self.args.module)

        self.logger.debug("Path: %s", artifactToRun)

         # find a free port number
        import socket;                
        sock = socket.socket();
        try:
            sock.bind(('', 0))
            portNum = sock.getsockname()[1];
        finally:
            sock.close();

        stdoutArgs = []

        stdoutOptions = []
        if args.run_output_file:
            # emrun will append to this file if it exists. So we have to make sure
            # that it does not exist first
            if os.path.exists(args.run_output_file):
                os.remove(args.run_output_file)
                
            stdoutOptions = ['--log_stdout', "%s" % os.path.abspath(args.run_output_file) ]
            runOutputFileHandled = True

        browserOptions = []

        #if sys.platform=="darwin":
        #    browserOptions += ["--browser firefox"];

        # note that we pass the --no_emrun_detect flag. This disables the warning
        # message that the files were not built with --emrun. The warning is generated
        # by a timed check after 10 seconds, which verifies that the JS code has
        # "checked in". However, since we have huge JS files, it may well be that the
        # browser takes longer to download and initialize them. So we disable the warning.

        # note that we do NOT pass the --kill_start flag. That can cause weird issues with
        # firefox on Linux. Firefox starts normally, but after a few minutes apparently the communication
        # pipe for stdour/stderr between the website and emrun breaks down.
        # At that point no further output can be transferred and the firefox tab also often crashes.
        # This happens only when we pass the kill flag.
        # It seems that some effect of the kill flag lingers in the background and influences
        # the newly started browser.
        # So, to avoid all that we do NOT kill the browser. The caller is responsible for ensuring
        # that the firefox launch works as expected.

        emrunPath = os.path.join(self.emscriptenInfo.getEmscriptenSdkRootPath(), "emrun")

        arguments = [emrunPath, "--no_emrun_detect", "--port", "%s" % (portNum)] + browserOptions + stdoutOptions + [artifactToRun]

        if args.params:
            arguments += args.params


        runner = DesktopRunner(self.cmake, configuration, args)
        return runner.runStandardApplication(arguments)