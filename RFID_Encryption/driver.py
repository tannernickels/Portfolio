#!/usr/bin/env python3

def negate_flag(flag):
    flag = not flag
    return flag

import subprocess, os, sys

flag_U00 = False
flag_U01 = False
user1000 = 0x45538204
user1001 = 0x1e87e50c
currentUser = 0x00000000

'''Get UID from RFID scanner here'''
'''currentUser = scan_function()'''
'''-> currentUser is a hex value '''

currentUser = user1000


if currentUser == user1000:
    flag_U00 = True;
if currentUser == user1001:
    flag_U01 = True;

'''TESTING OPTION CALL ON TRAVERSE.PY'''
subprocess.call(['traverse.py', '-e', str(hex(currentUser))])

'''traverse.py -d currentUser'''

'''
while flag_U01 != flag_U00:

    if RFID is scanned again:
        if flag_U00:
            flag_U00 = negate_flag(flag_U00)
        elif flag U01:
            flag_U01 = negate_flag(flag_U00)
    (this will allow the loop to terminate)'''

'''traverse.py -e currentUser'''

sys.exit(0)
