if [ $1 -eq 1 ]
then 
    gcc -g -w -o a part1.c
    ./a "Parent Msg
" "Child1 Msg
" "Child2 Msg
" 7

elif [ $1 -eq 2 ]
then
    gcc -g -w -o part2 part2.c
    ./part2 "First message
" "Second message
" "Third message
" 3 > output2.txt
fi