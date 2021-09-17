#! /bin/csh -f

# TEST:
#   ls -R /dev

set test = 19 
set resultDir = results

echo "quit | grep mysh" > in.txt

echo "quit | grep mysh" > ${resultDir}/${test}.out

echo "MPCS Shell error" > ${resultDir}/${test}.err

echo 0 > ${resultDir}/${test}.status

./mysh in.txt
