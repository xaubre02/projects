# -----------------------
# Předmět: PRL 2018/2019
# Projekt: Bucket sort
# Datum:   2019-03-07
# Autor:   Tomáš Aubrecht
# Login:   xaubre02
# -----------------------

if [ $# != 2 ]; then
    echo "Chyba: neplatny pocet argumentu"
    echo "Pouziti: test.sh pocet_hodnot"
    exit 1
fi

values=$1
processors=$((2*$values-1))
som=`echo "scale=2 ; l($numbers) / l(2)" | bc -l`	

echo $som

# vytvoření souboru numbers
#dd if=/dev/random bs=1 count=$values of=numbers > /dev/null 2>&1

# překlad a spuštění
#mpicc --prefix /usr/local/share/OpenMPI bks.cpp -o bks -lm
#mpirun --prefix /usr/local/share/OpenMPI -np $processors bks

# vyčištění
#rm -rf bks numbers