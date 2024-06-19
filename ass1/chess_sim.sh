declare -A matrix
current_move=0
moves=""
uci_moves=""
amount_of_moves=0

init_matrix() {
    matrix[18]=8; matrix[a8]=r; matrix[b8]=n; matrix[c8]=b; matrix[d8]=q; matrix[e8]=k; matrix[f8]=b; matrix[g8]=n; matrix[h8]=r; matrix[98]=8;
    matrix[17]=7; matrix[a7]=p; matrix[b7]=p; matrix[c7]=p; matrix[d7]=p; matrix[e7]=p; matrix[f7]=p; matrix[g7]=p; matrix[h7]=p; matrix[97]=7;

    matrix[13]=3; matrix[a3]=.; matrix[b3]=.; matrix[c3]=.; matrix[d3]=.; matrix[e3]=.; matrix[f3]=.; matrix[g3]=.; matrix[h3]=.; matrix[93]=3;
    matrix[14]=4; matrix[a4]=.; matrix[b4]=.; matrix[c4]=.; matrix[d4]=.; matrix[e4]=.; matrix[f4]=.; matrix[g4]=.; matrix[h4]=.; matrix[94]=4;
    matrix[15]=5; matrix[a5]=.; matrix[b5]=.; matrix[c5]=.; matrix[d5]=.; matrix[e5]=.; matrix[f5]=.; matrix[g5]=.; matrix[h5]=.; matrix[95]=5;
    matrix[16]=6; matrix[a6]=.; matrix[b6]=.; matrix[c6]=.; matrix[d6]=.; matrix[e6]=.; matrix[f6]=.; matrix[g6]=.; matrix[h6]=.; matrix[96]=6;

    matrix[12]=2; matrix[a2]=P; matrix[b2]=P; matrix[c2]=P; matrix[d2]=P; matrix[e2]=P; matrix[f2]=P; matrix[g2]=P; matrix[h2]=P; matrix[92]=2;
    matrix[11]=1; matrix[a1]=R; matrix[b1]=N; matrix[c1]=B; matrix[d1]=Q; matrix[e1]=K; matrix[f1]=B; matrix[g1]=N; matrix[h1]=R; matrix[91]=1;
}

display_matrix() {
    echo "  a b c d e f g h"
    
    for ((i = 8 ; i >= 1; i--)) 
    do
        for j in {1,a,b,c,d,e,f,g,h}
        do
            echo -n "${matrix[$j$i]} "
        done
        echo -n "${matrix[9$i]}"
        echo ""    
    done

    echo "  a b c d e f g h"
}

display_current_move() {
    echo "Move $current_move/$amount_of_moves"
}

init_game() {
    touch moves.txt
    echo $uci_moves > moves.txt
    move_number='[0-9]+\.'
    amount_of_moves=$(egrep -o " " "moves.txt" | wc -l)
    ((amount_of_moves++))
    rm moves.txt

    init_matrix
    display_current_move
    display_matrix
}

swap() {
    local temp=${matrix[$1]}
    matrix[$1]=${matrix[$2]}
    matrix[$2]=$temp
}

advance_to_stage() {
    init_matrix
    
    local idx=0;
    for pair in $uci_moves
    do
        if [ $idx -eq $1 ]
        then
            continue
        fi

        src=${pair:0:2}
        dst=${pair:2:2}
        prom=""

        if [ ${#pair} -eq 5 ]; then
            prom=${pair:4:1}
        fi

        if [ "${matrix[$src]}" = "K" ] && [ "$src" = "e1" ]
        then 
            if [ "$dst" = "g1" ]
            then
                matrix[e1]='.'
                matrix[f1]='R'
                matrix[g1]='K'
                matrix[h1]='.'
                ((idx++))
                continue

            elif [ "$dst" = "c1" ]
            then
                matrix[e1]='.'
                matrix[d1]='R'
                matrix[c1]='K'
                matrix[a1]='.'
                ((idx++))
                continue
            fi

        elif [ "${matrix[$src]}" = "k" ] && [ "$src" = "e8" ]
        then 
            if [ "$dst" = "g8" ]
            then
                matrix[e8]='.'
                matrix[f8]='r'
                matrix[g8]='k'
                matrix[h8]='.'
                ((idx++))
                continue

            elif [ "$dst" = "c8" ]
            then
                matrix[e8]='.'
                matrix[d8]='r'
                matrix[c8]='k'
                matrix[a8]='.'
                ((idx++))
                continue
            fi
        fi

        if [ "${matrix[$src]}" = "P" ] && [ "${src:0:1}" != "${dst:0:1}" ] && [ "${matrix[$dst]}" = "." ]
        then
            fallen_pawn="${dst:0:1}5"
            matrix[$src]='.'
            matrix[$dst]='P'
            matrix[$fallen_pawn]='.'
            ((idx++))
            continue
        elif [ "${matrix[$src]}" = "p" ] && [ "${src:0:1}" != "${dst:0:1}" ] && [ "${matrix[$dst]}" = "." ]
        then
            fallen_pawn="${dst:0:1}4"
            matrix[$src]='.'
            matrix[$dst]='p'
            matrix[$fallen_pawn]='.'
            ((idx++))
            continue
        fi        

        if [ ${#prom} -ne 0 ]; then
            matrix[$dst]=$prom
        else
            swap $src $dst
        fi

        matrix[$src]='.'
        ((idx++))
    done
}

process_input() {
    echo -n "Press 'd' to move forward, 'a' to move back, 'w' to go to the start, 's' to go to the end, 'q' to quit:"
    read key
    if [ "$key" = "q" ]
    then
        echo
        echo "Exiting."
        echo "End of game."
        exit 0
    elif [ "$key" = "d" ]
    then
        if [ $current_move -ge $amount_of_moves ]
        then
            echo
            echo "No more moves available."
            return
        else
            ((current_move++))
            advance_to_stage $current_move 
        fi
    elif [ "$key" = "a" ]
    then
        if [ $current_move -gt 0 ]
        then
            ((current_move--))
            advance_to_stage $current_move
        fi
    elif [ "$key" = "s" ]
    then
        current_move=$amount_of_moves
        advance_to_stage $current_move
    elif [ "$key" = "w" ]
    then
        current_move=0
        echo
        init_game
        return 0
    else
        echo
        echo "Invalid key pressed: $key"
        return 0
    fi
    echo
    display_current_move
    display_matrix
}


main() {
    local bool=0
    local first=1

    while IFS= read -r line
    do 
        if [ $bool -eq 0 ]
        then
            if [ $first -eq 1 ]
            then
                echo "Metadata from PGN file:"
                first=0
            fi

            echo "$line"
            
            if [ -z "$line" ]
            then
                bool=1
            fi
            
        else
            moves+=$(echo -n "$line ")
        fi
    done < "$1"

    uci_moves=$(python3 parse_moves.py "$moves")
    
    init_game
    while true
    do
        process_input
    done
}


if [ ! $# -eq 1 ]
then 
    echo "Usage: $0 <source_pgn_file>s"
    exit 1
fi

if [ ! -e $1 ]
    then 
        echo "Error: File '$1' does not exist."
        exit 1
fi

main $1