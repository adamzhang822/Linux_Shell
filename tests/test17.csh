#! /bin/csh -f

# TEST:
#   find /bin -iname 'ps' > out; ls -la /bin | sort | wc -l; quit

set test = 17 
set tmpFile = fout.1
set resultDir = results

echo "find /bin -name ps>${resultDir}/${tmpFile};ls -l /bin | sort | wc -l;quit" > in.txt

rm -f ${resultDir}/${tmpFile}
rm -f ${resultDir}/${test}.${tmpFile}

echo "find /bin -name ps>${resultDir}/${tmpFile};ls -l /bin | sort | wc -l;quit" > ${resultDir}/${test}.out
find /bin -name 'ps' > ${resultDir}/${test}.${tmpFile}
ls -l /bin | sort | wc -l >> ${resultDir}/${test}.out

rm -f ${resultDir}/${test}.err
touch ${resultDir}/${test}.err

echo 0 > ${resultDir}/${test}.status

./mysh in.txt
