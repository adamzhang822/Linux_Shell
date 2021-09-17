#! /bin/csh -f

# TEST:
#   pwdf;ls /bin

set test = 18
set resultDir = results

echo "pwdf;ls /bin" > in.txt
echo "pwdf;ls /bin" > ${resultDir}/${test}.out
ls /bin >> ${resultDir}/${test}.out

echo "MPCS Shell error" > ${resultDir}/${test}.err

echo 0 > ${resultDir}/${test}.status

./mysh in.txt
