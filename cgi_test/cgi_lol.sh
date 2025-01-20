#!/bin/bash

# simply echoes quesy string and request body

printf "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nServer: cgi_lol\r\nDate: $(date -R)\r\n\r\n" 

A=$(cat -)
echo $QUERY_STRING
echo $A
# while true; do
# 	sleep 1
# done
