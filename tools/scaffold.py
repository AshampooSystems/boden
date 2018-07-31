#!/usr/bin/env python

from jinja2 import Environment, FileSystemLoader
from argparse import ArgumentParser
import os
import inflection


native_control_default = {
	"mac": "<<NSView?>>",
	"gtk": "<<GtkWidget?>>",
	"win32": "<<?>>",
	"winuwp": "<<?>>",
	"ios": "<<UIView?>>",
	"android": "<<?>>",
	"webems": "<<div?>>"
}


def allowedPlatforms():
	return ["mac","gtk","win32","winuwp","ios","android","webems"]

def stripJ2Ext(filename):
	ext = os.path.splitext(filename)[1]
	if ext == '.j2':
		return filename[:len(filename)-len(ext)]
	return filename

def parseFilename(filename):
	untemplated   = stripJ2Ext(filename)
	title         = os.path.splitext(untemplated)[0]
	ext           = os.path.splitext(untemplated)[1]
	return (untemplated, title, ext)

def relTemplatePath(context, root):
	return os.path.relpath(root, context.template_path)


class Core:
	def isPlatformSpecific(self):
		return True

	def detectPlatform(self, path):
		path = os.path.normpath(path)
		components = path.split(os.path.sep)
		for component in components:
			if component in allowedPlatforms():
				return component;
		return None

	def supportedFileTitles(self):
		return ["core", "jcontrol"]
 
	def filename(self, context, file, type, platform):
		(untemplated, title, ext) = parseFilename(file)
		if title == "core":
			return context.args.title[0] + inflection.camelize(type) + ext
		elif title == "jcontrol": # Android supplemental class
			return "J" + inflection.camelize(context.args.nativename) + ext

	def generate(self, context, target_filepath, root, file, platform):
		if context.args.nativename != None:
			native_control_name = context.args.nativename
		else:
			native_control_name = native_control_default[platform]

		template = context.env.get_template(os.path.join(relTemplatePath(context, root), file))
		with open(target_filepath, 'wb') as f:
			f.write(template.render(
				name=context.args.title[0],
				varName=inflection.camelize(context.args.title[0], False),
				nativeControlName=native_control_name))

class CoreTest:
	def isPlatformSpecific(self):
		return True

	def detectPlatform(self, path):
		path = os.path.normpath(path)
		components = path.split(os.path.sep)
		for component in components:
			if component in allowedPlatforms():
				return component;
		return None

	def supportedFileTitles(self):
		return ["core_test"]

	def filename(self, context, file, type, platform):
		(untemplated, title, ext) = parseFilename(file)
		camelizedType = inflection.camelize(type)
		return "test" + context.args.title[0] + "Core_" + platform + ext

	def generate(self, context, target_filepath, root, file, platform):
		if context.args.nativename != None:
			native_control_name = context.args.nativename
		else:
			native_control_name = native_control_default[platform]

		template = context.env.get_template(os.path.join(relTemplatePath(context, root), file))
		with open(target_filepath, 'wb') as f:
			f.write(template.render(
				name=context.args.title[0],
				varName=inflection.camelize(context.args.title[0], False),
				nativeControlName=native_control_name))

class TestCore:
	def isPlatformSpecific(self):
		return False

	def detectPlatform(self, path):
		return None

	def supportedFileTitles(self):
		return ["test_core"]

	def filename(self, context, file, type, platform):
		(untemplated, title, ext) = parseFilename(file)
		camelizedType = inflection.camelize(type)
		return "Test" + context.args.title[0] + "Core" + ext

	def generate(self, context, target_filepath, root, file, platform):
		if context.args.nativename != None:
			native_control_name = context.args.nativename
		else:
			native_control_name = native_control_default[platform]

		template = context.env.get_template(os.path.join(relTemplatePath(context, root), file))
		with open(target_filepath, 'wb') as f:
			f.write(template.render(
				name=context.args.title[0],
				varName=inflection.camelize(context.args.title[0], False),
				nativeControlName=native_control_name))

class Control:
	def isPlatformSpecific(self):
		return False

	def detectPlatform(self, path):
		return None

	def supportedFileTitles(self):
		return ["control"]

	def filename(self, context, file, type, platform):
		(untemplated, title, ext) = parseFilename(file)
		camelizedType = inflection.camelize(type)
		return context.args.title[0] + ext

	def generate(self, context, target_filepath, root, file, platform):
		template = context.env.get_template(os.path.join(relTemplatePath(context, root), file))
		with open(target_filepath, 'wb') as f:
			f.write(template.render(name=context.args.title[0]))

