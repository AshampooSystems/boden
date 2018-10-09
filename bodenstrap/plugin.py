
from . import port


class GlobalVersion:
    def __init__(self, version, opaque_data):
        """
        version is the version string of the installed component,

        opaque_data must be a dictionary. It contains arbitrary opaque data that is passed
        to the plugin's activate method when this version is to be activated.
        """
        if not port.is_dict(opaque_data):
            raise ValueError("The opaque_data argument must be a dictionary.")

        self.version = version
        self.opaque_data = opaque_data


class InstallResult:
    def __init__(self, installed_version, opaque_data, self_managed):
        """ 
        installed_version is the version number that was installed. It must be an
        actual version number, even if the requested version to install 
        was a special value like 'latest'.

        opaque_data must be a dictionary. It contains arbitrary opaque data that is passed
        to the plugin's activate method when this version is to be activated.

        self_managed is a boolean that indicates if this component manages its versioning
        itself and can update itself in place. I.e. it indicates if the version of the
        installed component may change over time.
        """

        if not port.is_dict(opaque_data):
            raise ValueError("The opaque_data argument must be a dictionary.")

        self.installed_version = installed_version
        self.opaque_data = opaque_data
        self.self_managed = self_managed



class Plugin:

    def can_handle(self, component_name):        
        """ Returns True if this plugin can handle the specified
            component."""
        raise NotImplementedError()


    def detect_global_versions(self, component_name):
        """ Detects the globally installed versions for the specified component.
            Returns a list with GlobalVersion instances
        """
        raise NotImplementedError()


    def verify_valid_install_version(self, component_name, version):
        """ Verifies that the specified version string is a valid version to install
            for this component.
            If version is not valid then an appropriate Exception is thrown.
            """
        raise NotImplementedError()


    def install(self, component_name, version, version_dir, dep_man):
        """ Installs the specified version of the specified component.

            version is a version string. Some components may support the special
            value "latest" to install the latest stable version.

            version_dir is the path to a directory for this component version.
            If the component can be installed locally then it should be stored
            in this directory. Additional support files and control util.print_information
            for the installation may also be stored there.

            dep_man is a DependencyManager instance.

            You can use dep_man.ensure_installed( version_pred, install_version_string)
            to install other dependencies that this package requires.

            Returns an InstallResult instance.
            """
        raise NotImplementedError()


    def get_self_managed_version(self, component_name, opaque_data, version_dir, dep_man):
        """ Returns the current version number of a self managed component.
        """
        raise NotImplementedError()

    def update_self_managed(self, component_name, opaque_data, version_dir, dep_man):
        """ Updates a self managed component installation to the latest version.
        Returns the new version number.
        """
        raise NotImplementedError()


    def activate(self, env, component_name, version, opaque_data, version_dir, dep_man):
        """ Modifies an environment variable dictionary (env parameter) so that the specified
        component version is active for processes that use these environment variables.

        component_name is the name of the component to activate.

        version is the version number string of the version to activate. This is either a version
        that was installed with the 'install' method, or that was returned by detect_global_versions.
        For self managed installations the version is None (see InstallResult.self_managed).

        version_data is the opaque version data dictionary that was returned for this version
        by the corresponding 'install' method call or 'detect_global_versions'. It can contain
        additional control data about the version.

        version_dir is the directory containing version-specific data, if the version was installed with
        the 'install' method. version_dir is None if the version was returned by 'detect_global_versions'.

        dep_man is a DependencyManager instance.
        """
        raise NotImplementedError()



