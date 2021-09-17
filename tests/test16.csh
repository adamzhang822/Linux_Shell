#! /bin/csh -f

# TEST:
#   cat run-tests.py ; pwd ; who

set test = 16

echo "cat run-tests.py  ;;; pwd ;;; who;;;" > in.txt

echo "cat run-tests.py  ;;; pwd ;;; who;;;" > results/${test}.out
cat run-tests.py >> results/${test}.out
pwd >> results/${test}.out
who  >> results/${test}.out

rm -f results/${test}.err
touch results/${test}.err

echo 0 > results/${test}.status

./mysh in.txt
