
from __future__ import print_function
import sys
import os
import subprocess
import types

def is_python2():
    return sys.version_info[0] == 2


def get_work_dir():
    if is_python2():
        return os.getcwdu()
    else:
        return os.getcwd()


def urlopen(*params, **kwargs):
    if is_python2():
        import urllib2
        return urllib2.urlopen(*params, **kwargs)
    else:
        import urllib.request
        return urllib.request.urlopen(*params, **kwargs)

def call_return_output(*params, **kwargs):
    output = subprocess.check_output(*params, **kwargs)
    if not is_python2():
        output = output.decode("utf-8", "replace")

    return output


def unix_shell_quote(s):
    # shlex.quote exists in python >= 3.3
    try:
        from shlex import quote
        return quote(s)
    
    except ImportError:

        # escape ourselves.
        result = ""

        escape_map = {}        
        for c in '"\'&\\<>| ':
            escape_map[c] = '\\'+c

        for c in s:
            escaped = escape_map.get(c)
            if escaped is not None:
                result += escaped
            else:
                result += c

        return result


def is_dict(o):
    if is_python2():
        return isinstance( o, types.DictType )
    else:
        return isinstance( o, dict )



def print_stderr(s):
    print( s, file=sys.stderr )



