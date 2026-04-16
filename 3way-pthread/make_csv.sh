#!/bin/bash

echo "threads,mem,time" > results.csv

for f in *.out; do

    threads=$(echo "$f" | sed -n 's/.*_t\([0-9]\+\)_m.*/\1/p')
    mem=$(echo "$f" | sed -n 's/.*_m\([^_]*\)_.*\.out/\1/p')
    time=$(grep "Elapsed time" "$f" | awk '{print $3}')

    if [ ! -z "$threads" ] && [ ! -z "$mem" ] && [ ! -z "$time" ]; then
        echo "$threads,$mem,$time" >> results.csv
    fi

done

cat results.csv
