import os
import tempfile
import shutil
import subprocess
import logging
import sys

from string import Template

class AndroidStudioProjectGenerator(object):
    def __init__(self, gradle, cmake, platformBuildDir, androidBuildApiVersion):
        self.logger = logging.getLogger(__name__)
        self.project_dir = platformBuildDir;
        self.gradle = gradle
        self.cmake = cmake
        self.androidBuildApiVersion = androidBuildApiVersion
        self.android_support_dir = os.path.normpath(os.path.join(os.path.realpath(__file__), "..", "android-support"))

        self.logger.debug("Android support directory: %s" %(self.android_support_dir))

    def getGradleDependency(self):
        return "classpath 'com.android.tools.build:gradle:3.2.1'"

    def prepare_gradle(self):
        # the underlying commandline build system for android is gradle.
        # Gradle uses a launcher script (called the gradle wrapper)
        # that enforces the use of a specific desired gradle version.
        # The launcher script will automatically download the correct
        # version if needed and use that.
        # Any version of gradle can generate the wrapper for any desired
        # version.
        # So now we need to generate the gradle wrapper for "our" version.
        # Right now we use gradle 4.1

        # Unfortunately gradle has the problem that it will try to load the build.gradle files if
        # it finds them in the build directory, even if we only want to generate the wrapper.
        # And loading the build.gradle may fail if the currently installed default
        # gradle version is incorrect.
        # So to avoid this problem we generate the wrapper in a temporary directory and then move it to the desired location.

        gradle_temp_dir = tempfile.mkdtemp();
        try:
            gradle_path = self.gradle.getGradlePath()

            subprocess.check_call( '"%s" wrapper --gradle-version=4.10.2' % (gradle_path),
                                   shell=True,
                                   cwd=gradle_temp_dir);

            for name in os.listdir(gradle_temp_dir):
                source_path = os.path.join( gradle_temp_dir, name)
                dest_path = os.path.join( self.project_dir, name)

                if os.path.isdir(dest_path):
                    shutil.rmtree(dest_path)
                elif os.path.exists(dest_path):
                    os.remove(dest_path)

                shutil.move( source_path, dest_path )

        finally:
            shutil.rmtree(gradle_temp_dir)


    def find_applications(self, project, args):
        result = []
        for target in project["targets"]:
            if target["type"] == "EXECUTABLE":
                if args.target and args.target != target["name"]:
                    continue
                result += [target]

        return result;

    def find_libraries(self, project):
        result = []
        for target in project["targets"]:
            if not target["type"] == "EXECUTABLE":
                result += [target]

        return result;

    def create_top_level_build_gradle(self):
        gradle_template = Template(open(os.path.join(self.android_support_dir, "top.build.gradle.in"), "r").read())
        result = gradle_template.substitute(gradle_dependency = self.getGradleDependency())

        open(os.path.join(self.project_dir, "build.gradle"), "w").write(result)

    def create_settings_gradle(self, apps):
        module_list = ", ".join(list(("':%s'" % (target["name"]) for target in apps)))

        gradle_template = Template(open(os.path.join(self.android_support_dir, "settings.gradle.in"), "r").read())
        result = gradle_template.substitute(include_list = module_list )
        open(os.path.join(self.project_dir, "settings.gradle"), "w").write(result)

    def de_unicode(self, string):
        if sys.version_info <= (3,0):
            return string.encode("utf-8")
        else:
            return string

    def gather_directories(self, app, project):
        targets = self.find_libraries(project)
        targets += [app]

        source_directories = list( ( self.de_unicode(target["sourceDirectory"] + "/src") for target in targets ))
        include_directories = list( ( self.de_unicode(target["sourceDirectory"] + "/include") for target in targets ))
        java_directories = list( ( self.de_unicode(target["sourceDirectory"] + "/java") for target in targets ))

        return (source_directories, include_directories, java_directories)     

    def create_target_build_gradle(self, module_directory, app, project, android_abi, android_dependencies, target_dependencies):
        self.make_directory(module_directory)
        directories = self.gather_directories(app, project)

        gradle_template = Template(open(os.path.join(self.android_support_dir, "target.build.gradle.in"), "r").read())
        
        cmakelists_path = os.path.join(project["sourceDirectory"], "CMakeLists.txt").replace('\\', '/')

        abi_filter_string = ""
        if android_abi:
            abi_filter_string = "abiFilters '%s'" % android_abi

        android_dependecy_string = "".join(list( ("    implementation '%s'\n" % (dependency) for dependency in android_dependencies)))
        target_dependency_string = "" #"".join(list( ("    implementation project(':%s')\n" % dependency for dependency in target_dependencies)))
        targets = [app["name"]]
        cmake_target_list = ", ".join( list(('"%s"' % (target) for target in targets)))

        result = gradle_template.substitute(
            compile_sdk_version = self.androidBuildApiVersion,
            target_sdk_version = self.androidBuildApiVersion,
            application_id = "io.boden.android." + app["name"],
            min_sdk_version = 23,
            version_code = 1,
            version_name = "1.0",
            cmake_target_list = cmake_target_list,
            cmake_target_arguments = '"-DANDROID_STL=c++_static", "-DANDROID_CPP_FEATURES=rtti exceptions"',
            abi_filter = abi_filter_string, 
            cpp_flags = "-std=c++17 -frtti -fexceptions",
            cmakelists_path = cmakelists_path,
            cmake_version = self.cmake.globalSettings["capabilities"]["version"]["string"],
            jni_src_dir_list = directories[0] + directories[1],
            java_src_dir_list = directories[2],
            android_dependencies = android_dependecy_string,
            android_module_dependency_code = target_dependency_string )

        open(os.path.join(module_directory, "build.gradle"), "w").write(result)

    def create_target_strings_xml(self, module_directory, app):
        directory = self.make_directory(os.path.join(module_directory, "src", "main", "res", "values"))

        strings_template = Template(open(os.path.join(self.android_support_dir, "strings.xml.in"), "r").read())

        strings = '<string name="app_name">%s</string>' % (app["name"])
        result = strings_template.substitute( xml_string_entries = strings )

        open(os.path.join(directory, "strings.xml"), "w").write(result)

    def copy_android_manifest(self, module_directory, app):
        directory = self.make_directory(os.path.join(module_directory, "src", "main"))
        build_directory = app['buildDirectory']

        src = os.path.join(build_directory, "AndroidManifest.xml")
        dest = os.path.join(directory, "AndroidManifest.xml")

        shutil.copy(src, dest)

    def generate(self, project, androidAbi, target_dependencies, args):
        if not os.path.isdir(self.project_dir):
            os.makedirs(self.project_dir);

        android_dependencies = self.cmake.cache["BAUER_ANDROID_DEPENDENCIES"].split(';')
        self.logger.debug("Dependencies: %s" % android_dependencies)

        self.prepare_gradle()
        self.create_top_level_build_gradle()

        apps = self.find_applications(project, args)

        self.create_settings_gradle(apps)

        for app in apps:
            module_directory = os.path.join(self.project_dir, app["name"]);

            self.create_target_build_gradle(module_directory, app, project, androidAbi, android_dependencies, target_dependencies[app["name"]])
            self.create_target_strings_xml(module_directory, app)
            self.copy_android_manifest(module_directory, app)

    def make_directory(self, path):
        if not os.path.isdir(path):
            os.makedirs(path)

        return path

