#/usr/bin/env python3

import os

with open('docs/CNAME','w') as f:
	f.write("www.boden.io")

os.remove('docs/robots.txt')