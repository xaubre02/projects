# --------------------------------
# Předmět: PRL 2018/2019
# Projekt: Výpočet úrovně vrcholu
# Datum:   2019-04-14
# Autor:   Tomáš Aubrecht
# Login:   xaubre02
# --------------------------------

if [ $# != 1 ]; then
    echo "Chyba: neplatny pocet argumentu"
    echo "Pouziti: test.sh retez"
    exit 1
fi

# počet procesorů
procs=`python -c "print(len('$1') * 2 - 2);"`

# překlad a spuštění
mpic++ --prefix /usr/local/share/OpenMPI vuv.cpp -o vuv
mpirun --prefix /usr/local/share/OpenMPI -np $procs vuv

# vyčištění
rm -rf vuv
