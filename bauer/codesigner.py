import logging
import os
import subprocess

from distutils.spawn import find_executable
from mackeychain import MacKeychain

class CodeSigner:
    def __init__(self, codeModel):
        self.logger = logging.getLogger(__name__)
        self.codeModel = codeModel
        self.codeSignUtil = find_executable('codesign')

        if not os.path.exists(self.codeSignUtil):
            raise Exception("Couldn't find codesign executable, aborting.")

    def sign(self, args):
        macKeychain = MacKeychain()

        if args.keychain:
            macKeychain.addKeychain(args.keychain)
            if args.password:
                macKeychain.unlockKeychain(args.keychain, args.password)

        try:
            for configuration in self.codeModel["configurations"]:
                for project in configuration["projects"]:
                    for target in project["targets"]:
                        if target["type"] in ["SHARED_LIBRARY", "EXECUTABLE"]:
                            for artifact in target["artifacts"]:
                                self.logger.info("Signing: %s ...", artifact)
                                arguments = [self.codeSignUtil, '--force', '-s', " ".join(args.identity), artifact]

                                subprocess.check_call(arguments)
        finally:
            if args.keychain:
                macKeychain.removeKeychain(args.keychain)
