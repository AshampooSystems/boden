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

    rootPath = os.path.abspath(os.path.join(os.path.realpath(__file__), "..", ".."))

    buildFolder = BuildFolder(bauerGlobals, generatorInfo, rootPath, args)

    commandProcessor = CommandProcessor(bauerGlobals, generatorInfo, args, rootPath, buildFolder)
    commandProcessor.process()

def main(argv):
    try:
        return run(argv)
    except HelpOptionUsed as e:
        exit(0)
    except cmakelib.CMakeError as e:
        print("Error:", e.cmakeError)
        print("Packet:", e.packet)
        print("****************************")
        traceback.print_exc()
        exit(1)

    except error.ErrorWithExitCode as e:
        print( "\n"+str(e)+"\n" , file=sys.stderr);
        print()
        traceback.print_exc();
        exit(e.exitCode);

    except Exception as e:        
        traceback.print_exc();
        exit(50)