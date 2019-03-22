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
    # počet listových procesorů je mocnina 2
    procs=`python -c "from math import ceil, log, sqrt; m=pow(2,ceil(log(log($values,2),2))); print(int(2*m-1));"`
fi

# vytvoření souboru numbers
dd if=/dev/random bs=1 count=$values of=numbers > /dev/null 2>&1

# překlad a spuštění
mpic++ --prefix /usr/local/share/OpenMPI bks.cpp -o bks
mpirun --prefix /usr/local/share/OpenMPI -np $procs bks

# vyčištění
rm -rf bks numbers
