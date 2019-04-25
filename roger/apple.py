import logging
import os
import sys
import shutil
import subprocess
import tempfile

from resource_file import ResourceFile

class Roger:
	def __init__(self):
		self.resource_file = ResourceFile()


	def execute(self, args):
		self.resource_file.parse_input(args)

		#if args.action == 'build':
		self.build(args)
		#if args.action == 'dependencies':
		#	self.dependencies(args)

	def full_path(self, path):
		if os.path.isabs(path):
			return path
		return os.path.normpath(os.path.join(self.resource_file.root_path, path))

	def copy(self, args, src, dest):
		folder = os.path.dirname(dest)
		src = self.full_path(src)
		
		if args.action == 'build':
			logging.debug("Copying %s => %s" % ( src, dest ))
			if not os.path.exists(folder):
				os.makedirs(folder)
			shutil.copyfile(src, dest)
		else:
			sys.stdout.write(src)
			sys.stdout.write("\\;")
			sys.stdout.write(dest)
			sys.stdout.write(";")

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

	def destination_path(self, args, src, bundle_path, lang_folder_name, suffix, real_ext = None):
		src = self.full_path(src)

		dest_name, dest_ext = os.path.splitext(bundle_path)
		dest_name += suffix
		dest_name = "".join([dest_name, dest_ext])

		if real_ext:
			dest_name += real_ext;

		dest = os.path.join(args.output_directory, lang_folder_name, dest_name)
		return dest

	def copy_file(self, args, lang_folder_name, src, bundle_path, suffix, real_ext = None, flatten_mac = True):
		dest = ''
		if args.platform == 'ios' or flatten_mac == False:
			dest = self.destination_path(args, src, bundle_path, lang_folder_name, suffix, real_ext)
		elif args.platform == 'mac':
			dest = self.destination_path_flattened(args, src, bundle_path, lang_folder_name, suffix, real_ext)

		self.copy(args, src, dest)

	def copy_image(self, args, lang_folder_name, bundle_path, resolutions, resolutionId, suffix):
		if resolutionId in resolutions:
			if len(resolutions[resolutionId]) > 0:		
				self.copy_file(args, lang_folder_name, resolutions[resolutionId], bundle_path, suffix)

	def combine_images_mac(self, args, resolutions, lang_folder_name, bundle_path):
		if "1.0x" in resolutions and "2.0x" in resolutions:
			if args.action == 'build':
				handle, temp_file_name = tempfile.mkstemp()
				fp = os.fdopen(handle,'w')
				fp.close()

				output = subprocess.check_output(
					[
						"tiffutil", "-cathidpicheck", 
						self.full_path(resolutions["1.0x"]), 
						self.full_path(resolutions["2.0x"]),
						"-out", temp_file_name
					], stderr=subprocess.STDOUT)

				self.copy_file(args, lang_folder_name, temp_file_name, bundle_path, "", ".tiff")
				os.remove(temp_file_name)
			elif args.action == 'dependencies':
				self.copy_file(args, lang_folder_name, resolutions["1.0x"], bundle_path, "", ".tiff")
				self.copy_file(args, lang_folder_name, resolutions["2.0x"], bundle_path, "", ".tiff")
		else:
			self.copy_image(args, lang_folder_name, bundle_path, resolutions, "1.0x", "")
			self.copy_image(args, lang_folder_name, bundle_path, resolutions, "2.0x", "@2x")



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
					bundle_path = image['bundle_path']
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
					bundle_path = raw["bundle_path"]
					filename = raw["file"]

					self.copy_file(args, lang_folder_name, filename, bundle_path, "", flatten_mac=False)

		if "assets" in self.resource_file.data:
			asset_list = self.resource_file.data["assets"]
			for asset in asset_list:
				bundle_path = asset["bundle_path"]
				filename = asset["file"]
				if "language" in asset:
					raise '"lanuage" is not supported for assets'

				self.copy_file(args, "Base.lproj/assets", filename, bundle_path, "", flatten_mac=False)


