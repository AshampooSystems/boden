
import re
import os
import os.path
import subprocess

from .. import port
from .. import util
from .. import plugin


def get_plugins():
    return [JavaPlugin()]


class JavaPlugin(plugin.Plugin):

    def can_handle(self, component_name):
        return component_name == "java"


    def _get_java_version(self, env=None):            
        try:
            output = port.call_return_output("java -version", env=env, shell=True, stderr=subprocess.STDOUT)
        except:
            output = ""

        # the output has multiple lines. The first one identifies the java version.
        # Other version numbers may follow on the next lines.
        line_list = output.splitlines()
        if len(line_list)==0:
            ver = ""

        else:
            first_line = line_list[0]

            # first look for a "normal" version number
            ver = util.extract_version(first_line)

            # if we find one and it starts with "1." then it is a developer
            # version. The product version omits the 1. prefix            
            if ver and ver.startswith("1."):
                ver = ver[2:]

            if not ver:
                # OpenJDK only lists the major version. We use the first
                # number that is not surrounded by alnum
                m = re.search("[^a-zA-Z\\d\\.-](\\d+)[^a-zA-Z\\d\\.-]", first_line)
                if m is not None:
                    ver = m.group(1)

        return ver

    def detect_global_versions(self, component_name):
        
        ver = self._get_java_version()

        if ver:
            return [ plugin.GlobalVersion(ver, {}) ]
        else:
            return []


    def _parse_install_version(self, version):
        if version=="latest":
            raise Exception("You must specify an explicit installation version for Java (append it in square brackets). 'latest' is not supported.")

        ver_ints = util.get_version_ints(version)
        if len(ver_ints)<1:
            raise Exception("Invalid Java version number: "+version)

        if len(ver_ints)>2:
            raise Exception("Please only specify the major version in the 'install version' part. For example, '11' for Java 11.")

        return ver_ints[0]


    def verify_valid_install_version(self, component_name, version):
        self._parse_install_version(version)


    def install(self, component_name, version, version_dir, dep_man):

        major_version = self._parse_install_version(version)

        temp_dir = util.TempDir()

        # note: the "ga" in the URL stands for "general availability"

        extension = ".tar.gz"

        if util.is_windows():
            os_part = "windows-x64"
            extension = ".zip"
        elif util.is_mac():
            os_part = "osx-x64"
        elif util.is_linux():
            os_part = "linux-x64"
        else:
            raise Exception("Java can only be automatically installed on macOS, Windows and Linux.")

        url = "https://download.java.net/java/ga/jdk%d/openjdk-%d_%s_bin%s" % (major_version, major_version, os_part, extension)

        download_path = temp_dir.sub_path("jdk"+extension)
                    
        util.download_file( url, download_path )
        if extension==".zip":
            util.unzip_file( download_path, version_dir, restore_unix_mode = True )
        else:
            util.untargz_file( download_path, version_dir )

        # the downloaded archives contain a subdirectory with the actual data.
        # detect it automatically
        if util.is_mac():
            jdk_subdir = util.find_subdir_name_with_item(version_dir, ["Contents", "Home"] )
            if jdk_subdir:
                home_subdir = os.path.join(jdk_subdir, "Contents", "Home" )

        else:
            home_subdir = util.find_subdir_name_with_item(version_dir, ["bin"] )
        if not home_subdir:
            raise Exception("Downloaded Java archive does not have expected contents.")

        bin_subdir = os.path.join(home_subdir, "bin")

        version_data = {"bin_subdir": bin_subdir, "home_subdir": home_subdir}

        # get the version number that was actually installed
        call_env = {}
        call_env.update( os.environ )
        # delete the existing PATH variable to ensure that we do not
        # accidentally end up with the version of a global installation if something went wrong
        del call_env["PATH"]

        self.activate(call_env, "java", version, version_data, version_dir, dep_man)

        version = self._get_java_version(env=call_env)        
        if not version:
            raise Exception("Error: new Java installation is apparently not functional.")

        return plugin.InstallResult(version, version_data, False )


    def activate(self, env, component_name, version, version_data, version_dir, dep_man):

        if version_dir:
            bin_dir = os.path.join(version_dir, version_data["bin_subdir"])
            home_dir = os.path.join(version_dir, version_data["home_subdir"])

            dep_man.prepend_to_path_list_env_var(env, "PATH", bin_dir )
            env["JAVA_HOME"] = home_dir
            env["JDK_HOME"] = home_dir

        else:
            # this is the globally installed version. Nothing to do.
            pass

