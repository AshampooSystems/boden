"""Generate .xcassets"""

import os
import json
import sys
import shutil

class AssetType:
    Image = 1
    Data = 2
    Icon = 3

class AssetSet:
    def __init__(self, path, name, type):
        self.ext = None
        self.contents_section = None
        if type == AssetType.Image:
            self.ext = "imageset"
            self.content_section_name = "images"

        if type == AssetType.Data:
            self.ext = "dataset"
            self.content_section_name = "data"

        if type == AssetType.Icon:
            self.ext = "appiconset"
            self.content_section_name = "images"

        self.path = os.path.join(path, "%s.%s" % (name, self.ext))

        self.contents = {
            "info" : {
                "version" : 1,
                "author" : "xcode"
            }
        }

        self.files = []
    
    def write(self, copy_file = True):
        if not os.path.exists(self.path):
            os.makedirs(self.path)

        descriptors = []

        for file in self.files:
            src = file["path"]
            filename = os.path.basename(src)
            dest = os.path.join(self.path, filename)
            if copy_file:
                shutil.copyfile(src, dest)

            data = file["description"]
            data["filename"] = filename
            descriptors.append(data)

        self.contents[self.content_section_name] = descriptors

        contents_file = open(os.path.join(self.path, "Contents.json"), "w")
        contents_file.write(json.dumps(self.contents, sort_keys=True, indent=2))

    def append_file(self, path, description):
        self.files.append({"path" : path, "description" : description})

class AssetBuilder:
    def __init__(self, path):
        self.path = path
        self.sets = []
        self.contents = {
            "info" : {
                "version" : 1,
                "author" : "xcode"
            }
        }

    def write(self, copy_files=True):
        if not os.path.exists(self.path):
            os.makedirs(self.path)
        content_file = open(os.path.join(self.path, "Contents.json"), "w")
        content_file.write(json.dumps(self.contents, sort_keys=True, indent=2))

        for set in self.sets:
             set.write(copy_files)


    def create_set(self, name, type):
        new_set = AssetSet(self.path, name, type)
        self.sets.append(new_set)
        return new_set


def main(args):
    asset_builder = AssetBuilder(args[0])

    test_set = asset_builder.create_set("Image", AssetType.Image)
    test_set.append_file("./tmp/icon-16.png", {"idiom" : "universal", "scale" : "1x"})
    test_set.append_file("./tmp/icon-32.png", {"idiom" : "universal", "scale" : "2x"})

    asset_builder.write()

if __name__ == "__main__":
    main(sys.argv[1:])