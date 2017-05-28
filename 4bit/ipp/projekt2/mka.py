#!/usr/bin/python3

# # # # # # # # # # # # # # # # # # # # # #
# Predmet: IPP
# Projekt: Minimalizace konecneho automatu
# Autor: Tomas Aubrecht - (xaubre02)
# Skolni rok: 2016/2017
# # # # # # # # # # # # # # # # # # # # # #

from arg import *
from parser import *

# Otevre soubor name a precte jeho obsah, ktery je navracen
# implicitne: stdin
def getContent(name):
    if name == False:
        return sys.stdin.read()

    try:
        f = open(name, 'r')
    except IOError:
        print("Unable to open the input file!", file=sys.stderr)
        sys.exit(2)

    return f.read()

# Zapise content do souboru name
# implicitne: stdout
def saveContent(name, content):
    if name == False:
        sys.stdout.write(content)
        return

    try:
        f = open(name, 'w+')
    except IOError:
        print("Unable to write into the output file!", file=sys.stderr)
        sys.exit(3)

    f.write(content)

######################
### HLAVNI PROGRAM ###  

# nacteni argumentu
arg = Arguments()
# ziskani konfigurace
option = arg.getOptions()
# inicializace parseru a analyzace vstupnich dat
parser = Parser()
parser.analyze(getContent(option.inFile), option.insens, option.rlo)
# ziskani automatu od parseru
fsm = parser.getAutomaton()
# kontrola, zdali je automat DKSA a pripadne jeho prevod na DKSA
ret = fsm.makeWellSpecifiedFA(option.mws, option.insens, option.nonFinish)
# kontrola, zdali byl zpracovan parametr -f
if ret != -1:
    saveContent(option.outFile, ret.__str__())
    sys.exit(0)

# minimalizace automatu, pokud je to pozadovano ve vstupni konfiguraci
fsm.minimize(option.minim)

# rozsireni mst - analyza retezce
ret = fsm.analyzeString(option.mst)
if ret == 0 or ret == 1:
    saveContent(option.outFile, ret.__str__())
    sys.exit(0)

# ulozeni vysledneho automatu na vystup
saveContent(option.outFile, fsm.__str__())
