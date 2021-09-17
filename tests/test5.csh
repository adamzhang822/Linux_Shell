#! /bin/csh -f

# TEST:
#   exit

set test = 5

echo "quit" > in.txt

echo "quit" > results/${test}.out

touch results/${test}.err

echo 0 > results/${test}.status

./mysh in.txt
 
