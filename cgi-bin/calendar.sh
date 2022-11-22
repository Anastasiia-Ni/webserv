#!/bin/bash


echo 'HTTP/1.1 200 OK'
echo 'Content-Type: text/html'
echo ''
echo '<h2>Calendar</h2>'
echo '<pre>'
cal
echo ''
date
echo '</pre>'