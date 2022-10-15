#!/usr/bin/python3

import datetime

print("Content-Type: text/plain")
print()
print(datetime.datetime.strftime(datetime.datetime.now(), "%H:%M:%S"))