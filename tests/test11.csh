#! /bin/csh -f

# TEST: 
#   cal | sort & non_existing_command

set test = 11 
set resultDir = results

echo "cal | sort & non_existing_command" > in.txt

echo "cal | sort & non_existing_command" > ${resultDir}/${test}.out
cal | sort >> ${resultDir}/${test}.out

echo "MPCS Shell error" > ${resultDir}/${test}.err

echo 0 > ${resultDir}/${test}.status

./mysh in.txt
