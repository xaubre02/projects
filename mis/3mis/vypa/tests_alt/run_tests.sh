#!/bin/sh

# name of the compiler script/binary
NAME=./vypcomp
# test files
LEX=$(find ./tests_alt/lex -type f -name 'lex*' | wc -l)
SYN=$(find ./tests_alt/syn -type f -name 'syn*' | wc -l)
SEM_UNDEF=$(find ./tests_alt/semUndef -type f -name 'semUndef*' | wc -l)
SEM_REDEF=$(find ./tests_alt/semRedef -type f -name 'semRedef*' | wc -l)
SEM_OTHER=$(find ./tests_alt/semOther -type f -name 'semOther*' | wc -l)
SEM_TYPES=$(find ./tests_alt/semTypes -type f -name 'semTypes*' | wc -l)

#Spuštění lexikálních testů
echo ""
echo "Lexikální analyzátor"
echo "********************"

for i in $( seq $LEX )
do
	$NAME ./tests_alt/lex/lex$i >/dev/null 2>/dev/null
	RETURN=$(echo $?)

	if [ $RETURN -eq 11 ]; then
		printf "Test #%02d: OK\n" $i
	else
		printf "Test #%02d: Failed!\n" $i
	fi
done

#Spuštění syntaktických testů
echo ""
echo "Syntaktický analyzátor"
echo "**********************"

for i in $( seq $SYN )
do
        $NAME ./tests_alt/syn/syn$i >/dev/null 2>/dev/null
        RETURN=$(echo $?)

        if [ $RETURN -eq 12 ]; then
                printf "Test #%02d: OK\n" $i
        else
                printf "Test #%02d: Failed!\n" $i
        fi
done

#Running semantic tests for checking definitions
echo ""
echo "Semantic analyzer - (un)definitions"
echo "***********************************"

for i in $( seq $SEM_UNDEF )
do
        $NAME ./tests_alt/semUndef/semUndef$i >/dev/null 2>/dev/null
        RETURN=$(echo $?)

        if [ $RETURN -eq 14 ]; then
                printf "Test #%02d: OK\n" $i
        else
                printf "Test #%02d: Failed!\n" $i
        fi
done

#Running semantic tests for checking redefinitions
echo ""
echo "Semantic analyzer - redefinitions"
echo "*********************************"

for i in $( seq $SEM_REDEF )
do
        $NAME ./tests_alt/semRedef/semRedef$i >/dev/null 2>/dev/null
        RETURN=$(echo $?)

        if [ $RETURN -eq 14 ]; then
                printf "Test #%02d: OK\n" $i
        else
                printf "Test #%02d: Failed!\n" $i
        fi
done


#Running semantic tests for other checks - e.g. number of parameters of function/method
echo ""
echo "Semantic analyzer - other checks"
echo "********************************"

for i in $( seq $SEM_OTHER )
do
        $NAME ./tests_alt/semOther/semOther$i >/dev/null 2>/dev/null
        RETURN=$(echo $?)

        if [ $RETURN -eq 14 ]; then
                printf "Test #%02d: OK\n" $i
        else
                printf "Test #%02d: Failed!\n" $i
        fi
done

#Running semantic tests for all types compatibility checks
echo ""
echo "Semantic analyzer - checks of type compatibility"
echo "************************************************"

for i in $( seq $SEM_TYPES )
do
        $NAME ./tests_alt/semTypes/semTypes$i >/dev/null 2>/dev/null
        RETURN=$(echo $?)

        if [ $RETURN -eq 13 ]; then
                printf "Test #%02d: OK\n" $i
        else
                printf "Test #%02d: Failed!\n" $i
        fi
done
