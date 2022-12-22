#! /usr/bin/python3

import cgi, os

f = '/Users/anifanto/Desktop/ft-server/fusion_web/upload/Ahmed160.jpeg'

if os.path.isfile(f):
    os.remove(f)
    message = 'File was deleted successfully'
else:
    message = 'Deleting failed'

print("Content-Type: text/html;charset=utf-8")
print ("Content-type:text/html\r\n")
print("<H1> " + message + " </H1>")