#!/bin/bash

echo -e 'HTTP/1.1 200 OK'
echo -e 'Content-Type: image/jpeg\r\n'

# echo $(wc -c /Users/anastasiianifantova/Desktop/ft-server/awebsite/img/apple.jpeg | awk '{print $1}')

cat /Users/anastasiianifantova/Desktop/ft-server/cgi-bin/tmp/suit.png

