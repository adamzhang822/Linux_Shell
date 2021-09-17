#! /bin/csh -f

# TEST:
#   [ invalid batch input file ]

set test = 1

echo "MPCS Shell error" > results/${test}.err
touch results/${test}.out
echo 1 > results/${test}.status

./mysh /this/file/is/inaccessible
