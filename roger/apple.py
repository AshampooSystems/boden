import logging
import os
import sys
import shutil
import subprocess
import tempfile
import copy

import base
from assetbuilder import AssetBuilder, AssetSet, AssetType

class Roger(base.Roger):
    def __init__(self):
        base.Roger.__init__(self)


    def execute(self, args):
        self.resource_file.parse_input(args)

        #if args.action == 'build':
        self.build(args)
        #if args.action == 'dependencies':
        #    self.dependencies(args)

    def destination_path(self, args, src, bundle_path, lang_folder_name, suffix, real_ext = None):
        src = self.full_path(src)

        dest_name, dest_ext = os.path.splitext(bundle_path)
        dest_name += suffix
        dest_name = "".join([dest_name, dest_ext])

        if real_ext:
            dest_name += real_ext;

        dest = os.path.join(args.output_directory, lang_folder_name, dest_name)
        return dest

    def destination_path_flattened(self, args, src, bundle_path, lang_folder_name, suffix, real_ext = None):
        src = self.full_path(src)

        dest_name, dest_ext  = os.path.splitext(bundle_path)
        dest_name = dest_name.replace('/', '_')
        dest_name += "_" + dest_ext.lstrip('.')
        dest_name += suffix
        if real_ext != None:
            dest_ext = real_ext
        dest_name = "".join([dest_name, dest_ext])

        dest = os.path.join(args.output_directory, lang_folder_name, dest_name)
        return dest

    def copy_file(self, args, lang_folder_name, src, bundle_path, suffix, real_ext = None, flatten_mac = True):
        dest = ''
        if args.platform == 'ios' or flatten_mac == False:
            dest = self.destination_path(args, src, bundle_path, lang_folder_name, suffix, real_ext)
        elif args.platform == 'mac':
            dest = self.destination_path_flattened(args, src, bundle_path, lang_folder_name, suffix, real_ext)
        return self.copy(args, src, dest)

    def copy_image(self, args, lang_folder_name, bundle_path, resolutions, resolutionId, suffix, flatten_mac = True):
        if resolutionId in resolutions:
            if len(resolutions[resolutionId]) > 0:
                source_image = self.get_source_image(args, resolutions[resolutionId])
                return self.copy_file(args, lang_folder_name, source_image, bundle_path, suffix, None, flatten_mac)
        return False

    def combine_images_mac(self, args, resolutions, lang_folder_name, bundle_path):
        if "1.0x" in resolutions and "2.0x" in resolutions:
            if args.action == 'build':
                handle, temp_file_name = tempfile.mkstemp()
                fp = os.fdopen(handle,'w')
                fp.close()

                output = subprocess.check_output(
                    [
                        "tiffutil", "-cathidpicheck",
                        self.full_path(self.get_source_image(args, resolutions["1.0x"])),
                        self.full_path(self.get_source_image(args, resolutions["2.0x"])),
                        "-out", temp_file_name
                    ], stderr=subprocess.STDOUT)

                self.copy_file(args, lang_folder_name, temp_file_name, bundle_path, "", ".tiff")
                os.remove(temp_file_name)
            elif args.action == 'dependencies':
                self.copy_file(args, lang_folder_name, self.get_source_image(args, resolutions["1.0x"]), bundle_path, "", ".tiff")
                self.copy_file(args, lang_folder_name, self.get_source_image(args, resolutions["2.0x"]), bundle_path, "", ".tiff")
        else:
            self.copy_image(args, lang_folder_name, bundle_path, resolutions, self.get_source_image(args, resolutions["1.0x"]), "")
            self.copy_image(args, lang_folder_name, bundle_path, resolutions, self.get_source_image(args, resolutions["2.0x"]), "@2x")


    def add_icon(self, args, resolutions, asset_set, idiom, size, scale):
        build_args = copy.deepcopy(args)
        build_args.action = "build"
        dest = os.path.relpath(asset_set.path, args.output_directory)

        if self.copy_image(build_args, dest, "%s-icon-%s.png" % (idiom, size), resolutions, "%s@%s" % (size,scale), "@%s" % (scale), flatten_mac=False):
            asset_set.append_file("%s-icon-%s@%s.png" % (idiom, size, scale), { "idiom" : idiom, "size" : size, "scale" : scale})


    def build(self, args):
        logging.debug("Building ...");

        if not os.path.exists(args.output_directory):
            os.makedirs(args.output_directory)

        if args.action == 'build':
            open(os.path.join(args.output_directory, 'resource-timestamp'), 'w')

        if "resources" in self.resource_file.data:
            resource_lists = self.resource_file.data["resources"]

            for resources in resource_lists:
                lang_folder_name = "Base.lproj"
                if "language" in resources:
                    if len(resources["language"]) > 0:
                        lang_folder_name = resources["language"] + ".lproj"

                images = resources["images"]

                for image in images:
                    bundle_path = image["bundle-path"]
                    resolutions = image['resolutions']
                    mac_combine = args.platform == 'mac'
                    if args.platform == 'mac':
                        if "mac_combine" in image:
                            if image['mac_combine']:
                                mac_combine = image['mac_combine']


                    if mac_combine:
                        self.combine_images_mac(args, resolutions, lang_folder_name, bundle_path)
                        self.copy_image(args, lang_folder_name, bundle_path, resolutions, "3.0x", "@3x")
                        self.copy_image(args, lang_folder_name, bundle_path, resolutions, "4.0x", "@4x")
                    else:
                        self.copy_image(args, lang_folder_name, bundle_path, resolutions, "1.0x", "")
                        self.copy_image(args, lang_folder_name, bundle_path, resolutions, "2.0x", "@2x")
                        self.copy_image(args, lang_folder_name, bundle_path, resolutions, "3.0x", "@3x")
                        self.copy_image(args, lang_folder_name, bundle_path, resolutions, "4.0x", "@4x")

                raws = resources["raw"]
                for raw in raws:
                    bundle_path = raw["bundle-path"]
                    filename = raw["file"]

                    self.copy_file(args, lang_folder_name, filename, bundle_path, "", flatten_mac=False)

        if "assets" in self.resource_file.data:
            asset_list = self.resource_file.data["assets"]
            for asset in asset_list:
                bundle_path = asset["bundle-path"]
                filename = asset["file"]
                if "language" in asset:
                    raise '"lanuage" is not supported for assets'
                self.copy_file(args, "Base.lproj/assets", filename, bundle_path, "", flatten_mac=False)

        if "icon" in self.resource_file.data:
            icon = self.resource_file.data["icon"]

            asset_builder = AssetBuilder(os.path.join(args.output_directory, "icon.xcassets"))

            icon_set = asset_builder.create_set("ApplicationIcon", AssetType.Icon)

            if args.platform == "mac":
                if "mac" in icon:
                    self.add_icon(args, icon["mac"], icon_set, "mac", "16x16", "1x")
                    self.add_icon(args, icon["mac"], icon_set, "mac", "16x16", "2x")
                    self.add_icon(args, icon["mac"], icon_set, "mac", "32x32", "1x")
                    self.add_icon(args, icon["mac"], icon_set, "mac", "32x32", "2x")
                    self.add_icon(args, icon["mac"], icon_set, "mac", "128x128", "1x")
                    self.add_icon(args, icon["mac"], icon_set, "mac", "128x128", "2x")
                    self.add_icon(args, icon["mac"], icon_set, "mac", "256x256", "1x")
                    self.add_icon(args, icon["mac"], icon_set, "mac", "256x256", "2x")
                    self.add_icon(args, icon["mac"], icon_set, "mac", "512x512", "1x")
                    self.add_icon(args, icon["mac"], icon_set, "mac", "512x512", "2x")
            else:
                if "iphone" in icon:
                    self.add_icon(args, icon["iphone"], icon_set, "iphone", "20x20", "2x")
                    self.add_icon(args, icon["iphone"], icon_set, "iphone", "20x20", "3x")
                    self.add_icon(args, icon["iphone"], icon_set, "iphone", "29x29", "2x")
                    self.add_icon(args, icon["iphone"], icon_set, "iphone", "29x29", "3x")
                    self.add_icon(args, icon["iphone"], icon_set, "iphone", "40x40", "2x")
                    self.add_icon(args, icon["iphone"], icon_set, "iphone", "40x40", "3x")
                    self.add_icon(args, icon["iphone"], icon_set, "iphone", "60x60", "2x")
                    self.add_icon(args, icon["iphone"], icon_set, "iphone", "60x60", "3x")

                if "ipad" in icon:
                    self.add_icon(args, icon["ipad"], icon_set, "ipad", "20x20", "1x")
                    self.add_icon(args, icon["ipad"], icon_set, "ipad", "20x20", "2x")
                    self.add_icon(args, icon["ipad"], icon_set, "ipad", "29x29", "1x")
                    self.add_icon(args, icon["ipad"], icon_set, "ipad", "29x29", "2x")
                    self.add_icon(args, icon["ipad"], icon_set, "ipad", "40x40", "1x")
                    self.add_icon(args, icon["ipad"], icon_set, "ipad", "40x40", "2x")
                    self.add_icon(args, icon["ipad"], icon_set, "ipad", "76x76", "1x")
                    self.add_icon(args, icon["ipad"], icon_set, "ipad", "76x76", "2x")
                    self.add_icon(args, icon["ipad"], icon_set, "ipad", "83.5x83.5", "2x")

                if "ios-marketing" in icon:
                    self.add_icon(args, icon["ios-marketing"], icon_set, "ios-marketing", "1024x1024", "1x")


            if args.action == "dependencies":
                sys.stdout.write(asset_builder.path)
                sys.stdout.write("\\;")
                sys.stdout.write(asset_builder.path)
                sys.stdout.write(";")


            asset_builder.write(copy_files=False)
