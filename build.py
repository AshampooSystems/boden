#! /usr/bin/python

import sys, os
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), './bauer'))

from bauer import main

if __name__ == "__main__":
    exit(main(sys.argv))
