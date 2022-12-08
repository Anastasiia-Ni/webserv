#!/bin/bash

# printf "HTTP/1.1 200 OK\r\nContent-type: image/png\r\n"
# # printf "Content-Length: 56508\r\n\r\n"
# printf "Content-Length: "
# printf $(wc -c /Users/anastasiianifantova/Desktop/ft-server/awebsite/img/apple.jpeg | awk '{print $1}')
# printf "\r\n\r\n"
# cat /Users/anastasiianifantova/Desktop/ft-server/awebsite/img/apple.jpeg

###################
echo -e 'HTTP/1.1 200 OK'
echo -e 'Content-Type: image/jpeg\r\n'
# echo 'Content-Length: '
# echo $(wc -c /Users/anastasiianifantova/Desktop/ft-server/awebsite/img/apple.jpeg | awk '{print $1}')

cat /Users/anastasiianifantova/Desktop/server-main/cgi-bin/Stray.png

