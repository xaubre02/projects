# # # # # # # # # # # # # # # # # # # # # #
# Predmet: IPP
# Projekt: Minimalizace konecneho automatu
# Autor: Tomas Aubrecht - (xaubre02)
# Skolni rok: 2016/2017
# # # # # # # # # # # # # # # # # # # # # #

import sys
from fsm import *

# Trida pro zpracovani vstupnich dat
# ----------------------------------
# Odstrani ze vstupniho souboru bile znaky a zkontroluje spravnost jeho formatu.
# 
class Parser:

    # vytvori prazdny automat
    def __init__(self):
        self.__fsm = FSM()

    # analyzuje vstupni data a naplni automat
    def analyze(self, data, case, rulesOnly):
        data = self.__removeComments(data)
        data = self.__removeWhiteSpaces(data)
        data = self.__changeCase(data, case)
        if rulesOnly:
            self.__parseRules(data)

        else:
            self.__parse(data)
            self.__fillAutomaton()

        self.__checkSyntax()
        self.__checkSemantics()

    # vrati automat ziskany ze vstupu
    def getAutomaton(self):
        return self.__fsm

    # Odstrani vsechny komentare z daneho stringu
    def __removeComments(self, data):
        # docasne nahrazeni symbolu konce radku a # 
        data = data.replace("'\n'", "'linefeed'")
        data = data.replace("'#'", "'hashtag'")
        
        clean = ""
        # prochazi vsechny radky vstupnich dat
        for line in data.splitlines():
            start = line.find("#")
            # pokud na radku neni komentar nalezen, uloz radek a pokracuj
            if start == -1:
                clean += line
                continue

            # vymaze komentar a ulozi zbytek
            line = line.replace(line[start: ], "")
            clean += line

        # obnoveni symbolu #
        clean = clean.replace("'hashtag'", "'#'")

        return clean

    # odstraneni bilych znaku
    def __removeWhiteSpaces(self, data):
        # nahrazeni bilych znaku, ktere jsou vstupnimi symboly
        data = data.replace("' '",  "'space'")
        data = data.replace("'\t'", "'hortab'")
        data = data.replace("'\v'", "'vertab'")
        data = data.replace("'\r'", "'carriage'")

        # odstraneni vsech bilych znaku
        data = data.replace(" ",  "")
        data = data.replace("\t", "")
        data = data.replace("\v", "")
        data = data.replace("\r", "")
        data = data.replace("\n", "")

        # obnoveni pouze bilych znaku u symbolu
        data = data.replace("'space'", "' '")
        data = data.replace("'hortab'", "'\t'")
        data = data.replace("'vertab'", "'\v'")
        data = data.replace("'carriage'", "'\r'")
        data = data.replace("'linefeed'", "'\n'")

        return data
    
    # pokud je pozadavek na case-insensitive, prevede na lowercase
    def __changeCase(self, data, case):
        if case:
            return data.lower()
        else:
            return data

    # analyzuje vstupni data
    def __parse(self, data):
        # kontrola pritomnosti zapouzdrovacich zavorek
        if data[0 : 1] != "(":
            self.__exitError("syn", "Missing '('!")

        if data[len(data) - 1 : len(data)] != ")":
            self.__exitError("syn", "Missing ')'!")

        # oriznuti zapouzdrovacich zavorek
        data = data[1 : len(data) - 1]
        # kontrola existence slozenych zavorek
        end = self.__checkBrackets(data)
        # ulozeni obsahu slozenych zavorek a zkraceni o ulozeny obsah
        self.stateStr = data[1 : end]
        data = data[end + 2:]            
        # kontrola existence slozenych zavorek
        end = self.__checkBrackets(data)
        # ulozeni obsahu slozenych zavorek a zkraceni o ulozeny obsah
        self.symbolStr = data[1 : end]
        data = data[end + 2:]
        # kontrola existence slozenych zavorek
        end = self.__checkBrackets(data)
        # ulozeni obsahu slozenych zavorek a zkraceni o ulozeny obsah
        self.ruleStr = data[1 : end]
        data = data[end + 2:]
        # kontrola existence carky u pocatecniho stavu
        end = data.find(",")
        if end == -1:
            self.__exitError("syn")
        # ulozeni pocatecniho stavu a zkraceni
        self.startStr = data[0 : end]
        data = data[end + 1:]
        # kontrola poslednich slozenych zavorek
        if data[0 : 1] != "{":
            self.__exitError("syn")

        if data[len(data) - 1 : len(data)] != "}":
            self.__exitError("syn")

        self.finalStr = data[1 :  len(data) - 1]

    # zkontroluje ohranicujici zavorky
    def __checkBrackets(self, data):
        if data[0 : 1] != "{":
            self.__exitError("syn")

        end = data.find("},")
        if end == -1:
            self.__exitError("syn")

        return end
    
    # provede zpracovani vstupnich dat obsahujici pouze zkraceny zapis pravidel
    def __parseRules(self, string):
        # docasne odstraneni carky
        string = string.replace("','", "'comma'")

        while True:
            if len(string) == 0:
                break

            # jedna se o posledni pravidlo
            end = string.find(",")
            if end == -1:
                self.__notInArray(self.__fsm.rules, string[0:])
                break

            self.__notInArray(self.__fsm.rules, string[0 : end])
                
            # zkraceni retezce o pridanou polozku
            string = string[end + 1:]

        # nastaveni pocatecniho stavu = vychozi stav prvniho pravidla
        self.__fsm.start = self.__fsm.rules[0].left

        # naplneni zbyvajicich komponent automatu
        for rule in self.__fsm.rules:
            # pokud je na konci cilove stavu tecka, jedna se o koncovy stav
            if rule.right[len(rule.right) - 1:] == ".":
                # odstraneni tecky
                rule.right = rule.right[0 : len(rule.right) - 1]
                if rule.right not in self.__fsm.final:
                    self.__fsm.final.append(rule.right)
                
                # pridani stavu do mnoziny stavu (bez tecky)
                if rule.right not in self.__fsm.states:
                    self.__fsm.states.append(rule.right)

            else:
                # pridani ciloveho stavu do mnoziny stavu
                if rule.right not in self.__fsm.states:
                    self.__fsm.states.append(rule.right)

            # pridani vychoziho stavu do mnoziny stavu
            if rule.left not in self.__fsm.states:
                self.__fsm.states.append(rule.left)

            # pridani symbolu do vstupni abecedy
            if rule.symbol not in self.__fsm.symbols:
                self.__fsm.symbols.append(rule.symbol)

    # prevede vsechny stringy automatu do poli
    def __fillAutomaton(self):
        self.__fsm.states  = self.__makeArray(self.stateStr)
        self.__fsm.symbols = self.__makeArray(self.symbolStr, 1)
        self.__fsm.rules   = self.__makeArray(self.ruleStr, 2)
        self.__fsm.start   = self.startStr
        self.__fsm.final   = self.__makeArray(self.finalStr)

    # prevede retezec do pole a zaroven castecne zkontroluje syntax
    # mode 0 - pro prevod stavu
    # mode 1 - pro prevod podporujici carku - symboly
    # mode 2 - pro prevod pravidel
    def __makeArray(self, string, mode = 0):
        # pokud je tam carka navic
        if string[len(string) - 1 : len(string)] == ",":
            self.__exitError("syn", "Missing item!")

        array = []
        # docasne odstraneni carky
        if mode == 2:
            string = string.replace("','", "'comma'")

        while True:
            if len(string) == 0:
                return array

            # rozparsovani s moznosti vyskytu carky
            if mode == 1 and len(string) >= 3:
                if string[0:3] == "','":
                    if string[0:3] not in array:
                        array.append(string[0:3])
                    # zkraceni stringu
                    if len(string) >= 4:
                        string = string[4:]
                    else:
                        string = string[3:]

                    continue

            end = string.find(",")
            if end == -1:
                # kontrola, zdali polozka uz neni obsazena v poli
                if mode == 2:
                    array = self.__notInArray(array, string[0:])
                
                else:
                    if string[0:] not in array:
                        array.append(string[0:])
                # ukonceni cyklu
                break

            # kontrola, zdali polozka uz neni obsazena v poli
            if mode == 2:
                array = self.__notInArray(array, string[0 : end])
            
            else:
                if string[0 : end] not in array:
                    array.append(string[0 : end])

            # zkraceni retezce o pridanou polozku
            string = string[end + 1:]

        return array

    # vlastni funkce pro kontrolu existence pravidla v poli
    def __notInArray(self, array, strPart):
        # obnoveni carky
        strPart = strPart.replace("'comma'", "','")

        if len(strPart) == 0:
            return array;

        count = 0
        for rule in array:
            if strPart == rule.strNoSpaces():
                count += 1
        # pravidlo neni obsazeno v poli
        if count == 0:
            array.append(Rule(strPart))

        return array

    # zkontroluje, zdali je automat syntakticky korektni
    def __checkSyntax(self):
        # kontrola jmen stavu
        for state in self.__fsm.states:
            if self.__validName(state) == False:
                self.__exitError("syn", "Invalid state name!")

        # kontrola jmen koncovych stavu
        for state in self.__fsm.final:
            if self.__validName(state) == False:
                self.__exitError("syn", "Invalid state name!")

        # kontrola jmena pocatecniho stavu
        if self.__validName(self.__fsm.start) == False:
                self.__exitError("syn", "Invalid state name!")

        # kontrola symbolu ve vstupni abecede
        for symbol in self.__fsm.symbols:
            if len(symbol) != 3:
                if symbol != "''''" and symbol and symbol != "'\\n'" and symbol != "'\\t'":
                    self.__exitError("syn", "Symbol must be one char in ''!")
            
            if len(symbol) == 3 and symbol[0] != "'" and symbol[2] != "'":
                self.__exitError("syn", "Symbol must be one char in ''!")

        for rule in self.__fsm.rules:
            # kontrola jmena stavu v pravidle
            if self.__validName(rule.left) == False or self.__validName(rule.right) == False:
                self.__exitError("syn", "Invalid state name!")
            # kotrola symbolu v pravidle
            if len(rule.symbol) != 3:
                if rule.symbol != "''''" and rule.symbol != "''" and rule.symbol != "'\\n'" and rule.symbol != "'\\t'":
                    self.__exitError("syn", "Symbol must be one char in ''!")
            
            if len(rule.symbol) == 3 and rule.symbol[0] != "'" and rule.symbol[2] != "'":
                self.__exitError("syn", "Symbol must be one char in ''!")

    # zkontroluje validitu jmena
    def __validName(self, name):
        if name == "":
            return False

        # musi obsahovat pouze pismena, cislice a podtrzitko
        for char in name:
            if char.isnumeric() == False and char.isalpha() == False and char != '_':
                return False

        # nesmi zacinat cislici a nesmi zacinat ani koncit podtrzitkem 
        if name[0 : 1].isnumeric() or name[0 : 1] == '_' or name[len(name) - 1:] == '_':
            return False

        return True

    # zkontroluje, zdali je automat semanticky korektni
    def __checkSemantics(self):
        # kontrola, zdali neni vstupni abeceda prazdna
        if len(self.__fsm.symbols) == 0:
            self.__exitError("sem", "Input alphabet is empty!")

        if self.__fsm.symbols[0] == "" and len(self.__fsm.symbols) == 1:
            self.__exitError("sem", "Input alphabet is empty!")

        # kontrola existence stavu a symbolu u pravidel
        for rule in self.__fsm.rules:
            if rule.left not in self.__fsm.states or rule.right not in self.__fsm.states:
                self.__exitError("sem", "State in a rule is not included in States!")

            # symbol musi byt v abecede - prazdny retezec neni symbol
            if rule.symbol not in self.__fsm.symbols and rule.symbol != "''":
                self.__exitError("sem", "Symbol in a rule is not included in States!")

        # kontrola, zdali pocatecni stav patri do mnoziny stavu
        if self.__fsm.start not in self.__fsm.states:
            self.__exitError("sem", "Start state is not included in States!")

        # kontrola, zdali mnozina koncovych stavu je podmnozinou stavu
        for state in self.__fsm.final:
            if state not in self.__fsm.states:
                self.__exitError("sem", "Final states are not subset of States!")

    # ukonci program s chybou a zpravou podle priznaku mode
    def __exitError(self, mode, msg = "Invalid Format!"):
        if mode == "syn":
            print("Syntax error:", msg, file=sys.stderr)
            sys.exit(60)

        elif mode == "sem":
            print("Semantics error:", msg, file=sys.stderr)
            sys.exit(61)
