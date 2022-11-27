#!/usr/bin/python3

import cgi
import os
import sys
import cgitb; cgitb.enable()
from unicodedata import name

form = cgi.FieldStorage()
fileitem = form['filename']

# data = sys.stdin.buffer.read()

# type = os.getenv("CONTENT_TYPE")
# index = type.find("boundary=")
# index = index + 9
# boundary = type[index:]
# boundary = bytes("--" + boundary + "--", encoding="utf-8")

# #filename
# i = 0
# start = data.find(b"filename=\"")
# if start > 0:
# 	start += 10
# 	end = data.find(b"\"", start)
# 	filename = data[start:end]
# else:
# 	filename = "default"
# path = b"cgi-bin/upload/"
# filename = path + filename

# #file type
# start = data.find(b"Content-Type:")
# if start >= 0:
# 	start += 14
# 	end = data.find(b"/", start)
# 	filetype = data[start:end]
# else:
# 	filetype = "NULL"

# if filetype == "text":
# 	flags = "wt"
# else:
# 	flags = "wb"
# strfilename = str(filename, "utf-8")
# f = open(strfilename, flags)

# #body-
# start = data.find(b"\n", end)
# start += 3
# end = data.find(boundary)
# f.write(data[start:end - 2])
# f.close()
# print ("HTTP/1.1 200 OK\r\n\r\n")



print("HTTP/1.1 200 OK")
print("Content-type:text/html\r\n")
# print("Transfer-Encoding: chunked\r\n")
# print("Location: tmp\r\n")

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

print("<html>")
print("<body>")

print("<h3>%20s</h3>" % (message))

print("</body>")
print("</html>")
