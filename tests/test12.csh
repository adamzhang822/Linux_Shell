#! /bin/bash -f

# TEST: 
#   ls -la | sort | uniq | wc -l; less

test="12"
resultDir="results"

echo "ls -la /bin | sort | uniq | wc -l; less" > in.txt

rm -f ${resultDir}/${test}.out
echo "ls -la /bin | sort | uniq | wc -l; less" > ${resultDir}/${test}.out
ls -la /bin | sort | uniq | wc -l >> ${resultDir}/${test}.out
less 2> ${resultDir}/${test}.err

echo 0 > ${resultDir}/${test}.status

./mysh in.txt
