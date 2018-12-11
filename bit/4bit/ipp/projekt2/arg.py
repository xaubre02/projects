# # # # # # # # # # # # # # # # # # # # # #
# Predmet: IPP
# Projekt: Minimalizace konecneho automatu
# Autor: Tomas Aubrecht - (xaubre02)
# Skolni rok: 2016/2017
# # # # # # # # # # # # # # # # # # # # # #

import sys

# Trida pro uchovani zadanych prepinacu
# -------------------------------------
# Ve vychozim stavu ma nastaveny False pro vsechny atributy
class Options:
    def __init__(self):
        self.help = False
        self.inFile = False
        self.outFile = False
        self.nonFinish = False
        self.minim = False
        self.insens = False
        self.wht = False
        self.rlo = False
        self.mst = False
        self.mws = False

# Trida pro zpracovani argumentu
# ------------------------------
# Nastavuje priznaky pro jednotlive prepinace, kontroluje jejich spravnost a korektni kombinaci.
# V pripade zadani prepinace --help vypise napovedu a ukonci program s navratovym kodem 0.
# V pripade chyby skonci s navratovym kodem 1.
class Arguments:

    # provede kontrolu argumentu a nastaveni priznaku
    def __init__(self):
        # privatni promenna
        self.__o = Options()
        # prochazi vsechny argumenty krome nulteho(nazev programu) a porovnava je s podporovanymi prepinaci
        # nastavuje priznak, ze byl prepinac zadan, pokud byl zadan opakovane, ukonci program s chybou
        for arg in sys.argv[1:]:

            if arg == "--help":
                if self.__o.help:
                    self.__argsError(arg)

                else:
                    self.__o.help = True

            elif arg[0:8] == "--input=":
                if self.__o.inFile != False:
                    self.__argsError(arg[0:8])

                else:
                    self.__o.inFile = arg[8:]

            elif arg[0:9] == "--output=":
                if self.__o.outFile != False:
                    self.__argsError(arg[0:9])

                else:
                    self.__o.outFile = arg[9:]

            elif arg == "-f" or arg == "--find-non-finishing":
                if self.__o.nonFinish:
                    self.__argsError(arg)

                else:
                    self.__o.nonFinish = True

            elif arg == "-m" or arg == "--minimize":
                if self.__o.minim:
                    self.__argsError(arg)

                else:
                    self.__o.minim = True

            elif arg == "-i" or arg == "--case-insensitive":
                if self.__o.insens:
                    self.__argsError(arg)

                else:
                    self.__o.insens = True

            elif arg == "-r" or arg == "--rules-only":
                if self.__o.rlo:
                    self.__argsError(arg)

                else:
                    self.__o.rlo = True

            elif arg[0:17] == "--analyze-string=":
                if self.__o.mst != False:
                    self.__argsError(arg[0:17])

                else:
                    self.__o.mst = arg[17:]

            elif arg == "--wsfa":
                if self.__o.mws:
                    self.__argsError(arg)

                else:
                    self.__o.mws = True

            else:
                print("Invalid parameter:", arg, file=sys.stderr)
                sys.exit(1)

        # kontrola prepinace --help
        if self.__o.help:
            # pokud byl zadan prepinac --help s jinymi prepinaci, ukonci program s chybou
            if len(sys.argv) > 2:
                print("Parameter '--help' can't be combined with others!", file=sys.stderr)
                sys.exit(1)
            # jinak vypise napovedu a uspesne ukonci program
            else:
                self.__printHelp()
                sys.exit(0)

        # kontrola nepovolenych kombinaci prepinacu
        if (self.__o.minim and self.__o.nonFinish) or ((self.__o.minim or self.__o.nonFinish) and self.__o.mst):
            print("Forbidden combination of parameters!", file=sys.stderr)
            sys.exit(1)

    # vrati objekt uchovavajici informace o zadanych prepinacich
    def getOptions(self):
        return self.__o
        
    # ukonci program s chybou a zpravou na stderr
    def __argsError(self, arg):
        print("Parameter '" + arg + "' typed more than once!", file=sys.stderr)
        sys.exit(1)

    # vytiskne napovedu
    def __printHelp(self):
        print(
            "\n\t*********************************************\n"
            "\t*  IPP - Second Project                     *\n"
            "\t*  Script Minimizing Finite State Machine   *\n"
            "\t*  Author: Tomas Aubrecht (xaubre02)        *\n"
            "\t*  Year: 2016/2017                          *\n"
            "\t*********************************************\n\n"
            "\tParameters:\n"
            "\t-----------\n\n"
            "\t--help\t\t\t\tshow this help\n\n"
            "\t--input=filename\t\tfilename is the name of input file\n\n"
            "\t--output=filename\t\tfilename is the name of output file\n\n"
            "\t-f, --find-non-finishing\tlooks for non-finishing states of well-specified fsm\n\n"
            "\t-m, --minimize\t\t\tperform a minimalization of well-specified fsm\n\n"
            "\t-i, --case-insensitive\t\tdoesn't do a difference between lowercase and uppercase\n"
            )