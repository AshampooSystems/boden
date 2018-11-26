from __future__ import print_function

import sys, json, os, shutil, cmakelib
import argparse
import traceback
import error
import logging

from bauerargparser import BauerArgParser
from bauerargparser import HelpOptionUsed
from commandprocessor import CommandProcessor
from buildfolder import BuildFolder
from bauerutilities import BauerGlobals
from generatorinfo import GeneratorInfo
from coloredlogger import ColorizingStreamHandler
from templatecreator import TemplateCreator


def setupLogging(argv):
    root = logging.getLogger()
    colorizer = ColorizingStreamHandler()
    root.addHandler(colorizer)

    if '--enable-debug-output' in argv or '-d' in argv:
        colorizer.setFormatter(logging.Formatter("[%(asctime)s](%(name)s): %(message)s"))
        root.setLevel(logging.DEBUG)
    else:
        colorizer.setFormatter(logging.Formatter("[%(asctime)s] %(message)s"))
        root.setLevel(logging.INFO)

def run(argv):
    bauerGlobals = BauerGlobals()

    setupLogging(argv)

    generatorInfo = GeneratorInfo();

    argParser = BauerArgParser(bauerGlobals, generatorInfo);
    argParser.buildBauerArguments(argv)

    args = argParser.parse_args()

    if args == None:
        return

    if args.command == 'new':
        templateCreator = TemplateCreator()
        templateCreator.generate(args)
        return

    rootPath = os.path.abspath(os.path.join(os.path.realpath(__file__), "..", ".."))
    source_folder = os.getcwd()

    buildFolder = BuildFolder(bauerGlobals, generatorInfo, source_folder, args)

    commandProcessor = CommandProcessor(bauerGlobals, generatorInfo, args, rootPath, source_folder, buildFolder)
    commandProcessor.process()

def main(argv):
    try:
        return run(argv)
    except HelpOptionUsed as e:
        exit(0)
    except cmakelib.CMakeError as e:

        if '-d' in argv:
            print("CMake error: %s" % (e.cmakeError), file=sys.stderr)
            print(" packet:", e.packet, file=sys.stderr)
            traceback.print_exc();
        else:
            print(" ".join(str(v) for v in e.arg), file=sys.stderr)
        exit(1)

    except error.ErrorWithExitCode as e:
        if '-d' in argv:
            traceback.print_exc();
        else:
            print(" ".join(str(v) for v in e.arg), file=sys.stderr)

        exit(e.exitCode);

    except Exception as e:        
        if '-d' in argv:
            traceback.print_exc();
        else:
            print(" ".join(str(v) for v in e.args), file=sys.stderr)
        exit(50)
