#!/bin/bash

echo "listing files in $1" 1>&2

FILES=$(ls -l $1)

if [ "$?" -ne "0" ]; then
	echo "FAIL"
	exit -1;
fi

printf "HTTP/1.1 200 OK\r\nContent-Type: default\r\nServer: autoindex_cgi\r\nDate: $(date -R)\r\c" 

A=$(echo "<html><head>
<meta charset="UTF-8">
<style type="text/css">
:root {
font-family: sans-serif;
}
img {
border: 0;
}
th {
text-align: start;
white-space: nowrap;
}
th > a {
color: inherit;
}
table[order] > thead > tr > th {
cursor: pointer;
}
table[order] > thead > tr > th::after {
display: none;
width: .8em;
margin-inline-end: -.8em;
text-align: end;
}
table[order="'"'"asc"'"'"] > thead > tr > th::after {
content: "'"'"\2193"'"'"; /* DOWNWARDS ARROW (U+2193) */
}
table[order="'"'"desc"'"'"] > thead > tr > th::after {
content: "'"'"\2191"'"'"; /* UPWARDS ARROW (U+2191) */
}
table[order][order-by="'"'"0"'"'"] > thead > tr > th:first-child > a ,
table[order][order-by="'"'"1"'"'"] > thead > tr > th:first-child + th > a ,
table[order][order-by="'"'"2"'"'"] > thead > tr > th:first-child + th + th > a {
text-decoration: underline;
}
table[order][order-by="'"'"0"'"'"] > thead > tr > th:first-child::after ,
table[order][order-by="'"'"1"'"'"] > thead > tr > th:first-child + th::after ,
table[order][order-by="'"'"2"'"'"] > thead > tr > th:first-child + th + th::after {
display: inline-block;
}
table.remove-hidden > tbody > tr.hidden-object {
display: none;
}
td {
white-space: nowrap;
}
table.ellipsis {
width: 100%;
table-layout: fixed;
border-spacing: 0;
}
table.ellipsis > tbody > tr > td {
overflow: hidden;
text-overflow: ellipsis;
}
/* name */
/* name */
th:first-child {
padding-inline-end: 2em;
}
/* size */
th:first-child + th {
padding-inline-end: 1em;
}
td:first-child + td {
text-align: end;
padding-inline-end: 1em;
}
/* date */
td:first-child + td + td {
padding-inline-start: 1em;
padding-inline-end: .5em;
}
/* time */
td:first-child + td + td + td {
padding-inline-start: .5em;
}
.symlink {
font-style: italic;
}
.dir ,
.symlink ,
.file {
margin-inline-start: 20px;
}
.dir::before ,
.file > img {
margin-inline-end: 4px;
margin-inline-start: -20px;
max-width: 16px;
max-height: 16px;
vertical-align: middle;
}
.dir::before {
content: url(resource://content-accessible/html/folder.png);
}
</style>
<link rel="'"'"stylesheet"'"'" media="'"'"screen, projection"'"'" type="'"'"text/css"'"'" href="'"'"chrome://global/skin/dirListing/dirListing.css"'"'">
<title>Index of $1</title>
</head>
<body dir="'"'"ltr"'"'">
<h1>Index of $1</h1>
<p id="'"'"UI_goUp"'"'"><a class="'"'"up"'"'" href="'"'".."'"'">..</a></p>
<table order="'"'""'"'" class="'"'""'"'">
<thead>
<tr>
<th><a href="'"'""'"'">Name</a></th>
<th><a href="'"'""'"'">Size</a></th>
<th colspan="'"'"2"'"'"><a href="'"'""'"'">Last Modified</a></th>
</tr>
</thead>
<tbody>"
echo "$FILES" | sed 1d |
while IFS= read -r line; do
	asd=( $line )
	if [[ ${asd[0]} == d* ]]; then
	echo ${asd[0]} >> filelol
		echo "<tr class="'"'"hidden-object"'"'">
<td data="'"'"${asd[8]}"'"'"><table class="'"'"ellipsis"'"'"><tbody><tr><td><a class="'"'"dir"'"'" href="'"'"${asd[8]}/"'"'">${asd[8]}</a></td></tr></tbody></table></td>
<td></td>
<td data="'"'"0"'"'">${asd[6]}/${asd[5]}</td>
<td>${asd[7]}</td>
</tr>"
	else
		echo "<tr class="'"'"hidden-object"'"'">
<td data="'"'"${asd[8]}"'"'"><table class="'"'"ellipsis"'"'"><tbody><tr><td><a class="'"'"file"'"'" href="'"'"${asd[8]}/"'"'"><img src="moz-icon://unknown?size=16" alt="File:">${asd[8]}</a></td></tr></tbody></table></td>
<td data="$((asd[4]/10))">$((asd[4]/1000)) KB</td>
<td data="'"'"0"'"'">${asd[6]}/${asd[5]}</td>
<td>${asd[7]}</td>
</tr>"
	fi
done
echo "</tbody></table>
</body></html>"
)

printf "Content-Length: ${#A}\r\n\r\n"
echo $A