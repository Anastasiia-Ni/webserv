#!/usr/bin/python3

import cgi, os

form = cgi.FieldStorage()

# Get a filename here
fileitem = form['filename']

# Test if the file was uploaded
if fileitem.filename:
   open('/Users/anifanto/Desktop/ft-server/cgi-bin/tmp/' + os.path.basename(fileitem.filename), 'wb').write(fileitem.file.read())
   message = 'The file "' + os.path.basename(fileitem.filename) + '" was uploaded successfully'
else:
   message = 'Uploading Failed'

print("Content-Type: text/html;charset=utf-8")
print ("Content-type:text/html\r\n")
print("<H1> " + message + " </H1>")
