#!/usr/bin/python3

import cgi, os
# import cgitb; cgitb.enable()
# f = open('/tmp/log.moa' ,'w')
# f.write("Test\n")
form = cgi.FieldStorage()
# f.write(str(form))

# Get filename here.

fileitem = form['filename']

# Test if the file was uploaded
if fileitem.filename:
   open('/home/vscode/src/Cursus/hAmanix/cgi-bin/tmp/' + os.path.basename(fileitem.filename), 'wb').write(fileitem.file.read())
   message = 'The file "' + os.path.basename(fileitem.filename) + '" was uploaded successfully'
else:
   message = 'Uploading Failed'
 
print("Content-Type: text/html;charset=utf-8")
print ("Content-type:text/html\r\n")
print("<H1> " + message + " </H1>")

