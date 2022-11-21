#!/usr/bin/python3

import cgi, cgitb 

form = cgi.FieldStorage() 

if form.getvalue('textcontent'):
   text_content = form.getvalue('textcontent')
else:
   text_content = "Not entered"

print ("Content-type:text/html\r\n\r\n")
print ("<html>")
print ("<head>")
print ("<title>Text Area - Fifth CGI Program</title>")
print ("</head>")
print ("<body>")
print ("<h2> Entered Text Content is %s</h2>")
print ("</body>")

#     <form action = "../cgi-bin/textarea.py" method = "post" target = "_blank">
#         <textarea name = "textcontent" cols = "40" rows = "4" color="grey">
# Type your text here...
#         </textarea> <br />
#         <input type = "submit" value = "Submit" />
#     </form>