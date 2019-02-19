#!/usr/bin/env python3

import subprocess, os, sys

'''Return to root directory (cd .) via chdir()'''
def returnToRoot():
    os.chdir(rootDirectory)

'''Checks the rwx permissions for a file and adds them to the dictionary p'''
def addPermissions(filename, p):
    r = os.access(filename, os.R_OK)
    w = os.access(filename, os.W_OK)
    x = os.access(filename, os.X_OK)
    p[filename] = [r, w, x]
    return

'''given a string p, containing only r w or x, and a file,
   this function checks if the given permissions are set 
   for that file'''
def checkPermissions(file, p):
    if len(p) > 3:
        return False
    status = permissions[file]
    val = True;
    p = list(p)

    for a in p:
        if a == 'r':
            val = val and status[0]
        elif a == 'w':
            val = val and status[1]
        elif a == 'x':
            val = val and status[2]
    return val

'''Given a filename, checks the permissions for that file
    and returns a string describing the set permissions
    i.e. a file w/ read and write permissions would return rw-'''
def getPermissions(filename):
    val = ""
    if permissions[filename][0] == True:
        val += 'r'
    else:
        val += '-'
    if permissions[filename][1] == True:
        val += 'w'
    else:
        val += '-'
    if permissions[filename][2] == True:
        val += 'x'
    else:
	    val += '-'
    return val

'''Usage function to print error message and exit the script if command line arguments are invalid'''
def Usage(arg):
    print('Usage: ' + arg + ': ' + 'incorrect arguments')
    print('   ' + arg + ' <-option> <user_id>')
    print('   Options: -e -> encrypt, -d -> decrypt')
    sys.exit(-1)

'''Given a root directory, traverse the root and all subdirectories.
    The 'path' variable will allow for manipulation of individual files
    in the directory tree'''
def traverse_encrypt(root):
    for directory, subDir, fileList in os.walk(root, topdown=True):
        for fname in fileList:
            path = directory + '/' + fname
            #addPermissions(path, permissions)
            #val = checkPermissions(path, 'rwx')
            if fname in accessibleFiles:
                print('\t%s' % path)
                '''################################################
                Encrypt files here
                'path' gives the full path of the file if necessary
                remove print statement when finished
                ################################################'''

        '''-encrypt UserId after encryption
           -call encrypt_symmetric_key() 
        '''

def traverse_decrypt(root):
    for directory, subDir, fileList in os.walk(root, topdown=True):
        for fname in fileList:
            path = directory + '/' + fname
            #addPermissions(path, permissions)
            #val = checkPermissions(path, 'rwx')
            if fname in accessibleFiles:
                print('\t%s' % path)
                '''################################################
                Decrypt files here
                'path' gives the full path of the file if necessary
                remove print statement when finished
                ################################################'''


from rw_permissions import permissions_to_dict, write_permissions_to_file, append_permissions_to_file

'''Checks for correct number of arguments'''
if len(sys.argv) != 3:
    Usage(sys.argv[0])

'''Set the encrypt/decrypt option, the hexadecimal form of the userID,
   the UID for each RFID card. This is hard-coded here for simplicity.
   In practice, when using many RFID cards, this would be implemented as a dictionary'''
option = sys.argv[1]
user_hex = int(sys.argv[2], 16)
user1000 = 0x45538204
user1001 = 0x1e87e50c
user = ''

'''Set the identifier for the user based on the hex value. 
   They are implemented as integer values because they will be used in encryption/decryption'''
if user_hex == user1000:
    user = 1000
if user_hex == user1001:
    user = 1001

'''Set the root directory to the cwd, read the permissions file into a dictionary,
   and create a list of accessible files unique to that user'''
permissions = {}
rootDirectory = os.path.dirname(os.path.realpath(__file__))
'''The permissions file is hardcoded because it should not be changed, nor should its name'''
permissions = permissions_to_dict('userFiles')
accessibleFiles = list(permissions[str(user)])

'''Depending on the option set on the command line, the script will either encrypt
   or decrypt the files belonging to the user'''

if option == '-e':
    traverse_encrypt(rootDirectory)

if option == '-d':
    traverse_decrypt(rootDirectory)

if option == '-t':
    print("TESTING SUBPROCESS CALLS")
    print(hex(user_hex))
    print(user)
