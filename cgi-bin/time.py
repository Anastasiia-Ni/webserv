#!/usr/bin/python3

import datetime


print("Content-type: text/html\r\n\r\n")
print()
print(datetime.datetime.strftime(datetime.datetime.now(), "%H:%M:%S"))