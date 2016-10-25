#!/usr/bin/env python3
"""
This helper script generates .def module definitions from a MinGW DLL, which can then be used to
create MSVC-compatible import libraries (.lib files) using Microsoft's LIB utility.

Based off notes at: https://wiki.videolan.org/GenerateLibFromDll
"""

from __future__ import print_function
import sys
import re
import subprocess

try:
    dll = sys.argv[1]
except IndexError:
    print("ERROR: No DLL specified!", file=sys.stderr)
    sys.exit(1)

dumpbin_output = subprocess.check_output(["dumpbin", "/exports", dll]).decode()
# What we want to do is find all the exported symbol names that MSVC's dumpbin shows, and then
# write those to a format that Microsoft's LIB tool understands.
_symbol_re = re.compile(r"^\s+\d+\s+[0-9A-F]+\s+[0-9A-F]{8} (.*?)$")
print('EXPORTS')
for line in dumpbin_output.splitlines():
    match = _symbol_re.match(line)
    if match:
        print(match.group(1))
