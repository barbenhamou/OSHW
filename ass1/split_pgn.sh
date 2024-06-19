#!/bin/bash

if [ ! $# -eq 2 ]
    then 
        echo "Usage: $0 <source_pgn_file> <destination_directory>"
        exit 1
fi

if [ ! -e $1 ]
    then 
        echo "Error: File '$1' does not exist."
        exit 1
fi

if [ ! -d $2 ]
    then
        mkdir -p $2
        echo "Created directory '$2'."
fi

lines=$(grep '\[Event ' "$1" | wc -l)
empty=0 
i=1

if [ $lines -gt 0 ]
then
    touch "${1/.pgn/_${i}.pgn}"
    mv ${1/.pgn/_${i}.pgn} $2
    echo "Saved game to $2/${1/.pgn/_${i}.pgn}"
fi

while IFS= read -r line
do
    if [ "$line" = '' ] && [ $empty -eq 1 ]
    then
        empty=0
        mv ${1/.pgn/_${i}.pgn} $2
        (( i++ ))
        touch ${1/.pgn/_${i}.pgn}
        echo "Saved game to $2/${1/.pgn/_${i}.pgn}"
        continue

    elif [ "$line" = '' ] && [ $empty -eq 0 ]
    then
        empty=1
    fi

    echo "$line" >> "${1/.pgn/_${i}.pgn}"
done < "$1"

mv ${1/.pgn/_${i}.pgn} $2

echo "All games have been split and saved to '$2'".