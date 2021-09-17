#! /bin/csh -f

# TEST:
#   ;;;;;;

set test = 15

echo ";;;;;;" > in.txt

echo ";;;;;;" > results/${test}.out

rm -f results/${test}.err
touch results/${test}.err

echo 0 > results/${test}.status

./mysh in.txt
