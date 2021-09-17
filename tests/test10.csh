#! /bin/csh -f

# TEST: 
#   date > results/25.fout.1 & ls $HOME & calc > /not/a/file

set test = 10 
set tmpFile = fout.1
set resultDir = results

echo "date > ${resultDir}/${tmpFile} & ls $HOME & calc > /not/a/file" > in.txt

rm -f ${resultDir}/${tmpFile}
rm -f ${resultDir}/${test}.${tmpFile}

echo "date > ${resultDir}/${tmpFile} & ls $HOME & calc > /not/a/file" > ${resultDir}/${test}.out
date > ${resultDir}/${test}.${tmpFile}
ls $HOME >> ${resultDir}/${test}.out

echo "MPCS Shell error" > ${resultDir}/${test}.err

echo 0 > ${resultDir}/${test}.status

./mysh in.txt
