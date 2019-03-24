#!/usr/bin/env python

import json
import argparse
import sys, os
import logging

import apple, android

def setup_logging(argv):
	FORMAT = '%(asctime)-15s %(message)s'
	if '-v' in argv or '--verbose' in argv:
		logging.basicConfig(stream=sys.stderr, level=logging.DEBUG, format=FORMAT)    
	else:
		logging.basicConfig(stream=sys.stderr, level=logging.INFO, format=FORMAT)    

def parse_arguments(argv):
	parser = argparse.ArgumentParser(description='Process resource.json files into platform specific format')

	subparsers = parser.add_subparsers(title='Action', dest='action', help='The action to take')

	build = subparsers.add_parser('build', help='Build the resource file')
	dependencies = subparsers.add_parser('dependencies', help='Output the inputfiles in the resource file')

	for p in [build,dependencies]:
		p.add_argument('input_file', help='The resource.json input file')
		p.add_argument('-v', '--verbose', action='store_true' ,help="Enable verbose output")
		p.add_argument('-p', '--platform', default='ios', choices=['ios', 'android', 'mac'])	
		p.add_argument('output_directory', help='Where to put the result')	

	return parser.parse_args(argv)

def main(argv):
	setup_logging(argv)
	args = parse_arguments(argv)
	logging.debug("Arguments: %s" %(args))

	if not args:
		return

	platform_roger = None

	if args.platform == 'ios':
		platform_roger = apple.Roger()
	if args.platform == 'mac':
		platform_roger = apple.Roger()
	if args.platform == 'android':
		platform_roger = android.Roger()

	platform_roger.execute(args)



if __name__ == "__main__":
	main(sys.argv[1:])


