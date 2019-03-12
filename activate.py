#!/usr/bin/env python

'''
Activates a given file, ie links modules/<file> to src/<file> and removes existing link

https://stackoverflow.com/questions/26727314/multiple-files-for-one-argument-in-argparse-python-2-7
https://stackoverflow.com/questions/23766689/python-argparse-arg-with-no-flag
'''

import os, re, argparse
from serial.tools import list_ports

parser = argparse.ArgumentParser(description='Activate given files', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('files', metavar='file', nargs='*', help='Files to activate')
# parser.add_argument('files', metavar='file', type=argparse.FileType('r'), nargs='*', help='Files to activate')

args = parser.parse_args()

# be sure to be in dir of script
os.chdir(os.path.dirname(os.path.realpath(__file__)))

# unlink any existing file
os.system('find src/ -type l -delete')

# always link common functions
if os.path.isdir('modules/common'):
    for f in os.listdir('modules/common/'):
        os.system('ln -s ../modules/common/{f} src/'.format(f=f))

# if dir given, link every file from the dir
files = []
for input in args.files:
    if os.path.isdir(input):
        # alternative glob.glob('files_path/[!.]*')
        files.extend([ os.path.join(input, f) for f in os.listdir(input) if not f.startswith(".") ])
    else:
        files.append(input)

# link each given file in src
for f in files:
    os.system('ln -fs ../{f} src/'.format(f=f))
