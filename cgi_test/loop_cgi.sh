#!/bin/bash

printf "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nServer: loop_cgi\r\nDate: $(date -R)\r\n\r\n" 

while true; do
	sleep 1
done
