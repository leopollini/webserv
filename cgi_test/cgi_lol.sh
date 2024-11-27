#!/bin/bash

# simply echoes quesy string and request body

printf "HTTP/1.1 200 OK\r\nContent-Type: default\r\nServer: autoindex_cgi\r\nDate: $(date -R)\r\n\r\n" 

A=$(cat -)
echo $QUERY_STRING
echo $A
# while true; do
# 	sleep 1
# done
