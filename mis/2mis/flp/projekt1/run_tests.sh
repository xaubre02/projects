#!/bin/bash

TESTCOUNT=21
TESTDIR=tests/
PREFIX=test
SUFFIX=.in
SUCCESSFUL=0

# first 21 tests have to cause an error in simplify-bkg program -> invalid input
for i in {00..20}
do
  cd ${TESTDIR}${PREFIX}$i${SUFFIX} > /dev/null 2>&1
  if [ $? -eq 1 ] ; then
    echo Test '#'$i: OK
    SUCCESSFUL=$((SUCCESSFUL + 1))
  else
    echo Test '#'$i: FAILED
  fi
done

echo ------------------------------
echo Successful tests: $SUCCESSFUL/$TESTCOUNT "("$((TESTCOUNT / SUCCESSFUL * 100))%")"
echo ------------------------------