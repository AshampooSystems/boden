
import shutil
import os
import os.path
import distutils
from distutils.versionpredicate import VersionPredicate
from distutils.version import LooseVersion, StrictVersion
import json
import random
import string
import types

from . import port
from . import util


def get_effective_dependency_dir(dep_dir):
    """ Returns the effective dependency directory path for a given
        input path.
        If dep_dir is not None then it is used directly.
        If it is None then the contents of the BODEN_STRAP_REPO environment
        variable are used. If that is not set then a subdirectory
        "boden-strap-data" in the current working directory is used.

        The function ensures that the returned directory exists.
        """

    if not dep_dir:
        dep_dir = os.environ.get("BODEN_STRAP_REPO")
        if not dep_dir:

            if util.is_windows():
                # we want the path to be as short as possible. Otherwise we risk exceeding MAX_PATH on windows.
                dep_dir = os.path.expandvars( os.path.join("%LOCALAPPDATA%", "boden-strap", "r") )
            else:
                dep_dir = os.path.expanduser( os.path.join("~", ".boden-strap", "r") )

    if not os.path.exists(dep_dir):
        os.makedirs(dep_dir)

    return dep_dir



class DependencyInfo:

    def __init__(self, dependency_string):
        """ Initializes a dependency info from a string.

        dependency_string must have the following structure:

        COMPONENT_NAME (VERSION_CONDITIONS)[INSTALL_VERSION]

        COMPONENT_NAME is the name of the component

        VERSION_CONDITIONS are conditions that the component version must satisfy.
        VERSION_CONDITIONS have the same structure as those from the python
        package management / distutils.
        VERSION_CONDITIONS and the surrounding round brackets can also be left out
        completely if any version is acceptable.

        INSTALL_VERSION indicates the version to install when the component
        is missing or does not satisfy the conditions.
        Some components support the special string "latest" for INSTALL_VERSION,
        which installs the latest stable version.
        If the [INSTALL_VERSION] part is missing completely then [latest] is assumed.
        """

        self._dependency_string = dependency_string

        self.install_version = None
        version_pred_string, sep, suffix = dependency_string.partition("[")
        if sep:
            if suffix.endswith("]"):            
                self.install_version = suffix[:-1]
        else:
            self.install_version = "latest"

            
        if not self.install_version or not version_pred_string:
            raise Exception("Invalid dependency string: "+dependency_string)

        
        # we use distutils to parse the version information.

        # Unfortunately, VersionPredicate only allows very few special characters.
        # Only alnum, dots and underscores are allowed.
        # We use escaping To allow us to handle all characters.

        name_part, sep, version_part = version_pred_string.partition("(")

        self.component_name = name_part.strip()

        if not sep:
            # no version conditions
            self._version_pred = distutils.versionpredicate.VersionPredicate("dummy")
        else:
            self._version_pred = distutils.versionpredicate.VersionPredicate("dummy" + sep + version_part)


    def __str__(self):
        return self._dependency_string


    def is_satisfied_by_version(self, version_string):
        # VersionPredicate cannot handle unicode strings in Python2
        if port.is_python2() and type(version_string)==types.UnicodeType:
            version_string = str(version_string)

        # we want to support version numbers that have only a single
        # component. However, satisfied_by uses StrictVersion internally.
        # which requires at least two. So we add a dummy number at the end
        # if necessary.
        if "." not in version_string:
            version_string+=".0"

        return self._version_pred.satisfied_by(version_string)



