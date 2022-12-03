#!/usr/bin/python3

import cgi, os
# import cgitb; cgitb.enable()
f = open('/tmp/log.moa' ,'w')
f.write("Test\n")
# for name, value in os.environ.items():
#     print("{0}: {1}".format(name, value),file=f)
form = cgi.FieldStorage()
f.write(str(form))
# Get filename here.
fileitem = form['filename']
# Test if the file was uploaded
if fileitem.filename:
   # strip leading path from file name to avoid
   # directory traversal attacks
   # fn = os.path.basename(fileitem.filename)
   open('/home/vscode/src/Cursus/test_temp/cgi-bin/tmp/' + os.path.basename(fileitem.filename), 'wb').write(fileitem.file.read())
	# open('/home/vscode/src/Cursus/test_temp/cgi-bin/suit.png' ,'wb').write(fileitem.file.read())
#    open('/tmp/' + fn, 'wb').write(fileitem.file.read())
   message = 'The file "' + os.path.basename(fileitem.filename) + '" was uploaded successfully :)'
else:
   message = 'Uploading Failed :('
 
print("Content-Type: text/html;charset=utf-8")
print ("Content-type:text/html\r\n")
print("<H1> File "+ message + " </H1>")

