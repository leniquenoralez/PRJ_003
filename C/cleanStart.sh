#!/bin/bash

make clean

if [ $? -eq 0 ]; then
    echo "Clean-up successful."

    make

    if [ $? -eq 0 ]; then
        echo "Compilation successful."
        ./sws
    else
        echo "Compilation failed."
    fi
else
    echo "Clean-up failed."
fi
