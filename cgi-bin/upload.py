#!/usr/bin/python3

import cgi
import os
import cgitb; cgitb.enable()

form = cgi.FieldStorage()

# Get filename here.
fileitem = form['filename']

print("HTTP/1.1 200 OK")
print("Content-type:text/html")

# Test if the file was uploaded
if fileitem.filename:
    # strip leading path from file name to avoid
    # directory traversal attacks
    fn = os.path.basename(fileitem.filename)
	# fn = os.path.basename(fileitem.filename.replace("\\", "/" ))
    open('cgi-bin/tmp/' + fn, 'wb').write(fileitem.file.read())

    message = 'The file "' + fn + '" was uploaded successfully'

else:
    message = 'No file was uploaded'

print("<html>")
print("<head>")

print(" <b>%20s</br>" % (message))

print("</body>")
print("</html>")

# print """Content-Type: text/html\n\
# <html>
# <body>
#    <p>%s</p>
# </body>
# </html>
# """ % (message,)