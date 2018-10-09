
import os
import os.path
import subprocess

from .. import port
from .. import util
from .. import plugin


def get_plugins():
    return [GradlePlugin()]


class GradlePlugin(plugin.Plugin):

    def can_handle(self, component_name):
        return component_name == "gradle"

    def detect_global_versions(self, component_name):
        ver = util.make_generic_get_version_call("gradle --version")
        if ver:
            return [ plugin.GlobalVersion(ver, {}) ]
        else:
            return []

    def _parse_version(self, version):
        if version=="latest":
            raise Exception("You must specify an explicit installation version for Gradle (append it in square brackets). 'latest' is not supported.")

        ver_ints = util.get_version_ints(version)
        if len(ver_ints)<2 or len(ver_ints)>3:
            raise Exception("Invalid Gradle version number: "+version)

        return ver_ints


    def verify_valid_install_version(self, component_name, version):
        self._parse_version(version)


    def _activate_java(self, env, dep_man):
        dep_man.ensure_installed_and_activate(env, "java(>=7.0)[11]" )

    def install(self, component_name, version, version_dir, dep_man):

        ver_ints = self._parse_version(version)

        env = {}
        env.update(os.environ)
        self._activate_java(env, dep_man)        

        temp_dir = util.TempDir()

        url = "https://services.gradle.org/distributions/gradle-%s-bin.zip" % (version)
        download_path = temp_dir.sub_path("gradle.zip")
                    
        util.download_file( url, download_path )
        util.unzip_file( download_path, version_dir, restore_unix_mode = True )

        # the downloaded archives contain a subdirectory called "gradle-VERSION".
        # But that might change. So we detect it automatically.
        gradle_subdir = util.find_subdir_name_with_item( version_dir, ["bin"])
        if not gradle_subdir:
            raise Exception("Downloaded Gradle archive does not have expected contents.")

        return plugin.InstallResult(version, {"subdir": gradle_subdir}, False )


    def activate(self, env, component_name, version, version_data, version_dir, dep_man):

        if version_dir:
            gradle_subdir = version_data["subdir"]

            self._activate_java(env, dep_man)

            dep_man.prepend_to_path_list_env_var(env, "PATH", os.path.join(version_dir, gradle_subdir, "bin") )

            env["GRADLE_HOME"] = version_dir

        else:
            # this is the globally installed version. Nothing to do.
            pass



