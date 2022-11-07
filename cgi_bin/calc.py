
#! /usr/bin/python3

import sys

print("Content-type: text/html\r\n\r\n")
print("<font size=+10>Environment</font><br>")

if len(sys.argv) > 4:
    print("Parameters are not correct")
    exit(1)
elif sys.argv[2] not in "+-/*^&" or not len(sys.argv[2]) == 1:
    print("Parameters are not correct")
    exit(1)
if sys.argv[1][0] in '-+':
    if not sys.argv[1][1:].isdigit():
        print("Parameters are not correct")
        exit(1)
elif not sys.argv[1].isdigit():
        print("Parameters are not correct")
        exit(1)
if sys.argv[3][0] in '-+':
    if not sys.argv[3][1:].isdigit():
        print("Parameters are not correct")
        exit(1)
elif not sys.argv[3].isdigit():
        print("Parameters are not correct")
        exit(1)  
elif sys.argv[3][0] == '-' and not sys.argv[3][1:].isdigit():
    print("Parameters are not correct")
    exit(1)

num1 = int(sys.argv[1])
num2 = int(sys.argv[3])
if (num1 > 10000 or num2 > 10000):
    print("Enter a lower number")
    exit(1)
result = 0
oper = sys.argv[2]

if oper == '+': result = num1 + num2
elif oper == '-': result = num1 - num2
elif oper == '/': result = round(num1 / num2, 2)
elif oper == '*': result = round(num1 * num2, 2)
elif oper == '^': result = round(pow(num1, num2), 2)
elif oper == '&': result = round(pow(num1, 1 / num2), 2)

try:
    print("<b>%s</b>: <br>" % str(result))
except:
    print("<b>%s</b>: <br>" %("Something went wrong"))