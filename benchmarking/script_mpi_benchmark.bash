#!/bin/bash

max_ranks=6
threads=1
cmd="mpirun --np"

echo "max ranks: "

for ranks in $(seq 1 ); do
    echo "Run with $ranks ranks"
    $cmd $ranks ./benchmark_mpi $threads
done

	     
