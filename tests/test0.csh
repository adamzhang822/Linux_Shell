#! /bin/csh -f

# TEST:
#   [ too many parameters to shell ]

set test = 0

echo "MPCS Shell error" > results/${test}.err
touch results/${test}.out

echo 1 > results/${test}.status

rm -f in.txt
touch in.txt

./mysh in.txt bar.txt

