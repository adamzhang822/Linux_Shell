#! /bin/csh -f

# TEST:
#   ls -la /bin
#   pwd
#   cal

set test = 14

echo "ls        -la        /bin   " > in.txt
echo "  pwd   " >> in.txt
echo "	cal	" >> in.txt

echo "ls        -la        /bin   " > results/${test}.out
ls -la /bin >> results/${test}.out
echo "  pwd   " >> results/${test}.out
pwd >> results/${test}.out
echo "	cal	" >> results/${test}.out
cal >> results/${test}.out

rm -f results/${test}.err
touch results/${test}.err

echo 0 > results/${test}.status

./mysh in.txt
