# -----------------------
# Předmět: PRL 2018/2019
# Projekt: Bucket sort
# Datum:   2019-03-07
# Autor:   Tomáš Aubrecht
# Login:   xaubre02
# -----------------------

if [ $# != 1 ]; then
    echo "Chyba: neplatny pocet argumentu"
    echo "Pouziti: test.sh pocet_hodnot"
    exit 1
fi

# počet hodnot
values=$1
if [ $values -lt 1 ]; then
    echo "Chyba: neplatny pocet hodnot - musi byt vetsi nez 0"
    exit 1
fi

# počet procesorů
if [ $values -eq 1 ]; then
    procs=1
else
    procs=`echo "2 * l($values) / l(2) - 1" | bc -l`
    # zaokrouhlení nahoru
    procs=`echo $procs | awk '{print ($0-int($0)>0)?int($0)+1:int($0)}'`
fi

# vytvoření souboru numbers
dd if=/dev/random bs=1 count=$values of=numbers > /dev/null 2>&1

# překlad a spuštění
#mpicc --prefix /usr/local/share/OpenMPI bks.cpp -o bks -lm
#mpirun --prefix /usr/local/share/OpenMPI -np $processors bks

# vyčištění
rm -rf bks numbers
