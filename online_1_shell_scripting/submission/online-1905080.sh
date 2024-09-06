#!/bin/bash

# mkdir "$2"
# user=$user
# mkdir -p $2/{user,others,group}

count=0
organize()
	{
		if [ -d "$1" ]
		then
			for i in "$1"/*
			do
				organize "$i"
			done
		elif [ -f "$1" ]
		then
			array[$count]=echo ls $i
			echo "${array[$count]}"
			count=$(( count + 1 ))
		fi
	}

organize $1