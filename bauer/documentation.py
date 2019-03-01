import os
import sys
import error
import shutil
import pprint
import logging
import subprocess

class Documentation:
    def __init__(self):
        self.logger = logging.getLogger(__name__)
        self.documentation_directory = os.path.normpath(os.path.join(os.path.realpath(__file__), '..', '..', 'docs'))
        self.logger.debug("Documentation folder: %s", self.documentation_directory)

    def install_requirements(self):
        self.logger.info("Installing requirements ...")
        subprocess.call([sys.executable, "-m", "pip", "install", "-r", os.path.join(self.documentation_directory, 'requirements.txt')])

    def run(self):
        self.install_requirements()
        self.logger.info("Starting server ...")

        subprocess.call(["mkdocs", "serve", "-f", os.path.join(self.documentation_directory, 'mkdocs.yml')])

        return
