#! /bin/csh -f

# TEST:
#   (empty)  

set test = 2

touch results/${test}.out
touch results/${test}.err
echo 0 > results/${test}.status

rm -f in.txt
touch in.txt # create a new empty file
./mysh in.txt
