import os

from buildconfiguration import BuildConfiguration

def listDirectories(dir):
    return next(os.walk(dir))[1]

class BuildFolder:
    def __init__(self, bauerGlobals, generatorInfo, rootPath, args):
        self.args = args
        self.bauerGlobals = bauerGlobals
        self.generatorInfo = generatorInfo

        if args.build_folder != None:
            self.buildfolder = args.build_folder
        else:
            self.buildfolder = os.path.join(rootPath, "build")

    def getBaseBuildDir(self):
        return self.buildfolder


    # Scans for existing build configurations
    def getExistingBuildConfigurations(self):
        buildDir = self.getBaseBuildDir()
        prepared = []
        if os.path.isdir(buildDir):
            for platform in listDirectories(buildDir):
                if platform not in self.bauerGlobals.platformMap:
                    continue

                for arch in listDirectories(os.path.join(buildDir, platform)):
                    for buildsystem in listDirectories(os.path.join(buildDir, platform, arch)):
                        if self.generatorInfo.isSingleConfigBuildSystem(self.generatorInfo.getBuildSystemForFolderName(buildsystem)):
                            for config in listDirectories( os.path.join(buildDir, platform, arch, buildsystem)):
                                if os.path.exists( os.path.join(buildDir, platform, arch, buildsystem, config, '.generateProjects.state') ):
                                    prepared.append( BuildConfiguration(platform=platform, arch=arch, buildsystem=buildsystem, config=config) )
                        else:
                            if os.path.exists( os.path.join(buildDir, platform, arch, buildsystem, '.generateProjects.state') ):
                                prepared.append( BuildConfiguration(platform=platform, arch=arch, buildsystem=buildsystem, config=None) )
        return prepared

    # Returns the closest match to the user selected configuration
    def getMatchingBuildConfigurations(self, existingConfigurations):
        matches = []

        for configuration in existingConfigurations:
            if self.args.platform != None and self.args.platform != configuration.platform:
                continue
            if self.args.arch != None and self.args.arch != configuration.arch:
                continue
            if self.args.build_system != None and self.args.build_system != configuration.buildsystem:
                continue
            if self.args.config != None and configuration[3] != None and self.args.config != configuration.config:
                continue

            matches.append(configuration)

        return matches

    def getBuildConfigurationsForCommand(self):
        existingConfigurations = self.getExistingBuildConfigurations()

        matchedConfigurations = self.getMatchingBuildConfigurations(existingConfigurations)

        # User specified configuration:
        if self.args.platform != None and self.args.build_system != None:
            if len(matchedConfigurations) > 0 and self.args.arch == None:
                return matchedConfigurations

            isSingleConfigBuildSystem = self.generatorInfo.isSingleConfigBuildSystem(self.args.build_system)

            if not isSingleConfigBuildSystem or self.args.config != None:
                arch = self.args.arch
                if not arch:
                    arch = 'std'

                config = self.args.config
                if not isSingleConfigBuildSystem:
                    config = None

                selectedConfiguration = [ BuildConfiguration(platform=self.args.platform, arch=arch, buildsystem=self.args.build_system, config=config) ]

                return selectedConfiguration

        return matchedConfigurations

    def getBuildDir(self, configuration):
        result = self.getBaseBuildDir()

        result = os.path.join(result, *filter(None, list(configuration)))

        return result