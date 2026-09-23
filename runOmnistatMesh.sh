minElem=$1
maxElem=$2
elemIncr=$3
useCab=$4
runsPer=$5
order=$6
dim=$7

mkdir "omniRun-$dim-$order"
cd "omniRun-$dim-$order"
for numElem in $(awk -v min="$minElem" -v max="$maxElem" -v incr="$elemIncr" 'BEGIN {
        for(i=min; i<=max; i = int(i * incr)) print i}'); do
        mkdir "omniRun-$numElem"
        cd "omniRun-$numElem"
        ${OMNISTAT_WRAPPER} usermode --start --interval 1.0
        srun -N1 -n1 -c1 --cpu-bind=threads --threads-per-core=1 -m block:cyclic ../builds/AMD_GFX90A/meshfields/IntegratorPerformance $useCab $order 1 $numElem $dim $runsPer
        cd ..
        ${OMNISTAT_WRAPPER} usermode --stopexporters
        echo "numElem = $numElem" > "energy_stats_${numElem}.txt"
        ${OMNISTAT_WRAPPER} query --job ${SLURM_JOB_ID} --interval 1 --export >> "energy_stats_${numElem}.txt"
        ${OMNISTAT_WRAPPER} usermode --stopserver
        rm -rf /tmp/omnistat/${SLURM_JOB_ID}/
done
