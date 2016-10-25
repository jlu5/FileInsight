#!/usr/bin/env python3
"""
This script expands Unix-like symlinks for the Windows operating system. For FileInsight, it is used
to bundle the Oxygen icon theme with the Windows release, and linked icons would otherwise fail to load.
"""

import fnmatch
import sys
import os
import traceback
import shutil

try:
    path = sys.argv[1]
except IndexError:
    print("ERROR: No icons folder specified!", file=sys.stderr)
    sys.exit(1)

def is_ascii(s):
    try:
        s.encode('ascii')
    except UnicodeDecodeError:
        return False
    else:
        return True

# Unix-style symlinks are extracted as a text file containing the target file as plain text.
for root, dirs, files in os.walk(path):
    # Recursively search for all PNG files
    for filename in fnmatch.filter(files, '*.png'):
        fullpath = os.path.join(root, filename)
        fullpath = os.path.abspath(fullpath)
        with open(fullpath, 'rb') as f:
            # Open and decode their contents.
            try:
                target = f.read().decode('utf-8', 'strict')
            except:
                continue
        fulltargetpath = os.path.join(root, target)
        fulltargetpath = os.path.abspath(fulltargetpath)
        print("Replacing broken link %s with %s" % (fullpath, fulltargetpath))
        # Replace the broken link (source) with the real file (dest)
        try:
            shutil.copy(fulltargetpath, fullpath)
        except OSError:
            traceback.print_exc()