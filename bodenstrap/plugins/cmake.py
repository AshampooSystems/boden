
import sys
import shutil
import os
import os.path
import subprocess

from .. import port
from .. import util
from .. import plugin

def get_plugins():
    return [CMakePlugin()]

class CMakePlugin(plugin.Plugin):

    def can_handle(self, component_name):
        return component_name == "cmake"

    def detect_global_versions(self, component_name):
        ver = util.make_generic_get_version_call("cmake --version")
        if ver:
            return [ plugin.GlobalVersion(ver, {}) ]
        else:
            return []


    def verify_valid_install_version(self, component_name, version):

        if version=="latest":
            raise Exception("You must specify an explicit installation version for CMake (append it in square brackets). 'latest' is not supported.")

        ver_ints = util.get_version_ints(version)
        if len(ver_ints)<2:
            raise Exception("Invalid CMake version number: "+version)


    def install(self, component_name, version, version_dir, dep_man):

        ver_ints = util.get_version_ints(version)
        if len(ver_ints)<2:
            raise Exception("Could not parse version number: "+version)

        while len(ver_ints)<3:
            ver_ints = ver_ints + [0]

        temp_dir = util.TempDir()

        if util.is_windows():            
            url = "https://cmake.org/files/v%d.%d/cmake-%d.%d.%d-win32-x86.zip" % (ver_ints[0], ver_ints[1], ver_ints[0], ver_ints[1], ver_ints[2])
            download_path = temp_dir.sub_path("cmake.zip")
            util.download_file( url, download_path )
            util.unzip_file( download_path, version_dir )

            # the downloaded archives contain a subdirectory called "cmake-VERSION", sometimes also called
            # cmake-VERSION-PLATFORM-ARCH.
            # We do not try to predict it - instead we detect it automatically.
            cmake_subdir = util.find_subdir_name_with_item( version_dir, ["bin"])

        else:
            url = "https://cmake.org/files/v%d.%d/cmake-%d.%d.%d.tar.gz" % (ver_ints[0], ver_ints[1], ver_ints[0], ver_ints[1], ver_ints[2])
            download_path = temp_dir.sub_path("cmake.tar.gz")
                        
            util.download_file( url, download_path )
            util.untargz_file( download_path, version_dir )

            # the downloaded archives contain a subdirectory called "cmake-VERSION", sometimes also called
            # cmake-VERSION-PLATFORM-ARCH.
            # We do not try to predict it - instead we detect it automatically.
            cmake_subdir = util.find_subdir_name_with_item( version_dir, ["bootstrap"])
        
        if not cmake_subdir:
            raise Exception("Downloaded CMake archive does not have expected contents.")

        if not util.is_windows():
            build_dir = os.path.join( version_dir, cmake_subdir )

            util.print_info("Building...")

            install_subdir = "inst"

            # note that we redirect the stdout of the called processes to our stderr
            subprocess.check_call( './bootstrap --prefix="%s"' % (os.path.join(version_dir, install_subdir)), shell=True, cwd=build_dir, stdout=sys.stderr)
            subprocess.check_call( 'make', shell=True, cwd=build_dir, stdout=sys.stderr)

            util.print_info("Installing isolated...")

            subprocess.check_call( 'make install', shell=True, cwd=build_dir, stdout=sys.stderr)

            util.print_info("Removing build files...")
            shutil.rmtree(build_dir)

            cmake_subdir = install_subdir            

        return plugin.InstallResult(version, {"subdir": cmake_subdir}, False )


    def activate(self, env, component_name, version, version_data, version_dir, dep_man):

        if version_dir:
            cmake_dir = os.path.join(version_dir, version_data["subdir"])

            dep_man.prepend_to_path_list_env_var(env, "PATH", os.path.join(cmake_dir, "bin") )

        else:
            # this is the globally installed version. Nothing to do.
            pass
