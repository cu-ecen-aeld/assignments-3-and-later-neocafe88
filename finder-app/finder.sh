#!/bin/bash

if [ $# -eq 2 ]
then
    if [ -d $1 ] 
    then
        n_files=$(find $1 -type f | wc -l)
        n_matches=$(grep $2 $1/* -R -I | wc -l)

        echo "The number of files are $n_files and the number of matching lines are $n_matches"
    else
        echo $1 does not exist
        exit 1
    fi 
else
    echo [Usage] finder.sh files_dir search_string
    exit 1
fi 
