

import sys
import os
import os.path
import subprocess

from .. import port
from .. import util
from .. import plugin


def get_plugins():
    return [AndroidPlugin()]
    

class AndroidPlugin(plugin.Plugin):

    def can_handle(self, component_name):
        return component_name.startswith("android.")


    def is_valid_sdk_root(self, sdk_root):
        if not sdk_root:
            return False
        if not os.path.isdir(sdk_root):
            return False
        if not os.path.isdir( os.path.join(sdk_root, "tools", "bin") ):
            return False


    def try_get_sdk_root(self):
        alternatives_list = [
            os.environ.get("ANDROID_SDK_ROOT"),
            os.environ.get("ANDROID_HOME") ]
        
        if util.is_mac():
            alternatives_list.append( os.path.expanduser("~/Library/Android/sdk") )

        elif util.is_windows():
            alternatives_list.append( os.path.expandvars("%LOCALAPPDATA%\\Android\\Sdk") )

        else:
            alternatives_list.append( os.path.expanduser("~/.android/sdk") )


        for sdk_root in alternatives_list:            
            if self.is_valid_sdk_root(sdk_root):
                return sdk_root

        return None



    def get_android_package_name(self, component_name):
        prefix = "android."
        if not component_name.startswith( prefix ):
            raise Exception("Invalid android component name: "+component_name)

        return component_name[ len(prefix): ]


    def get_sdk_tools_version(self, sdk_root, ignore_error = False):
        output = ""
        try:
            output = self.call_sdk_manager( sdk_root, ["--version"], return_output=True)
        except:
            if ignore_error:
                pass
            else:
                raise

        ver = util.extract_version(output)
        return ver



    def detect_global_versions(self, component_name):

        sdk_root = self.try_get_sdk_root()
        if sdk_root:
            # The sdkmanager must be functional
            tools_ver = self.get_sdk_tools_version(sdk_root, ignore_error=True)
            if tools_ver:
                if component_name in ("android.tools", "android.sdk-root"):     
                    return [(tools_ver, {"sdk_root": sdk_root})]
                
                else:
                    android_package = self.get_android_package_name(component_name)

                    ver = self.get_installed_packages(sdk_root).get(android_package)
                    if ver:
                        return [ plugin.GlobalVersion(ver, {"sdk_root": sdk_root}) ]

        return []


    def verify_valid_install_version(self, component_name, version):
        # we cannot install any old version of the sdk tools, since we cannot reliably derive the download
        # URLs.
        # Instead we only install the latest ones, by first downloading a fixed SDK version and then
        # updating the sdk installation afterwards.

        if version!="latest":
            raise Exception("Only the latest version of %s can be installed. You must specify 'latest' as the install version." % component_name)


    def install(self, component_name, version, version_dir, dep_man):

        self.verify_valid_install_version(component_name, version)

        if component_name=="android.sdk-root":
            # android.sdk-root is a virtual component, representing the SDK installation.
            # it automatically includes the android tools

            if util.is_windows():
                platform_string = "windows"
            elif util.is_mac():
                platform_string = "darwin"
            elif util.is_linux():
                platform_string = "linux"
            else:
                raise Exception("Cannot install Android SDK tools because the current platform is unknown: "+sys.platform)

            temp_dir = util.TempDir()

            url = "https://dl.google.com/android/repository/sdk-tools-%s-4333796.zip" % platform_string
            download_path = temp_dir.sub_path("sdk-tools.zip")
            util.download_file( url, download_path )
            util.unzip_file( download_path, version_dir, restore_unix_mode=True )

            sdk_root = version_dir

            # The version of the SDK tools that we just downloaded is probably already outdated. So first we
            # update it.

            util.print_info("Updating Android SDK Tools...")

            self.call_sdk_manager(sdk_root, ["--install", "tools"] )

            util.print_info("Finished updating Android SDK Tools.")

            ver = self.get_sdk_tools_version(sdk_root)

            if not ver:
                raise Exception("Error: installed android SDK tools are not functional.")            

        else:

            env = {}
            dep_man.ensure_installed_and_activate(env, "android.sdk-root[latest]")
            sdk_root = env["ANDROID_SDK_ROOT"]

            android_package = self.get_android_package_name(component_name)

            util.print_info("Installing/updating android package %s..." % component_name)            

            self.call_sdk_manager(sdk_root, ["--install", android_package] )

            util.print_info("Finished installing/updating android package %s." %  component_name)

            if android_package=="tools":
                ver = self.get_sdk_tools_version(sdk_root)
            else:
                ver = self.get_installed_packages(sdk_root).get(android_package)

            if not ver:
                raise Exception("Android package apparently not installed successfully (cannot obtain version information)")

        return plugin.InstallResult(ver, {}, True )


    def get_self_managed_version(self, component_name, version_data, version_dir, dep_man):
        """ Returns the actual version number of a self managed component."""

        if component_name=="android.sdk-root":
            sdk_root = version_dir

            ver = self.get_sdk_tools_version(sdk_root)
            
        else:
            env = {}
            dep_man.ensure_installed_and_activate(env, "android.sdk-root")
            sdk_root = env["ANDROID_SDK_ROOT"]

            android_package = self.get_android_package_name(component_name)
            ver = self.get_installed_packages(sdk_root).get(android_package)

        if not ver:
            raise Exception("Error: Unable to determine version of %s component in '%s'." % (component_name, version_dir) )

        return ver

    def update_self_managed(self, component_name, version_data, version_dir, dep_man):

        if component_name=="android.sdk-root":
            util.print_info("Updating Android SDK Tools...")

            self.call_sdk_manager(sdk_root, ["--install", "tools"] )

            util.print_info("Finished updating Android SDK Tools.")

        else:
            env = {}
            dep_man.ensure_installed_and_activate(env, "android.sdk-root")
            sdk_root = env["ANDROID_SDK_ROOT"]

            util.print_info("Installing/updating android package %s..." % component_name)            

            android_package = self.get_android_package_name(component_name)
            self.call_sdk_manager(sdk_root, ["--install", android_package] )

            util.print_info("Finished installing/updating android package %s." %  component_name)

        return self.get_self_managed_version(component_name, version_data, version_dir, dep_man)



    def activate(self, env, component_name, version, version_data, version_dir, dep_man):

        if version!="selfmanaged":
            raise Exception("Version must be the special 'selfmanaged' value for component %s" % component_name)

        if component_name=="android.sdk-root":

            if version_dir is None:
                # The caller wants a globally installed component.
                # The version data contains the sdk root path
                sdk_root = version_data["sdk_root"]

            else:
                # this is a locally installed sdk. The sdk root is the version directory
                sdk_root = version_dir

            env["ANDROID_SDK_ROOT"] = sdk_root
            env["ANDROID_HOME"] = sdk_root

        else:
            # Since all SDK components are installed in the SDK location,
            # we activate other SDK components simply by activating the root component "android.tools".

            dep_man.ensure_installed_and_activate( env, "android.sdk-root" )



    def get_package_info(self, sdk_root):
        """ Returns a tuple (installed_map, available_map). installed_map refers to the installed
            packages, available_map to packages that can be installed.
            Each dictionary contains the package identifier (SDK path) as the key and the version number as the values.

            Note that quite often the a version number is included in the package identifier. In that case the
            version might only be a revision / update number.

            For example, a key of an Android system image is "system-images;android-20;google_apis;x86" and its value might be "2"
            (indicating that this image is at its second revision).
            """

        out = self.call_sdk_manager(sdk_root, ["--list"], return_output = True )

        installed_map = {}
        available_map = {}
        section_num = 0
        in_section = False
        for curr_line in out.splitlines():

            if not curr_line.startswith("  "):
                in_section = False

            if curr_line.startswith("  --"):                
                # a section header separator line
                section_num += 1
                in_section = True

            else:

                # we are in the first section (Installed Packages)
                if in_section:

                    cols = curr_line.split(" | ")
                    if len(cols)>=2:
                        name = cols[0].strip()
                        version = cols[1].strip()

                        if section_num == 1:
                            installed_map[name] = version

                        elif section_num == 2:
                            available_map[name] = version

        return (installed_map, available_map)

    def get_installed_packages(self, sdk_root):
        return self.get_package_info(sdk_root) [0]


    def call_executable_in_dir(self, exe_dir, exe_title, param_list, env, return_output = False):

        if util.is_windows():
            extension_list = [".exe", ".bat", ".cmd"]
        else:
            extension_list = [""]

        name_list = []
        for ext in extension_list:            
            name_list.append( (exe_title+ext).lower() )

        exe_path = None
        for name in os.listdir(exe_dir):
            if name.lower() in name_list:
                exe_path = os.path.join(exe_dir, name)

        if exe_path is None:
            raise Exception("Error: %s executable not found in %s" % (exe_title, exe_dir) )          

        arg_list = [ exe_path ] + param_list

        if return_output:

            output = port.call_return_output( arg_list,
                                            env = env )
            return output

        else:
            subprocess.check_call(  arg_list,
                                    env = env,
                                    stdout=sys.stderr)




    def call_sdk_executable(self, sdk_root, subdir_components, exe_title, param_list, return_output = False):

        exe_dir = os.path.join(sdk_root, *subdir_components)

        env = {}
        env.update( os.environ )
        env["ANDROID_HOME"] = sdk_root
        env["ANDROID_SDK_ROOT"] = sdk_root

        return self.call_executable_in_dir( exe_dir, exe_title, param_list, env, return_output = return_output)


    def call_sdk_manager(self, sdk_root, param_list, return_output = False):
        return self.call_sdk_executable( sdk_root, ["tools", "bin"], "sdkmanager", param_list, return_output = return_output)