class DependencyManager:

    def __init__(self, managed_dep_dir, plugin_manager):

        self.managed_dep_dir = get_effective_dependency_dir(managed_dep_dir)
        self.plugin_manager = plugin_manager
        self._using_printed_set = set()

        self._currently_installing_list = []


    def prepend_to_path_list_env_var(self, env, var_name, path):
        old_value = env.get(var_name)
        if old_value:
            sep = ";" if util.is_windows() else ":"
            env[var_name] = path+sep+old_value
        
        else:
            env[var_name] = path            


    def _get_version_data_path(self, ver_dir):
        return os.path.join(ver_dir, "_version_data.json")


    def _try_load_version_data(self, ver_dir):
        try:
            with open( self._get_version_data_path(ver_dir), "rb") as f:
                return json.loads(f.read().decode("utf-8"))
        
        except:
            raise
            return None

    def _store_version_data(self, ver_dir, version_data):
        with open( self._get_version_data_path(ver_dir), "wb") as f:
            return f.write(json.dumps(version_data).encode("utf-8") )


    class CandidateVersion:
        def __init__(self, version, data, dir, self_managed):
            self.version=version

            if version:
                self.version_obj = LooseVersion(version)
            else:
                self.version_obj = None

            self.data = data
            self.dir = dir
            self.self_managed = self_managed

    def is_valid_version_subdir(self, dir_name):

        if dir_name.startswith("_"):
            # temp dir
            return False


        valid_number = False
        try:
            LooseVersion(dir_name)
            valid_number = True
        except:
            pass

        return dir_name=="sm" or valid_number

    def _get_managed_installed_versions(self, plugin, component_name):
        """ Returns a list with CandidateVersion instances."""

        result_list = []

        comp_dir = os.path.join( self.managed_dep_dir, component_name )
        if os.path.exists(comp_dir):            
            for subdir_name in os.listdir( comp_dir ):

                if self.is_valid_version_subdir(subdir_name):
                    version_dir = os.path.join(comp_dir, subdir_name )

                    if os.path.isdir(version_dir):

                        try:

                            version_data = self._try_load_version_data(version_dir)

                        except:
                            version_data = None
                            
                        if version_data is not None:

                            if subdir_name=="sm":
                                self_managed = True
                                version = plugin.get_self_managed_version( component_name, version_data, version_dir, self )
                            else:
                                self_managed = False
                                version = subdir_name

                            result_list.append( self.CandidateVersion( version, version_data, version_dir, self_managed ) )

                            
                            

        return result_list


    def print_using(self, comp_name, candidate):

        t = (comp_name, str(candidate.version))
        if t not in self._using_printed_set:


            if candidate.dir is None:
                from_where = "(globally installed version)"
            else:
                from_where = "(local version)"


            util.print_info("Using %s v%s %s" % (comp_name, candidate.version, from_where) )
            self._using_printed_set.add( t )



    def ensure_installed_and_activate(self, env, dependency):
        """ Ensures that a dependency matching the dependency specifier is installed.

        dependency must be either a DependencyInfo instance or a dependency string.

        Also modified the specified environment variable dictionary so that the dependency
        is active when these environment variables are active.
        """

        if isinstance(dependency, DependencyInfo):
            dep_info = dependency
        else:
            dep_info = DependencyInfo(dependency)

        comp_name = dep_info.component_name

        if comp_name.lower() in self._currently_installing_list:
            raise Exception("Dependency cycle detected. %s is already being installed. Currently installing: %s" % (comp_name, str(_currently_installing_list) ) )

        self._currently_installing_list.append( comp_name.lower() )
        try:
            install_version_obj = LooseVersion(dep_info.install_version)

            plugin = self.plugin_manager.get_plugin( comp_name )

            # we always verify that the install version is valid. Otherwise we'll only find out
            # when we try to install.
            plugin.verify_valid_install_version(comp_name, dep_info.install_version)

            all_versions = []
            for global_ver in plugin.detect_global_versions( comp_name ):
                all_versions.append( self.CandidateVersion(global_ver.version, global_ver.opaque_data, None, False ) )

            all_versions.extend( self._get_managed_installed_versions( plugin, comp_name ) )

            candidate_list = []
            self_managed_list = []
            for candidate in all_versions:                
                if candidate.self_managed:
                    self_managed_list.append( candidate )
                if dep_info.is_satisfied_by_version(candidate.version):
                    candidate_list.append( candidate )

            if len(candidate_list)==0 and len(self_managed_list)>0:

                # we have no candidates that satisfy the conditions. But if we have a self managed
                # candidate then we can try to upgrade it to the newest version.
            
                candidate = self_managed_list[0]

                util.print_info("Currently installed %s %s does not satisfy the conditions. Trying to update it..." % (comp_name, candidate.version) )

                version = plugin.update_self_managed(comp_name, candidate.dir, candidate.data, self)
                if version==candidate.version:
                    util.print_info("No update available.")
                else:
                    util.print_info("Updated to %s" % version)                    

                    if dep_info.is_satisfied_by_version(version):
                        candidate_list.append( self.CandidateVersion (version, version_data, version_dir, True))

                if len(candidate_list)==0:
                    raise Exception("Cannot satisfy version condition: even the latest version %s %s does not satisfy it." % (comp_name, version) )

            if len(candidate_list)==0 and (len(self_managed_list)==0 or dep_info.install_version!="latest"):
                # we do not want the temp folder names to be too long. That can cause problems
                # on some platforms (e.g. windows). So we only use few random digits
                while True:
                    temp_name = "_" + ''.join(random.choice(string.ascii_lowercase + string.digits) for i in range(4))
                    temp_ver_dir = os.path.join( self.managed_dep_dir, comp_name, temp_name)
                    if not os.path.exists(temp_ver_dir):
                        break


                util.print_info("Installing %s [%s] (local isolated installation)..." % (comp_name, dep_info.install_version))

                os.makedirs(temp_ver_dir)
                try:
                    install_result = plugin.install(comp_name, dep_info.install_version, temp_ver_dir, self)

                    # ensure that we have a valid version number
                    version_is_ok = False
                    try:                        
                        if install_result.installed_version:
                            LooseVersion(install_result.installed_version)                        
                            version_is_ok = True
                    except:
                        pass

                    if not version_is_ok:
                        raise Exception("Plugin has returned an invalid installed_version: %s" % install_result.installed_version)

                    if install_result.self_managed:
                        ver_dir_name = "sm"

                    else:
                        ver_dir_name = install_result.installed_version
                    
                    self._store_version_data( temp_ver_dir, install_result.opaque_data )

                    ver_dir = os.path.join( self.managed_dep_dir, comp_name, ver_dir_name)

                    if os.path.exists(ver_dir):
                        util.print_info("Replacing old installation in "+ver_dir)
                        old_name = "_old_" + ''.join(random.choice(string.ascii_lowercase + string.digits) for i in range(10))
                        old_ver_dir = os.path.join( self.managed_dep_dir, comp_name, old_name )
                        os.rename(ver_dir, old_ver_dir)
                        os.rename( temp_ver_dir, ver_dir)

                        shutil.rmtree( old_ver_dir )

                    else:
                        os.rename( temp_ver_dir, ver_dir)

                except:                        
                    
                    try:
                        if os.path.exists(temp_ver_dir):
                            pass #shutil.rmtree(temp_ver_dir)
                    except:
                        pass
                    
                    raise

                util.print_info("Finished installing %s v%s (local isolated installation)." % (comp_name, install_result.installed_version))

                candidate_list.append( self.CandidateVersion(install_result.installed_version, install_result.opaque_data, ver_dir, install_result.self_managed) )

            # sort ascending by version number
            candidate_list.sort( key = lambda c: c.version_obj )

            # use the highest matching version
            chosen = candidate_list[-1]

            if chosen.self_managed:
                activate_version = "selfmanaged"
            else:
                activate_version = chosen.version

            plugin.activate( env, comp_name, activate_version, chosen.data, chosen.dir, self )

            self.print_using(comp_name, chosen)

        finally:
            del self._currently_installing_list[-1]



    def prepare_environment(self, dependency_list):
        """ Prepares an environment variable dictionary, so that the specified
        dependencies are available when a process is called with this environment.

        dependency_list must be a list with dependency strings or DependencyInfo instances.

        Missing dependencies are automatically downloaded and installed locally.

        Returns a dictionary with the complete environment variables that should be used
        to execute a subprocess with the specified dependencies.
        The function starts with the normal environment variables of the current process and
        then applies all environment patches for the dependencies. The resulting dictionary
        is returned.
        """

        env = {}
        env.update(os.environ)

        for dep in dependency_list:
            self.ensure_installed_and_activate(env, dep) 

        return env



