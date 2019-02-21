#!/bin/bash

TESTCOUNT=30
TESTDIR=tests/
PREFIX=test
SUFFIX=.in
SUCCESSFUL=0

# first 21 tests have to cause an error in simplify-bkg program -> invalid input
for i in {00..29}
do
  ./simplify-bkg -i ${TESTDIR}${PREFIX}$i${SUFFIX} > /dev/null 2>&1
  if [ $? -eq 1 ] ; then
    echo Test '#'$i: OK
    SUCCESSFUL=$((SUCCESSFUL + 1))
  else
    echo Test '#'$i: FAILED
  fi
done

echo -----------------------
echo Successful tests: $SUCCESSFUL/$TESTCOUNT
echo -----------------------