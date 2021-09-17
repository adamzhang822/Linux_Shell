#! /bin/csh -f

# TEST:
#   cd
#   pwd

set test = 6

echo "cd" > in.txt
echo "pwd" >> in.txt

echo "cd" > results/${test}.out
echo "pwd" >> results/${test}.out
echo $HOME >> results/${test}.out

rm -f results/${test}.err
touch results/${test}.err

echo 0 > results/${test}.status

./mysh in.txt
