from collections import defaultdict
import sys
import subprocess

'''Reads a permissions file and converts it into a defaultdict to
    be used by the permissions.py script'''
def permissions_to_dict(filename):
    d = defaultdict(list)
    with open(filename) as f:
        for line in f:
            vals = line.split()
            d[vals[0]].append(vals[1])
    f.close()
    return d

'''Given a dictionary of permissions and a filename, open the file
    to write, then write the permissions to it. The reverse operation of
    permissions_to_dict()'''
def write_permissions_to_file(p, filename):
    with open(filename, 'w+') as w:
        for user in p:
            files = list(p[user])
            for f in files:
                w.write(user + ' ' + f + '\n')
    w.close()

'''Given a string 'user', a permissions file, and a filename to be added to the
    permissions file, append the new information'''
def append_permissions_to_file(user, file, p_file):
    with open(p_file, 'a+') as a:
        a.write(user + " " + file + '\n')
    a.close()

'''Generates a new permissions file with arbitrary filenames.
    Uncommenting all subprocess.call lines and the line assigning 'newDir' 
    will create a new directory and populate it with the files written
    to the permissions file'''
def generate_random_permissions_file(filename, n):
    from random import randint
    a = list('ABCDEFGHIJKLMNOPQRSTUVWXYZ')
    #newDir = filename + 'Dir'
    #subprocess.call(['mkdir', newDir])
    with open(filename, 'w+') as w:
        for f in range(int(n)):
            index = f%26
            user = (randint(1, 10)*17)%2
            newFile = 'file' + a[index] + str(f)
            #subprocess.call(['touch', newDir + '/' + newFile])
            w.write('100' + str(user) + ' ' + newFile + '\n')



generate_random_permissions_file('rand', 50)
