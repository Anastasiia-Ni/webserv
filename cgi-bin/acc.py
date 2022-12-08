import http
""" 
if(cookie[SID])
    name = sessions[a].getName()
    counter = sessions[a].getNumberOfVistis()
    print("Hello ", name, "1!")
    print("You have visited this page ", counter, " Times")
else
"""
print("HTTP/1.1 301 Multiple Choices")
# print("Content-type: text/html\r\n\r\n")
print("Location: /services.html\r\n")

print("<html>")
print("<title>Login</title>")
print("<body> ")
print("<b>AMANIX Session Test</b><br /><br />")
print("<form action=\"/cgi-bin/auth.py\" method=\"POST\">")
print("Username: <input type=\"text\" name=\"username\">")
print("Password: <input type=\"password\" name=\"password\"><br /><br />")
print("<input type=\"submit\" value=\"Submit\" />")
print("</form>")
print("</body>")
print("</html>")