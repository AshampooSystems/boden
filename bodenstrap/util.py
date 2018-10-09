
from __future__ import print_function
import argparse
import re
import sys
import shutil
import os
import os.path
import tempfile
import distutils.versionpredicate
from distutils.version import LooseVersion, StrictVersion
import imp
import subprocess
import textwrap
import json
import types
import time
import zipfile
import random
import string
import logging

from . import port




def is_windows():
    return sys.platform=="win32"

def is_mac():
    return sys.platform=="darwin"

def is_linux():
    return sys.platform.startswith("linux")


def print_info(s):
    logging.info(s)
    

_generic_version_pattern = "\d+\.\d+(\.\d+)*[a-zA-Z]*"

def extract_version(in_text):
    """Tries to extract a version number from the specified input text.
       Returns the version string, or None if none is found."""
    m = re.search(_generic_version_pattern, in_text)
    if m is None:
        return None
    else:
        return m.group(0)



def make_generic_get_version_call(commandline, env=None, capture_stderr = False ):

    result = []

    try:
        if capture_stderr:
            output = port.call_return_output(commandline, env=env, shell=True, stderr=subprocess.STDOUT)        

        else:
            with tempfile.TemporaryFile() as stderr_temp:
                if is_windows():
                    stderr_temp_file = stderr_temp.file
                else:
                    stderr_temp_file = stderr_temp
                output = port.call_return_output(commandline, env=env, shell=True, stderr=stderr_temp_file)        
        
    except:
        output = ""

    return extract_version(output)




def download_file(url, dest_path):

    print_info("Downloading "+url)

    try:
        download = port.urlopen(url)
        with open(dest_path, "wb") as dest_file:    
            shutil.copyfileobj(download, dest_file)

    except:
        if os.path.exists(dest_path):
            os.remove(dest_path)
        raise

    print_info("Download complete")

def unzip_file(path, dest_dir, restore_unix_mode = False):
    print_info("Unpacking...")

    with zipfile.ZipFile(path, 'r') as archive:
        archive.extractall(dest_dir)

        if not is_windows() and restore_unix_mode:
            # python's zip implementation does not restore the executable
            # flag and other mode flags. Fix that.
            for item_info in archive.infolist():

                dest_path = os.path.join(dest_dir, item_info.filename)

                attr = item_info.external_attr >> 16
                if attr>0:
                    os.chmod( dest_path, attr)
                
        archive.close()

    print_info("Done unpacking")


def untargz_file(path, dest_dir):
    print_info("Unpacking...")
    import tarfile
    with tarfile.open(path, 'r') as archive:
        archive.extractall(dest_dir)
    print_info("Done unpacking")



class TempDir:
    def __init__(self):
        self.path = tempfile.mkdtemp()

    def sub_path(self, name):
        return os.path.join(self.path, name)

    def __del__(self):
        shutil.rmtree(self.path)


def get_version_ints(ver_string):

    num_list=[]
    for comp in ver_string.split("."):
        if comp:
            m = re.match("\\d+", comp)
            if m is not None:
                num_list.append( int( m.group(0) ) )

    return num_list


def find_subdir_name_with_item(parent_dir, item_path_components):
    for subdir_name in os.listdir(parent_dir):
        item_path = os.path.join(parent_dir, subdir_name, *item_path_components)
        if os.path.exists(item_path):
            return subdir_name


    return None


