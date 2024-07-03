if [ $1 -eq 1 ]
then 
    gcc -g -w -o a part1.c
    ./a "Parent Msg
" "Child1 Msg
" "Child2 Msg
" 3
rm a    

elif [ $1 -eq 2 ]
then
    gcc -g -w -o part2 part2.c
    ./part2 "First message" "Second message" "Third message" 3 > output2.txt
    rm part2

elif [ $1 -eq 3 ]
then
    gcc -g -w -o a buffered_open.c
    ./a

elif [ $1 -eq 4 ]
then
    mkdir ./source_directory
    mkdir ./source_directory/subdir1
    mkdir ./source_directory/subdir2
    touch ./source_directory/file1.txt
    touch ./source_directory/file2.txt
    chmod 777 ./source_directory/file1.txt
    chmod 666 ./source_directory/file2.txt
    touch ./source_directory/subdir1/file4.txt
    touch ./source_directory/subdir1/file3.txt
    touch ./source_directory/subdir2/file5.txt
    ln -s ../file1.txt ./source_directory/subdir2/file6.txt
    gcc -g -w -o main_program copytree.c part4.c
    ./main_program source_directory dest_directory
fi

