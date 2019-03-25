#!/bin/bash
sizes=(500 1000)
nthreads=(1 2 3 4)
for size in "${sizes[@]}"
do
    for nthreads in "${nthreads[@]}"
    do
	qsub -v size=$size,nthreads=$nthreads,schedule='dynamic' mm_p3.sge
	echo "$size - $nthreads - $schedule"

    done
done
