#!/bin/bash

printf "HTTP/1.1 200 OK\r\nContent-type: image/png\r\n"
# printf "Content-Length: 56508\r\n\r\n"
printf "Content-Length: "
printf $(wc -c /home/vscode/src/Cursus/test_temp/cgi-bin/Stray.png | awk '{print $1}')
printf "\r\n\r\n"
# # echo 'работай плиз: image/jpeg'
# # echo ''
cat /home/vscode/src/Cursus/test_temp/cgi-bin/Stray.png
