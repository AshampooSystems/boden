import os
import sys
import error
import shutil
import pprint
import logging

class TemplateCreator:
    def __init__(self):
        self.logger = logging.getLogger(__name__)

    def process_template(self, path, vars):
        template = open(path,'r').read()
        #for arg in dir(args):
        for k,v in vars.items():
            template = template.replace('{%s}' % k, v)

        open(path, 'w').write(template)


    def generate(self, args):
        template_folder = os.path.normpath(os.path.join(os.path.realpath(__file__), '..', 'templates', 'app'))
        boden_folder = os.path.normpath(os.path.join(os.path.realpath(__file__), '..', '..'))
        project_folder = os.path.join(os.path.abspath(args.project_folder), args.project_name)

        if os.path.exists(project_folder) and len(os.listdir(project_folder)) > 0:
            raise error.ProgramArgumentError("Selected folder %s is not empty!" % project_folder)

        if args.project_name == 'test':
            raise error.ProgramArgumentError("'test' is not a valid project name, since cmake regards it as a special target!")

        if "-" in args.project_name:
            raise error.ProgramArgumentError("You cannot use '-' in the project name, as Android can not handle it")

        boden_relative_path = os.path.relpath(boden_folder, project_folder)

        vars = {
            'project_name' : args.project_name,
            'project_folder' : project_folder,
            'boden_relative_path' : boden_relative_path
        }

        self.logger.info("Creating project in %s", project_folder)

        shutil.copytree(template_folder, project_folder)

        for entries in os.walk(project_folder):
            for file in entries[2]:
                self.process_template(os.path.join(entries[0],file),vars)