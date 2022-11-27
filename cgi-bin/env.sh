#!/bin/bash

printf "HTTP/1.1 200 OK\r\nContent-type: text/plain\r\n\r\n"

echo 'Environment:'
echo ''
env

# echo 'HTTP/1.1 200 OK\r\n'
# echo 'Content-Type: text/plain\r\n\r\n'
# echo ''
# echo 'Environment:'
# echo ''
# env
