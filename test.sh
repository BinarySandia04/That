#!/bin/bash

# Reset
Reset='\033[0m'       # Text Reset

# Regular Colors
Black='\033[0;30m'        # Black
Red='\033[0;31m'          # Red
Green='\033[0;32m'        # Green
Yellow='\033[0;33m'       # Yellow
Blue='\033[0;34m'         # Blue
Purple='\033[0;35m'       # Purple
Cyan='\033[0;36m'         # Cyan
White='\033[0;37m'        # White

wrong=0

echo -e "${Yellow}Testing files${Reset}"

FILES=`find tests/ -type f -print`
for f in $FILES
do
    awk 'BEGIN{RS="=========="; ORS=""} { textNumber++; print $0 > textNumber".out" }' $f
    tail -n +2 2.out > 3.out
    mv 3.out 2.out

    echo -n "$f --> "
    zag 1.out > 3.out
    if [[ $(diff -q 2.out 3.out) ]]; then
        echo -e "${Red}Failed${Reset}"
        diff 2.out 3.out
        wrong=1
    else
        echo -e "${Green}Passed${Reset}"
    fi
done

rm 1.out 2.out 3.out
