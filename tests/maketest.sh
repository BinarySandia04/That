#!/bin/bash

TestFile="tests.txt"

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

tests=0
wrong=0

rmdir -r _build 2> /dev/null
mkdir _build 2> /dev/null

printf "%-12s | %-8s | %-8s | %-8s | %-8s\n" "Test" "C++" "That(C)" "Python" "That(I)"

dotest() {
    tests=$(($tests + 1))
    
    cd "$1"
    name=${1::-1}
    printf "$Green%-12s$Reset" $name

    rm code 2> /dev/null
    rm -R .that 2> /dev/null

    g++ code.cpp -lm -o code
    # echo "C++:"
    local T1=`(time ./code > cpp.out) 2>&1 | grep real | awk '{print $2}'`
    ../That -c code.that

    # echo "That (C):"
    local T2=`(time ./code > thatc.out) 2>&1 | grep real | awk '{print $2}'`
    rm code

    # echo "Python:"
    local T3=`(time python3 code.py > py.out) 2>&1 | grep real | awk '{print $2}'`

    local T4=`(time ../That code.that > thati.out) 2>&1 | grep real | awk '{print $2}'`
    
    printf " | $Yellow%-8s$Reset | $Yellow%-8s$Reset | $Yellow%-8s$Reset | $Yellow%-8s$Reset\n" $T1 $T2 $T3 $T4

    if ! cmp --silent -- "cpp.out" "thatc.out"; then
        echo -e -n "$Red"
        echo -e -n "Compiled differ$Reset\n"
        wrong=$(($wrong + 1))
        echo "Expected:"
        cat cpp.out
        echo "Got:"
        cat thatc.out
    fi

    if ! cmp --silent -- "cpp.out" "thati.out"; then
        echo -e -n "$Red"
        echo -e -n "Interpreted differ$Reset\n"
        wrong=$(($wrong + 1))
        echo "Expected:"
        cat cpp.out
        echo "Got:"
        cat thati.out
    fi

    # Clean
    rm cpp.out 2> /dev/null
    rm thatc.out 2> /dev/null
    rm thati.out 2> /dev/null
    rm py.out 2> /dev/null

    rm code 2> /dev/null
    rm -R .that 2> /dev/null

    cd ..
}

if [ $# -eq 0 ]; then
    while read d; do
        if [ "$d" != "_build/" ]; then
            # Fem test per cadascún
            dotest $d
        fi
    done < $TestFile
else
    dotest $1/
fi

printf "Result: "
if [ $wrong -eq 0 ]; then
    printf "$Green"
    printf "All tests passed$Reset\n"
else
    printf "$Red$wrong errors$Reset\n"
fi
rmdir -r _build 2> /dev/null