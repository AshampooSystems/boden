
import imp

from . import port
from . import util
import bodenstrap.plugins
from bodenstrap.plugins import *

class PluginManager:

    _plugin_counter = 0

    def __init__(self):
        self.plugin_list = []


    def add_plugins_from_module(self, plugin_module):
        for plugin in plugin_module.get_plugins():
            self.plugin_list.insert(0, plugin )


    def add_builtin_plugins(self):
        for name in sorted(bodenstrap.plugins.__all__):
            plugin_module = getattr(bodenstrap.plugins, name)
            self.add_plugins_from_module(plugin_module)
            


    def load_plugins_from_module(self, module_path):
        name = "_boden_dep_manager_plugin_%d_" % PluginManager._plugin_counter
        PluginManager._plugin_counter += 1

        with open(path, "r") as f:
            plugin_module = imp.load_module( name, f, module_path, "")

        self.add_plugins_from_module(plugin_module)


    def get_plugin(self, component_name):
        for plugin in self.plugin_list:
            if plugin.can_handle(component_name):
                return plugin

        raise Exception("No plugin found for component '%s'" % (component_name))




