#! /bin/csh -f

# TEST:
#   (empty)

set test = 3

touch results/${test}.err
echo "" > results/${test}.out
echo 0 > results/${test}.status

echo "" > in.txt
./mysh in.txt
