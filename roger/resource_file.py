import json
import os
import logging
from pprint import pprint, pformat

class ResourceFile:
    def __init__(self):
        pass

    def absolute_root_path(self, args, data):
        return os.path.abspath(os.path.join(os.path.dirname(args.input_file), data['root-path']))

    def parse_input(self, args):
        fp = open(args.input_file, 'r')
        j = fp.read()

        self.data = json.loads(j)

        self.root_path = self.absolute_root_path(args, self.data)

        logging.debug("Root path: %s" %(self.root_path))
        logging.debug("Input data: \n%s"% pformat(self.data))

