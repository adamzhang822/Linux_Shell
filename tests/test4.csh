#! /bin/csh -f

# TEST:
#   pwd /invalid/path

set test = 4

echo "pwd /my/favourite/dir" > in.txt

echo "pwd /my/favourite/dir" > results/${test}.out

echo "MPCS Shell error" > results/${test}.err

echo 0 > results/${test}.status

./mysh in.txt
