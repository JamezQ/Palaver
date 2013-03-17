#!/bin/bash

if [ -z "$1" ];then
    echo USAGE $0 "dictionary.dic"
    exit 1
fi

while read -ep "Your Command: " i;do
    echo
    ./dictionary "$i" "$1"
    echo
done
