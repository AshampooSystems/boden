from __future__ import print_function
from distutils.spawn import find_executable

import sys
import cmakelib
import json
import os
import shutil
import pprint
import logging
import error

cmakelib.print_communication = False

class CMake:
    def __init__(self, cmakeExecutable = ""):
        self.logger = logging.getLogger(__name__)

        if cmakeExecutable == "":
            self.cmakeExecutable = find_executable('cmake')

            if not self.cmakeExecutable:
                raise Exception("Could not find cmake executable")
        else:
            self.cmakeExecutable = cmakeExecutable

        self.logger.debug("CMake found at: %s", self.cmakeExecutable)

        self.codeModel = {}

    def open(self, sourceDirectory, buildDirectory, generatorName, extraGeneratorName = "", extraEnv = {}):

        self.proc = cmakelib.initServerProc(self.cmakeExecutable, cmakelib.communicationMethods[0], extraEnv)
        if self.proc is None:
            raise Exception("Failed starting cmake server")

        packet = cmakelib.waitForRawMessage(self.proc)

        if packet == None:
            raise Exception("Unknown failure, maybe cmake does not support server mode")

        if packet['type'] != 'hello':
            raise Exception("No hello message received from server")

        self.protocolVersion = [ packet["supportedProtocolVersions"][0]["major"], packet["supportedProtocolVersions"][0]["minor"] ]

        self.logger.debug("Server Protocol version: %s", self.protocolVersion)

        sourceDirectory = sourceDirectory.replace('\\', '/')
        buildDirectory = buildDirectory.replace('\\', '/')

        cmakelib.writePayload(self.proc, { 'type': 'handshake', 
                                           'protocolVersion': { 'major': self.protocolVersion[0], 'minor' : self.protocolVersion[1] },
                                           'cookie': 'OPEN_HANDSHAKE', 'sourceDirectory': sourceDirectory, 'buildDirectory': buildDirectory,
                                           'generator': generatorName, 'extraGenerator': extraGeneratorName })

        reply = cmakelib.waitForReply(self.proc, 'handshake', 'OPEN_HANDSHAKE', False)

        cmakelib.writePayload(self.proc, { "type": "globalSettings"} )
        packet = cmakelib.waitForReply(self.proc, 'globalSettings', '', False)

        self.globalSettings = packet
        self.logger.debug("VERSION: %s" % self.globalSettings["capabilities"]["version"]["string"])

    def waitForResult(self, expectedReply, expectedCookie):
        while 1:
            payload = cmakelib.waitForRawMessage(self.proc)
            if payload["inReplyTo"] != expectedReply or payload["cookie"] != expectedCookie:
                raise Exception("Invalid packet received")

            msgType = payload["type"]
            if msgType == 'reply':
                break
            elif msgType == 'message':
                print("--", payload["message"])
            elif msgType == 'progress':
                pass
            elif msgType == 'error':
                raise Exception("Error occured during configure:", payload["errorMessage"])
            else:
                raise Exception("Invalid response:", payload)


    def configure(self, extraArguments = []):
        self.logger.info("Configuring ...")

        cmakelib.writePayload(self.proc, { "type":"configure", "cacheArguments": extraArguments, "cookie":"CONFIGURE" } )
        self.waitForResult("configure", "CONFIGURE")

        self.logger.info("Done.")

        self.logger.info("Generating ...")
        
        cmakelib.writePayload(self.proc, { "type":"compute", "cookie":"COMPUTE" } )
        self.waitForResult("compute", "COMPUTE")

        cmakelib.writePayload(self.proc, { "type":"codemodel", "cookie":"CODEMODEL" } )
        payload = cmakelib.waitForRawMessage(self.proc)

        if not payload or not "cookie" in payload or payload["cookie"] != "CODEMODEL":
            raise Exception("Something went wrong trying to configure the project. ( Unexpected response from cmake during codemodel request: %s )" % (payload))

        self.codeModel = payload

    def executableTarget(self, config, targetName):
        cmakeTargetToRun = None

        for cmakeConfiguration in self.codeModel["configurations"]:
            if cmakeConfiguration["name"] == '' or cmakeConfiguration["name"] == config:
                self.logger.debug("Found config: %s", cmakeConfiguration["name"])

                for cmakeProject in cmakeConfiguration["projects"]:
                    self.logger.debug(" Found project: %s", cmakeProject["name"])

                    for cmakeTarget in cmakeProject["targets"]:
                        self.logger.debug("  Found target: %s", cmakeTarget["name"])

                        if cmakeTarget["name"] == targetName:
                            cmakeTargetToRun = cmakeTarget

        if not cmakeTargetToRun:
            raise error.ProgramArgumentError("Couldn't find module %s" % self.args.target)

        if cmakeTargetToRun["type"] != "EXECUTABLE":
            raise error.ProgramArgumentError("Module %s is not an executable" % self.args.target)

        return cmakeTargetToRun

    def executableArtifactPath(self, target):
        executableArtifact = None
        executableName = target["fullName"]

        for artifact in target["artifacts"]:
            path, name = os.path.split(artifact)
            if name == executableName:
                executableArtifact = artifact
                break

        return executableArtifact