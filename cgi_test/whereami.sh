#!/bin/bash

printf "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nServer: whereami\r\nDate: $(date -R)\r\n\r\n" 

echo $PWD
