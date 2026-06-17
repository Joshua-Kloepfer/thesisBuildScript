#!/bin/bash
# srun -N1 -n1 -c1 --cpu-bind=threads --threads-per-core=1 -m block:cyclic ./executable

minElem=$1
maxElem=$2
elemIncr=$3
runsPer=$4
order=$5
dim=$6

ulimit -s unlimited

echo "RUNNING WITH: minElem:$minElem, maxElem:$maxElem, elemIncr:$elemIncr, runsPer:$runsPer, order: $order, dim: $dim"

for numElem in $(awk -v min="$minElem" -v max="$maxElem" -v incr="$elemIncr" 'BEGIN {
	for(i=min; i<=max; i = int(i * incr)) print i}'); do
	srun -N1 -n1 -c1 --cpu-bind=threads --threads-per-core=1 -m block:cyclic rocprof --hip-trace  ./IntegratorMfemTest -o $order -s 1 -n $numElem -t $((dim - 2)) -r $runsPer
done
