import subprocess 
import re
import logging

import error

class IOSInfo:
    def __init__(self):
        self.logger = logging.getLogger(__name__)

    def getSelectedDeviceType(self, args):
        if not args.ios_device_type:
            return self.getNewestIPhoneDeviceType()
        else:
            return self.findDeviceType(args.ios_device_type)

    def getSelectedOS(self, args):
        if not args.ios_simulator_os:
            return self.getHighestAvailableSimulatorOS('iOS')
        else:
            return self.findSimulatorOS(args.ios_simulator_os)

    def getAvailableDeviceTypes(self):
        result = subprocess.check_output("xcrun simctl list", shell=True).decode(encoding='utf-8')

        simDeviceRegex = r"(.*) \((.*SimDeviceType.*)\)"

        devices = {}

        matches = re.finditer(simDeviceRegex, result, re.MULTILINE)
        for matchNum, match in enumerate(matches):
            name = match.groups()[0]
            deviceId = match.groups()[1]

            devices[name] = deviceId
        return devices

    def getAvailableSimulatorOSVersions(self):
        result = subprocess.check_output("xcrun simctl list", shell=True).decode(encoding='utf-8')

        simRuntimeRegex = r"(.*) ([0-9]+\.[0-9]+).*(com\.apple\.CoreSimulator\.SimRuntime\..*)"

        versions = {}

        matches = re.finditer(simRuntimeRegex, result, re.MULTILINE)
        for matchNum, match in enumerate(matches):
            osType = match.groups()[0]
            version = float(match.groups()[1])
            name = match.groups()[2]

            if not osType in versions:
                versions[osType] = {}
            versions[osType][version] = name

        return versions

    def getHighestAvailableSimulatorOS(self, osType):
        versions = self.getAvailableSimulatorOSVersions()
        if len(versions) == 0:
            raise Exception("No iOS Simulators installed!")
        if len(versions[osType]) == 0:
            raise Exception("No iOS Simulators installed!")

        return versions[osType][max(versions[osType])]

    def getNewestIPhoneDeviceType(self):
        deviceTypes = self.getAvailableDeviceTypes()
        if deviceTypes == 0:
            raise Exception("No iPhone device types found!")

        iPhones = {k: v for k, v in deviceTypes.items() if 'iPhone' in k}
        # sorting works right now where iPhone X is the newest, might break in the future! 
        newest = sorted(iPhones.keys())[-1]

        return iPhones[newest]

    def findDeviceType(self, name):
        deviceTypes = self.getAvailableDeviceTypes()
        if name in deviceTypes:
            return deviceTypes[name]

        self.logger.critical("Couldn't find Device type %s", name)
        self.logger.info("Available device types:")
        for name, deviceId in deviceTypes.items():
            self.logger.info("  %s" % (name))

        raise error.ProgramArgumentError("--ios-device-type invalid")

    def findSimulatorOS(self, name):
        versions = self.getAvailableSimulatorOSVersions()
        if ' ' in name:
            n,version = name.split(' ')[:2]
            fversion = float(version)

            if n in versions:
                if fversion in versions[n]:
                    return versions[n][fversion]
        
        self.logger.critical("Couldn't find iOS version %s", name)
        self.logger.info("Available versions:")
        for type, v in versions.items():
            for version in v.keys():
                self.logger.info("  %s %s" % (type, version))

        raise error.ProgramArgumentError("--ios-simulator-version invalid")

