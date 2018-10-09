
import argparse
import subprocess
import textwrap
import logging

from . import port
from . import util
from . import pluginman
from . import dependency

def prepare_help_text(help_text, line_length=80):

    out_list = []
    for l in help_text.splitlines():

        if len(l)>line_length:
            out_list.extend( textwrap.wrap(l, line_length) )
        else:
            out_list.append(l)

    return "\n".join(out_list)

_help_text_raw = """\
Automatically downloads and installs dependencies and then executes a command with these dependencies active.

Boden Strap will install missing dependencies in an isolated, localized repository. The dependencies do not influence other programs - they are only active for the command that is executed through Boden Strap.

-- Dependencies --

Each DEPENDENCY passed in a --with option must have the following format:
COMPONENTNAME(VERCONDITIONS)[INSTALLVER]

COMPONENTNAME is the name of the component to install. Note that components must be explicitly supported (either builtin or by a plugin).

VERCONDITIONS defines which component versions are acceptable. VERCONDITIONS and the surrounding round brackets can also be left out if all versions are acceptable. See examples.

INSTALLVER the version to download and install if no suitable version was found.

Examples:

python -m bodenstrap --with "cmake[3.11]" 
accepts all CMake versions and installs v3.11 if CMake is not found at all

python -m bodenstrap --with "cmake(3.11)[3.11]" 
accepts only CMake 3.11 and installs v3.11 if it is not found

python -m bodenstrap --with "cmake(>=3.11)[3.12]" 
accepts CMake 3.11 or higher and installs v3.12 if none is found.

python -m bodenstrap --with "cmake(>=3.4, <3.12, !=3.6)[3.11]"
accepts any CMake version bigger than v3.4 and smaller than 3.12, but not version 3.6.
If no suitable version is found then version 3.11 is installed.

"""


def do_main(arg_list):

    logging.basicConfig( level=logging.INFO, format='> %(message)s' )

    parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter, description=prepare_help_text(_help_text_raw) )
    parser.add_argument('--with', action='append', dest="with_list", metavar="DEPENDENCY", help="""\
Specifies one dependency to install. You can pass multiple --with parameters. See details above for the format of DEPENDENCY.
""")
    parser.add_argument('--with-file', action='append', dest="with_file_list", metavar="DEPFILE", help="""\
Specifies a dependency file. Each non-empty line that does not start with a # must be a dependency (same format as --with parameter). You can pass multiple --with-file parameters.
""")
    #parser.add_argument('--plugin', action='append', dest="plugin_module_list", metavar="PLUGINMODULE", help="Optional plugin module to load. You can pass multiple --plugin parameters.")
    parser.add_argument('--repo', metavar='REPO', help="The directory to install downloaded dependencies to. If this is omitted then the path is taken from the environment variable BODEN_STRAP_REPO. If that is also missing then a default directory is used (in AppData on Windows, otherwise in the user's home directory)")
    parser.add_argument('command', nargs=argparse.REMAINDER, metavar='COMMAND', help="The remainder of the commandline after the options is the command to execute. The dependencies specified via the other options will be active for this command.")

    args = parser.parse_args(arg_list)

    plugin_manager = pluginman.PluginManager()

    plugin_manager.add_builtin_plugins()

    #if args.plugin_module_list:
    #    for module_path in args.plugin_module_list:
    #        plugin_manager.load_plugins_from_module(module_path)

    dep_string_list = []
    if args.with_list:
        dep_string_list.extend( args.with_list )

    if args.with_file_list:
        for dep_info_file in args.with_file_list:
            with open(dep_info_file, "rb") as f:
                data = f.read().decode("utf-8")
                for line in data.splitlines():
                    line = line.strip()
                    if line and not line.startswith("#"):
                        dep_string_list.append(line)


    # parse all dependency infos to ensure that they are valid
    dep_info_list = []
    for dep_string in dep_string_list:
        dep_info_list.append( dependency.DependencyInfo(dep_string) )


    dep_man = dependency.DependencyManager( args.repo, plugin_manager )

    command_env = dep_man.prepare_environment(dep_info_list)

    if args.command:

        util.print_info("Executing command...")

        if util.is_windows():
            return subprocess.call( args.command, shell=True, env=command_env )        

        else:
            # on non-windows systems we cannot pass the list to subprocess.call.
            # The shell will not handle it correctly.
            # So we create a single string and add the individual commands.

            commandline = ""
            for c in args.command:
                if commandline:
                    commandline += " "
                commandline += port.unix_shell_quote(c)

            return subprocess.call( commandline, shell=True, env=command_env )            
        
    else:
        util.print_info("No command specified. Not executing anything.")

    return 0






