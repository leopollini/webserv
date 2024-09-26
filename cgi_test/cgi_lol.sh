#!/bin/bash

printf "HTTP/1.1 200 OK\r\nContent-Type: default\r\nServer: autoindex_cgi\r\nDate: $(date -R)\r\n\r\n" 

A=$(cat -)

echo $QUERY_STRING 1>&2
echo $A 1>&2
# while true; do
# 	sleep 1
# done
