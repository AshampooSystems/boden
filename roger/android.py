import logging
import os
import sys
import shutil

from resource_file import ResourceFile
import base

class Roger(base.Roger):
    def __init__(self):
        base.Roger.__init__(self)

    def execute(self, args):
        self.resource_file.parse_input(args)

        #if args.action == 'build':
        self.build(args)
        #if args.action == 'dependencies':
        #    self.dependencies(args)

    def copy_file(self, args, folder_name, src, bundle_path, suffix, flatten=True):
        src = self.full_path(src)

        if flatten:
            dest_path = bundle_path.replace('/', '_')
            dest_name, dest_suffix = os.path.splitext(dest_path)
            dest_path = dest_path.replace('.', '_') + dest_suffix
        else:
            dest_path = bundle_path

        dest = os.path.join(args.output_directory, folder_name + suffix, dest_path)
        self.copy(args, src, dest)

    def build(self, args):
        logging.debug("Building ...");

        if "resources" in self.resource_file.data:
            resource_lists = self.resource_file.data["resources"]

            for resources in resource_lists:
                lang_suffix = ""
                if "language" in resources:
                    if len(resources["language"]) > 0:
                        lang_suffix = "-%s" % (resources["language"])

                images = resources["images"]

                for image in images:
                    bundle_path = image["bundle-path"]
                    resolutions = image['resolutions'];

                    self.copy_image(args, "res/drawable", bundle_path, resolutions, "0.75x", lang_suffix + "-ldpi")
                    self.copy_image(args, "res/drawable", bundle_path, resolutions, "1.0x",  lang_suffix + "-mdpi")
                    self.copy_image(args, "res/drawable", bundle_path, resolutions, "1.5x",  lang_suffix + "-hdpi")
                    self.copy_image(args, "res/drawable", bundle_path, resolutions, "2.0x",  lang_suffix + "-xhdpi")
                    self.copy_image(args, "res/drawable", bundle_path, resolutions, "3.0x",  lang_suffix + "-xxhdpi")
                    self.copy_image(args, "res/drawable", bundle_path, resolutions, "4.0x",  lang_suffix + "-xxxhdpi")

                raws = resources["raw"]
                for raw in raws:
                    bundle_path = raw["bundle-path"]
                    filename = raw["file"]

                    self.copy_file(args, "res/raw", filename, bundle_path, lang_suffix)


        if "assets" in self.resource_file.data:
            asset_list = self.resource_file.data["assets"]
            for asset in asset_list:
                bundle_path = asset["bundle-path"]
                filename = asset["file"]
                if "language" in asset:
                    raise '"lanuage" is not supported for assets'

                self.copy_file(args, "assets", filename, bundle_path, "", False)

        if "icon" in self.resource_file.data:
            icon = self.resource_file.data["icon"]
            if "android" in icon:
                android_icon = icon["android"]

                self.copy_image(args, "res/mipmap", "ic_launcher.png", android_icon, "48x48", "-mdpi")
                self.copy_image(args, "res/mipmap", "ic_launcher.png", android_icon, "72x72", "-hdpi")
                self.copy_image(args, "res/mipmap", "ic_launcher.png", android_icon, "96x96", "-xhdpi")
                self.copy_image(args, "res/mipmap", "ic_launcher.png", android_icon, "144x144", "-xxhdpi")
                self.copy_image(args, "res/mipmap", "ic_launcher.png", android_icon, "192x192", "-xxxhdpi")






