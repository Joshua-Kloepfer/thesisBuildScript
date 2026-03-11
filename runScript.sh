#!/bin/bash
# srun -N1 -n1 -c1 --cpu-bind=threads --threads-per-core=1 -m block:cyclic ./executable

minSize=$1
maxSize=$2
sizeIncr=$3
minElem=$4
maxElem=$5
elemIncr=$6
useCab=$7
runsPer=$8

echo "RUNNING WITH: minSize:$minSize, maxSize:$maxSize, sizeIncr:$sizeIncr, minElem:$minElem, maxElem:$maxElem, elemIncr:$elemIncr, useCab:$useCab, runsPer:$runsPer"

for ((size=minSize; size<=maxSize; size+=sizeIncr)); do
	for ((numElem=minElem; numElem<=maxElem; numElem+=elemIncr)); do
		srun -N1 -n1 -c1 --cpu-bind=threads --threads-per-core=1 -m block:cyclic ./builds/AMD_GFX90A/meshfields/IntegratorPerformance $useCab 1 $size $numElem 2 $runsPer >> meshfields_output.txt
		srun -N1 -n1 -c1 --cpu-bind=threads --threads-per-core=1 -m block:cyclic ./builds/AMD_GFX90A/meshfields/IntegratorPerformance $useCab 1 $size $numElem 3 $runsPer >> meshfields_output.txt
		srun -N1 -n1 -c1 --cpu-bind=threads --threads-per-core=1 -m block:cyclic ./builds/AMD_GFX90A/meshfields/IntegratorPerformance $useCab 2 $size $numElem 2 $runsPer >> meshfields_output.txt
		srun -N1 -n1 -c1 --cpu-bind=threads --threads-per-core=1 -m block:cyclic ./builds/AMD_GFX90A/meshfields/IntegratorPerformance $useCab 2 $size $numElem 3 $runsPer >> meshfields_output.txt

		srun -N1 -n1 -c1 --cpu-bind=threads --threads-per-core=1 -m block:cyclic ./meshfields/mfemTest/IntegratorMfemTest --order 1 --size $size --numElem $numElem --type 2 --runs $runsPer >> mfem_output.txt
		srun -N1 -n1 -c1 --cpu-bind=threads --threads-per-core=1 -m block:cyclic ./meshfields/mfemTest/IntegratorMfemTest --order 1 --size $size --numElem $numElem --type 1 --runs $runsPer >> mfem_output.txt
		srun -N1 -n1 -c1 --cpu-bind=threads --threads-per-core=1 -m block:cyclic ./meshfields/mfemTest/IntegratorMfemTest --order 2 --size $size --numElem $numElem --type 2 --runs $runsPer >> mfem_output.txt
		srun -N1 -n1 -c1 --cpu-bind=threads --threads-per-core=1 -m block:cyclic ./meshfields/mfemTest/IntegratorMfemTest --order 2 --size $size --numElem $numElem --type 1 --runs $runsPer >> mfem_output.txt
	done
done
