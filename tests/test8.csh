#! /bin/csh -f

# TEST:
#   ls -la /bin > results/fout.1

set test = 8 
set tmpFile = fout.1
set resultDir = results

echo "ls -la /bin > ${resultDir}/${tmpFile}" > in.txt

echo "ls -la /bin > ${resultDir}/${tmpFile}" > ${resultDir}/${test}.out

rm -f ${resultDir}/${tmpFile}
rm -f ${resultDir}/${test}.${tmpFile}
ls -la /bin > ${resultDir}/${test}.${tmpFile}

rm -f ${resultDir}/${test}.err
touch ${resultDir}/${test}.err

echo 0 > results/${test}.status

./mysh in.txt
