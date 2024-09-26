#!/bin/bash

mv $1 hehetrasj/

if [ $? -ne 0 ]; then
	printf "HTTP/1.1 500 Internal Server Error\r\nContent-Type: default\r\nServer: autoindex_cgi\r\nDate: $(date -R)\r\n\r\n<!doctype html>
<html>
<body>
<text> Not Deleted $1 :( </text>
</body>
</html>"
else
	printf "HTTP/1.1 200 OK\r\nContent-Type: default\r\nServer: autoindex_cgi\r\nDate: $(date -R)\r\n\r\n<!doctype html>
<html>
<body>
<text> Deleted $1 :) </text>
</body>
</html>"
fi
