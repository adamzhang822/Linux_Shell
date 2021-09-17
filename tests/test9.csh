#! /bin/csh -f

# TEST:
#   ls /bin -lat > fout.1 & cal | wc -l & grep inc pr.c > fout.2
#   sleep 2s; mv fout.1 results/fout.1; mv fout.2 results/fout.2; cd results/; pwd

set test = 9 
set tmpFile = fout.1
set tmpFile2 = fout.2
set resultDir = results

echo "ls /bin -lat>${tmpFile} & cal | wc -l & grep inc pr.c>${tmpFile2}" > in.txt
echo "sleep 2s; mv ${tmpFile} ${resultDir}; mv ${tmpFile2} ${resultDir}; cd ${resultDir}; pwd" >> in.txt

rm -f ${resultDir}/${tmpFile}
rm -f ${resultDir}/${tmpFile2}
rm -f ${resultDir}/${test}.${tmpFile}
rm -f ${resultDir}/${test}.${tmpFile2}

echo "ls /bin -lat>${tmpFile} & cal | wc -l & grep inc pr.c>${tmpFile2}" > ${resultDir}/${test}.out
cal | wc -l >> ${resultDir}/${test}.out
ls /bin -lat > ${resultDir}/${test}.${tmpFile}
grep inc pr.c > ${resultDir}/${test}.${tmpFile2}
echo "sleep 2s; mv ${tmpFile} ${resultDir}; mv ${tmpFile2} ${resultDir}; cd ${resultDir}; pwd" >> ${resultDir}/${test}.out
readlink -f results >> ${resultDir}/${test}.out

rm -f ${resultDir}/${test}.err
touch ${resultDir}/${test}.err

echo 0 > ${resultDir}/${test}.status

./mysh in.txt