class ControlTest:
	def isPlatformSpecific(self):
		return False

	def detectPlatform(self, path):
		return None

	def supportedFileTitles(self):
		return ["control_mock", "control_test"]

	def filename(self, context, file, type, platform):
		(untemplated, title, ext) = parseFilename(file)
		camelizedType = inflection.camelize(type)
		if title == "control_mock":
			return "Mock" + context.args.title[0] + "Core" + ext
		elif title == "control_test":
			return "Test" + context.args.title[0] + ext

	def generate(self, context, target_filepath, root, file, platform):
		template = context.env.get_template(os.path.join(relTemplatePath(context, root), file))
		with open(target_filepath, 'wb') as f:
			f.write(template.render(name=context.args.title[0]))

class CoreInterface:
	def isPlatformSpecific(self):
		return False

	def detectPlatform(self, path):
		return None

	def supportedFileTitles(self):
		return ["core_interface"]

	def filename(self, context, file, type, platform):
		(untemplated, title, ext) = parseFilename(file)
		camelizedType = inflection.camelize(type)
		return "I" + context.args.title[0] + "Core" + ext

	def generate(self, context, target_filepath, root, file, platform):
		template = context.env.get_template(os.path.join(relTemplatePath(context, root), file))
		with open(target_filepath, 'wb') as f:
			f.write(template.render(name=context.args.title[0]))

class Context:
	def __init__(self, boden_path, template_path, env, args):
		self.boden_path = boden_path
		self.template_path = template_path
		self.env = env
		self.args = args

		self.types = []
		self.generator = {}

		if args.core == True:
			self.types.append("core")
			self.generator["core"] = Core()

		if args.control == True:
			self.types.append("control")
			self.generator["control"] = Control()

		if args.core_interface == True:
			self.types.append("core_interface")
			self.generator["core_interface"] = CoreInterface()

		if args.generate_tests == True:
			if args.control_test == True:
				self.types.append("control_test")
				self.generator["control_test"] = ControlTest()
			if args.core_test == True:
				self.types.append("test_core")
				self.types.append("core_test")
				self.generator["test_core"] = TestCore()
				self.generator["core_test"] = CoreTest()

		if args.platform == "all":
			self.platforms = allowedPlatforms();
		else:
			if args.platform in allowedPlatforms():
				self.platforms = [args.platform]
			else:
				raise Exception("Invalid platform")


def generateScaffold(context):
	for type in context.types:
		path = os.path.join(context.template_path, type);
		for root, subdirs, files in os.walk(path):
			rel_path = os.path.relpath(root, path)
			target_path = os.path.join(context.boden_path, rel_path)

			platform = context.generator[type].detectPlatform(rel_path)

			if not context.generator[type].isPlatformSpecific() or platform == context.args.platform:
				for file in files:
					(untemplated, title, ext) = parseFilename(file)
					if title in context.generator[type].supportedFileTitles():
						target_filepath = os.path.join(target_path, context.generator[type].filename(context, file, type, platform))
						print("[" + type + "] " + target_filepath)
						if not context.args.dryrun:
							if not os.path.isfile(target_filepath) or context.args.force == True:
								context.generator[type].generate(context, target_filepath, root, file, platform);
							else:
								print("File exists and won't be overwritten (specify --force to override)")


parser = ArgumentParser()
parser.add_argument("title", metavar="Title", type=str, nargs=1,
	                help = "Title of the item to be scaffolded")
parser.add_argument("-p", "--platform", dest="platform", default="all",
	                help="Platform (all,android,gtk,ios,mac,webems,win32,winuwp), defaults to all")
parser.add_argument("-n", "--native-name", dest="nativename", type=str, default=None,
	                help="Specify native control name (default=insert placeholder)")
parser.add_argument("-d", "--dry-run", action="store_true", dest="dryrun", default=False,
	                help="Do not write files (default=false)")
parser.add_argument("-t", "--generate-tests", action="store_true", dest="generate_tests", default=False,
	                help="Generate tests (control test + core test)")
parser.add_argument("--no-core", action="store_false", dest="core", default=True,
	                help="Do not create core")
parser.add_argument("--no-control", action="store_false", dest="control", default=True,
	                help="Do not create control")
parser.add_argument("--no-core-interface", action="store_false", dest="core_interface", default=True,
	                help="Do not create core interface")
parser.add_argument("--no-control-test", action="store_false", dest="control_test", default=True,
	                help="Do not create control test and control mock (only applies if -t is specified)")
parser.add_argument("--no-core-test", action="store_false", dest="core_test", default=True,
	                help="Do not create test for platform core (only applies if -t is specified)")
parser.add_argument("--force", action="store_true", dest="force", default=False,
	                help="Force overwriting files")

args = parser.parse_args()

script_path   = os.path.dirname(os.path.realpath(__file__))
template_path = os.path.join(script_path, "templates");
boden_path    = os.path.dirname(os.path.abspath(script_path))

print("Boden path:    " + boden_path)
print("Template path: " + template_path)

env = Environment(loader=FileSystemLoader(template_path))

generateScaffold(Context(boden_path, template_path, env, args))
