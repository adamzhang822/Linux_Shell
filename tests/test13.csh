#! /bin/csh -f

# TEST:
#   echo "very long line...."
#   cal

set test = 13
set resultDir = results

echo "Justcheckingifyoureportanerrorformorethan64characters.Loremipsumdolorsitamet,consecteturadipiscingelit,seddoeiusmodtemporincididuntutlaboreetdoloremagnaaliqua.Utenimadminimveniam,quisnostrudexercitationullamcolaborisnisiutaliquipexeacommodoconsequat." > in.txt
echo "cal" >> in.txt

echo "Justcheckingifyoureportanerrorformorethan64characters.Loremipsumdolorsitamet,consecteturadipiscingelit,seddoeiusmodtemporincididuntutlaboreetdoloremagnaaliqua.Utenimadminimveniam,quisnostrudexercitationullamcolaborisnisiutaliquipexeacommodoconsequat." > ${resultDir}/${test}.out
echo "cal" >> ${resultDir}/${test}.out
cal >> ${resultDir}/${test}.out

echo "MPCS Shell error" > results/${test}.err

echo 0 > results/${test}.status

./mysh in.txt
