#!/bin/bash

if test "$#" -ne 1
then 
    exit
fi

counter=0

while read line
do
if  echo "$line" | grep -E "^[A-Z][A-Za-z0-9 ,]*" | grep -E "$1" | grep -v ",[ ]*si " |  grep -E -q "[?|!|.]$"
then (( counter++ ))
fi
done

echo "$counter"