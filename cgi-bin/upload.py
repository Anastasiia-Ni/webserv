#!/usr/bin/python3

import cgi
import os
import sys
import cgitb; 
cgitb.enable()
from unicodedata import name


print("HTTP/1.1 200 OK")
print("Content-type:text/html\r\n\r\n")
# print("Transfer-Encoding: chunked\r\n")
# print("Location: tmp\r\n\r\n")
# print()  

for param in os.environ.keys():
    print("<b>%20s</b>: %s<br>" % (param, os.environ[param]))
form = cgi.FieldStorage()
# form = cgi.FieldStorage(
#     environ={'REQUEST_METHOD':'POST'})

if "filename" not in form:  #or "addr" not in form:
    print("<H1>Error</H1>")
    print("Please fill in the name and addr fields.")
else:
	print("<p>name:", form["filename"].value)
# 	#print("<p>addr:", form["addr"].value)
# # print(form.keys)
# # fileitem = form['filename']

# content_type = os.getenv("CONTENT_TYPE")

# # print("content type: ", content_type)

# cont_len = os.getenv("CONTENT_LENGTH")

# # print("content length ", cont_len)


# # data = sys.stdin.buffer.read()


# # print("<p><b>%s</b>", data)


# # fileitem = "filename"

# # data = sys.stdin.buffer.read()
# type = os.getenv("CONTENT_TYPE")
# # print(type)
# index = type.find("boundary=")
# index = index + 9
# # print("BOUNDARY")
# boundary = type[index:]
# # print(boundary)
# boundary = bytes("--" + boundary + "--", encoding="utf-8")


# # #filename
# # i = 0
# start = data.find(b"filename=\"")

# if start > 0:
# 	start += 10
# 	end = data.find(b"\"", start)
# 	filename = data[start:end]
# else:
# 	filename = "default"
# path = b"cgi-bin/upload/"
# filename = path + filename.encode('utf-8')

# print(filename)

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

# Test if the file was uploaded
# if fileitem.filename:
#     # strip leading path from file name to avoid
#     # directory traversal attacks
# 	fn = os.path.basename(fileitem.filename)
# 	# fn = os.path.basename(fileitem.filename.replace("\\", "/" ))
# 	open('cgi-bin/tmp/' + fn, 'wb').write(fileitem.file.read())
# 	message = 'The file "' + fn + '" was uploaded successfully'

# else:
# 	message = 'No file was uploaded'

# print("<html>")
# print("<head>")

# print("<html>")
# print("<body>")

# print("<h3>%20s</h3>" % (message))

# print("</body>")
# print("</html>")