#!/bin/bash
planPolicies=('static' 'dynamic' 'guided')
for nthreads in 1 2 3 4; do 
	for schedule in static dynamic guided; do
		for size in 500 1500 2000 2500; do
			qsub -v size=$size,nthreads=$nthreads,schedule=$schedule mm2_p3.sge
			echo "$size - $nthreads - $schedule"
		done
   	done
done
