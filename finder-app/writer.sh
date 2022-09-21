#!/bin/bash
  
if [ $# -eq 2 ]
then
    mkdir -p $(dirname "$1")
    echo $2 > $1
else
    echo [Usage] writer.sh write_file write_str
    exit 1
fi 
