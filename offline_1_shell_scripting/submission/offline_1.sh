#!/bin/bash

if [ -n "${4+set}" ]
then
	if [ -n "${5+set}" ]
	then
		if [ $5 = "-v" ]
		then
			if [ -n "${6+set}" ]
			then
				if [ $6 = "-nonexecute" ]
				then
					printv=1
					execute=0
				else
					printv=1
					execute=1
				fi
			else
				printv=1
				execute=1
			fi
		elif [ $5 = "-nonexecute" ]
		then
			printv=0
			execute=0
		fi
	else
		printv=0
		execute=1
	fi

	mkdir -p $2/{C,Python,Java}

	if [ $execute = "1" ]
	then
		touch $2/demoResult.txt
		echo "student_id	type	matched	not_matched" >> $2/demoResult.txt
	fi

	for f in "$1"/*.zip 
	do 
		mv "$f" "$1/${f: -11}"  # renaming those zip files using student id
	done

	for f in "$1"/*.zip 
	do 
		mkdir ${f%.zip}  # making directory with student id
		unzip -q $f -d ${f%.zip}  # unziping inside the student id directory
		rm $f
	done

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
			if [[ "$1" = *.c ]]
			then
				mkdir "$target/C/$root/"
				cp "$1" "$target/C/$root/main.c"
			elif [[ "$1" = *.java ]]
			then
				mkdir "$target/Java/$root/"
				cp "$1" "$target/Java/$root/Main.java"
			elif [[ "$1" = *.py ]]
			then
				mkdir "$target/Python/$root/"
				cp "$1" "$target/Python/$root/main.py"
			fi
		fi
	}

	target="$2"
	for f in "$1"/*
	do
		root=${f: -7}
		if [ $printv = "1" ]
		then
			if [ $execute = "1" ]
			then
				echo "Organizing files of ${root}"
				echo "Executing files of ${root}"

			else
				echo "Organizing files of ${root}"
			fi
		fi
		organize "$f"
	done

	count=0
	for f in "$4"/*
	do
		count=$(( count + 1 ))
		array[count]=$f
	done

	if [ $execute = "1" ]
	then
		for g in "$2"/*
		do
			if [ -d "$g" ]
			then
				if [ "$g" = "$2/C" ]
				then
					parent="C"
				elif [ "$g" = "$2/Python" ]
				then
					parent="Python"
				else
					parent="Java"
				fi
				for f in "$g"/*
				do	
					if [ $parent = "C" ]
					then
						gcc "$f/main.c" -o "$f/main.out"
					elif [ $parent = "Java" ]
					then
						javac $(realpath $f/Main.java)
					fi
					root=${f: -7}
					num=0
					matched=0
					not_matched=0
					for i in "$3"/*
					do
						num=$(( num + 1 ))
						if [ $parent = "C" ]
						then
							$f/./main.out < "$i" > "$f/out$num.txt"
						elif [ $parent = "Java" ]
						then
							java -cp $(realpath $f/)  Main < "$i" > "$f/out$num.txt"
						else
							python3 $f/main.py < "$i" > "$f/out$num.txt" 
						fi

						if cmp -s -- "${array[$num]}" "$f/out$num.txt"
						then
							matched=$(( matched + 1 ))
						else
							not_matched=$(( not_matched + 1 ))
						fi
					done
					combined="${root}\t${parent}\t${matched}\t${not_matched}"
					echo -e "$combined" >> $2/demoResult.txt
				done
			fi
		done
		mv $2/demoResult.txt $2/result.csv
	fi
else
	echo "Usage:"
	echo "./organize.sh <submission folder> <target folder> <test folder> <answer folder> [-v] [nonexecute]"
	echo ""
	echo "-v: verbose"
	echo "-nonexecute: do not execute code files"
fi

