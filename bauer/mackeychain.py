import subprocess 
import re
import logging
import os, sys
import argparse
from distutils.spawn import find_executable

import error
import bauer
from bauerargparser import BauerArgParser
from bauerargparser import EnvDefault

class MacKeychain:
    def __init__(self):
        self.logger = logging.getLogger(__name__)
        self.sec = find_executable('security')
        self.logger.debug('security path: %s', self.sec)

    def callAndGet(self, cmd, useError):
        self.logger.debug('Calling %s' % cmd)
        proc = None
        if useError:
            proc = subprocess.Popen(cmd, stderr=subprocess.PIPE)
        else:
            proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)

        if useError:
            output = proc.stderr.read()
        else:
            output = proc.stdout.read()

        self.logger.debug('Output:\n%s' % output)
        return output

    def callAndGetOutput(self, cmd):
        return self.callAndGet(cmd, False)

    def callAndGetError(self, cmd):
        return self.callAndGet(cmd, True)

    def listKeychains(self):
        currentList = self.getCurrentKeychains()
        self.logger.info("Current list:\n%s" % ( "\n".join(currentList) ))

    def addKeychain(self, keychainPath):
        self.logger.debug('Add: %s', keychainPath)

        keychainPath = self.makeAbs(keychainPath)

        currentList = self.getCurrentKeychains()
        currentList += [keychainPath]

        result = self.setCurrentKeychains(currentList)

    def removeKeychain(self, keychainPath):
        self.logger.debug('Remove: %s', keychainPath)
        keychainPath = self.makeAbs(keychainPath)

        currentList = self.getCurrentKeychains()
        removedList = filter(lambda k: k != keychainPath, currentList)
        self.logger.debug('new: %s', removedList)

        result = self.setCurrentKeychains(removedList)

    def unlockKeychain(self, keychainPath, password):
        self.logger.debug('Unlock: %s *****', keychainPath )
        currentKeychains = self.getCurrentKeychains()
        if not keychainPath in currentKeychains:
             raise error.ProgramArgumentError('Keychain %s is not registered!' % keychainPath)

        result = self.callAndGetError([self.sec, 'unlock-keychain', '-p', password, keychainPath])
        if len(result) > 0:
            self.logger.critical(result)

    def makeAbs(self, path):
        if not os.path.isabs(path):
            path = os.path.abspath(path)
        if not os.path.exists(path):
            raise FileNotFoundError(errno.ENOENT, os.strerror(errno.ENOENT), path)
        return path

    def getCurrentKeychains(self):
        keychains = self.callAndGetOutput([self.sec, 'list-keychains', '-d', 'user'])
        newKeychainList = []
        for keychain in keychains.splitlines():
            cleaned = keychain.strip().strip('"')
            newKeychainList += [ cleaned ]

        return newKeychainList

    def setCurrentKeychains(self, keychains):
        return self.callAndGetOutput([self.sec, 'list-keychains', '-d', 'user', '-s'] + keychains)


def main():
    bauer.setupLogging(sys.argv)

    argParser = BauerArgParser(None, None)
    parser = argparse.ArgumentParser()
    argParser.setBaseParser(parser)

    cmdParsers = {}

    subs = parser.add_subparsers(title="Command", help='The command to execute', dest='command')
    cmdParsers['add'] = subs.add_parser('add', description="Adds a keychain to the current users search list")
    cmdParsers['remove'] = subs.add_parser('remove', description="Removes a keychain from the current users search list")
    cmdParsers['unlock'] = subs.add_parser('unlock', description="Unlocks a keychain")
    listParser = subs.add_parser('list', description="List registered keychains")

    argParser.buildGlobalArguments(cmdParsers.values())
    argParser.addKeychainArgument(cmdParsers.values(), required=True)
    argParser.addPasswordArgument([cmdParsers['unlock']], required=True)

    args = parser.parse_args()

    macKeychain = MacKeychain()

    if args.command == 'add':
        macKeychain.addKeychain(args.keychain)
    elif args.command == 'remove':
        macKeychain.removeKeychain(args.keychain)
    elif args.command == 'unlock':
        macKeychain.unlockKeychain(args.keychain, args.password)
    elif args.command == 'list':
        macKeychain.listKeychains()

if __name__ == "__main__":
    main()
