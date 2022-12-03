#!/bin/bash

echo -e 'HTTP/1.1 200 OK'
echo -e 'Content-Type: text/plain\r\n\r\n'

echo '<h3>'
echo 'Environment:'
echo '</h3>'
echo ''
echo '<pre>' 
env 
echo '</pre>'

echo '<h3>'
echo 'Hostname'
echo '</h3>'
echo '<pre>' 
hostname
echo '</pre>'
