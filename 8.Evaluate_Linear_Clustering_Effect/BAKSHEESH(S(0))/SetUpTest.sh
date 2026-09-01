#!/bin/bash

for ((a=1; a<23; a++)); do
    mkdir -p "$a.Round"

    cp main.cpp "$a.Round/main.cpp"

    sed -i '' "11c\\
#define TestRound $a
" "$a.Round/main.cpp"
done
