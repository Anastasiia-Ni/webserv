#! /usr/bin/python3

import cgi, os

f = '/Users/anastasiianifantova/Desktop/ft-server/cgi-bin/tmp/cookies.jpeg'

if os.path.isfile(f):
    os.remove(f)
    message = 'File was deleted successfully'
else:
    message = 'Deleting failed'
 
print("Content-Type: text/html;charset=utf-8")
print ("Content-type:text/html\r\n")
print("<H1> " + message + " </H1>")