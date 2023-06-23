#!/bin/bash

echo "Content-type: text/plain"
echo

#echo "QUERY_STRING = $QUERY_STRING"

myDir=/var/www/top/html/cgi

if [ -e top ]   #file exist ?
then
  ./top $QUERY_STRING
else
  echo "file not found"
fi
