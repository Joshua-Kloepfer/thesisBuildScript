#!/bin/bash
# srun -N1 -n1 -c1 --cpu-bind=threads --threads-per-core=1 -m block:cyclic ./executable

minElem=$1
maxElem=$2
elemIncr=$3
useCab=$4
runsPer=$5
order=$6
dim=$7

ulimit -s unlimited

echo "RUNNING WITH: minElem:$minElem, maxElem:$maxElem, elemIncr:$elemIncr, useCab:$useCab, runsPer:$runsPer, order: $order, dim: $dim"
# srun -N1 -n1 -c1 --cpu-bind=threads --threads-per-core=1 -m block:cyclic ./builds/AMD_GFX90A/meshfields/IntegratorPerformance $useCab $order $minSize $minElem $dim $runsPer > tmp

for numElem in $(awk -v min="$minElem" -v max="$maxElem" -v incr="$elemIncr" 'BEGIN {
	for(i=min; i<=max; i = int(i * incr)) print i}'); do
	srun -N1 -n1 -c1 --cpu-bind=threads --threads-per-core=1 -m block:cyclic rocprof --hip-trace ./builds/AMD_GFX90A/meshfields/IntegratorPerformance $useCab $order 1 $numElem $dim $runsPer
done
