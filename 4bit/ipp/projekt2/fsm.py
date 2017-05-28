# # # # # # # # # # # # # # # # # # # # # #
# Predmet: IPP
# Projekt: Minimalizace konecneho automatu
# Autor: Tomas Aubrecht - (xaubre02)
# Skolni rok: 2016/2017
# # # # # # # # # # # # # # # # # # # # # #

import sys

# Trida reprezentujici pravidlo prechodu
# --------------------------------------
# Atributy: left ..... vychozi levy stav
#           symbol ... symbol vstupni abecedy
#           right .... cilovy pravy stav
class Rule:
    # inicializuje objekt a zaroven zkontroluje syntaxi pravidla
    def __init__(self, string):
        # kontrola pritomnosti sipky a jeji nasledne odstraneni
        self.__findSymbol(string, "->")
        string = string.replace("->", "")
        
        # kontrola, zdali symbol neni apostrof
        start = string.find("''''")
        if start != -1:
            self.left = string[0 : start]
            self.symbol = "''''"
            self.right = string[start + 4:]

        else:
            # kontrola pritomnosti prvniho apostrofu
            start = self.__findSymbol(string, "'")
            # kontrola pritomnosti druheho apostrofu
            end = self.__findSymbol(string, "'", start + 1) + 1
            self.left = string[0 : start]
            self.symbol = string[start : end]
            self.right = string[end:]

        if self.left == "" or self.right == "":
            sys.stderr.write("Syntax error: Invalid Rule!\n")
            sys.exit(60)
    # funkce pro prevod objektu do retezce
    def __str__(self):
        return self.left + " " + self.symbol + " -> " + self.right

    # funkce pro prevod objektu do retezce bez mezer
    def strNoSpaces(self):
        return self.left + self.symbol + "->" + self.right
    
    # porovna, zdali jsou pravidla shodna
    def equal(self, rule):
        return (self.left == rule.left and
               self.right == rule.right and
               self.symbol == rule.symbol) 

    # najde v retezci symbol od dane pozice a vrati jeho polohu nebo skonci s chybou
    def __findSymbol(self, string, symbol, pos = 0):
        index = string.find(symbol, pos)
        if index == -1:
            sys.stderr.write("Syntax error: Invalid rule!\n")
            sys.exit(60)
        return index

    # vrati levou cast pravidla
    def leftSide(self):
        return self.left + " " + self.symbol

# Trida reprezentujici automat
# ----------------------------
# Atributy: states .... mnozina vsech stavu
#           symbols ... symbol vstupni abecedy
#           rules ..... mnozina vsech pravidel
#           start ..... pocatecni stav
#           final ..... mnozina koncovych stavu
class FSM:
    # inicializuje atributy automatu
    def __init__(self):
        self.states = []
        self.symbols = []
        self.rules = []
        self.start = None
        self.final = []

    # funkce pro prevod automatu do formatu pro tisk
    def __str__(self):
        result = "(\n"
        result += self.__appendArray(self.states)
        result += self.__appendArray(self.symbols)
        result += self.__appendRules()
        result += self.start + ",\n"
        result += self.__appendArray(self.final)
        # odstraneni posledni carky
        result = result[0 : len(result) - 2]
        result += "\n)\n"
        return result

    # vrati serazene pole obalene slozenymi zavorkami
    def __appendArray(self, array):
        array.sort()
        string = "{"
        for item in array:
            string += item + ", "
        # odstraneni carky a mezery za posledni polozkou
        string = string[0 : len(string) - 2]
        string += "},\n"
        return string
    
    # vrati pravidla obalena slozenymi zavorkami
    def __appendRules(self):
        # serazeni podle vychoziho stavu a dale pak podle symbolu
        self.rules.sort(key = lambda r: (r.left, r.symbol))
        string = "{\n"
        for rule in self.rules:
            string += str(rule) + ",\n"
        # odstraneni carky a odradkovani za posledni polozkou
        string = string[0 : len(string) - 2]
        string += "\n},\n"
        return string

    # kontrola, zdali se jedna o dobre specifikovany konecny automat
    # zaroven zpracuje parametr -f nebo --find-non-finishing
    # pokud je zadan parametr -wsfa (make) prevede automat na DKSA
    def makeWellSpecifiedFA(self, make, case, nonFinish):
        # ukonci program s chybou a zpravou msg
        def exitError(msg):
            print("WSFA error:", msg, file=sys.stderr)
            sys.exit(62)

        # kontrola epsilon prechodu
        if not self.__isWithoutEpsilon():
            exitError("Automaton contains an epsilon move!")

        # kontrola, zdali je automat deterministicky
        if not self.__isDeterministic(False):
            exitError("Automaton is not deterministic!")

        # ziska vsechny nedostupne a neukoncujici stavy
        inaccessible = self.__getInaccessible()
        nonTerminating = self.__getNonTerminating()

        # rozsireni - prevede automat na DSKA
        if make:
            # kontrola poctu nedostupnych stavu
            if len(inaccessible) > 0:
                # odstrani tyto nedostupne stavy
                self.__removeStates(inaccessible)

            # kontrola poctu neukoncujicich stavu
            if len(nonTerminating) > 1:
                # odstrani tyto neukoncujici stavy
                self.__removeStates(nonTerminating)

            # kontrola, zdali je automat uplny DKA
            if not self.__isDeterministic(True):
                # muze byt pouze jeden neukoncujici stav - qFALSE
                self.__removeStates(nonTerminating)
                self.__makeCompleteFA(case)
                # znovunacteni neukoncujicich stavu (qFALSE)
                nonTerminating = self.__getNonTerminating()

        else:
            # kontrola poctu nedostupnych stavu
            if len(inaccessible) > 0:
                exitError("Automaton has an inaccessible state!")

            # kontrola poctu neukoncujicich stavu
            if len(nonTerminating) > 1:
                exitError("Automaton has more than 1 non-terminating state!")

            # kontrola, zdali je automat uplny DKA
            if not self.__isDeterministic(True):
                exitError("Automaton is not Complete FA!")

        # vypise neukoncujici stav nebo 0, pokud takovy stav neni
        if nonFinish:
            if len(nonTerminating) == 0:
                return 0

            else:
                return nonTerminating[0]
            
        return -1

    # kontrola, zdali je automat bez epsilon prechodu
    def __isWithoutEpsilon(self):
        # prochazi vsechny pravidla a kontroluje je
        for rule in self.rules:
            if rule.symbol == "''":
                return False

        return True

    # kontrola, zdali je automat uplny DKA
    # pro kazde pa -> q z R plati: R - {pa -> q} neobsahuje pa ->*
    # prochazi vsechny pravidla a kazdou pocatecni cast porovnava se vsemi pravidly - musi se vyskytovat pouze jednou
    def __isDeterministic(self, complete):
        # prochazi vsechny stavy a kontroluje, jestli kazdy stav ma od kazdeho symbolu jedno pravidlo
        for state in self.states:
            for symbol in self.symbols:
                count = 0
                # pokud je nalezeno pravidlo odpovidajici stavu a symbolu, inkrementuje se hodnota count
                for rule in self.rules:
                    if state + " " + symbol  == rule.leftSide():
                        count += 1
                
                # uplny DKA se nesmi zaseknout - muze prejit do prave jedne dalsi konfigurace
                if complete and count != 1:
                    return False
                # deterministicky automat muze prejit maximalne do jedne dalsi konfigurace
                if not complete and count > 1:
                    return False

        return True

    # zjistuje, zdali je stav dostupny
    # prochazi vsechna pravidla a porovnava jejich pravou stranu se vstupnim stavem, pokud jsou shodne, zjistuje zdali je leva strana dostupnu
    def __isAccesible(self, state, checked):    
        for rule in self.rules:
            # pokud uz jsem pravidlo prochazel, pokracuj
            if rule in checked:
                continue

            # pokud se rovna jejich prava strana
            if state == rule.right:
                checked.append(rule)
                # pokud je leva strana pocatecni stav, tak je stav dostupny
                if rule.left == self.start:
                    return True
                # rekurzivne kontroluje, zdali je leva strana dostupnym stavem
                if self.__isAccesible(rule.left, checked):
                    return True

        return False

    # spocita a vrati pocet vsech nedostupnych stavu
    def __getInaccessible(self):
        inacc = []
        for state in self.states:
            # pomocne pole pro uchovani zkontrolovanych pravidel - kvuli deadlocku
            checked = []
            if state == self.start:
                continue
            if self.__isAccesible(state, checked):
                continue
            # ulozeni nedostupneho stavu
            inacc.append(state)

        return inacc

    # zjistuje, zdali je stav ukoncujici
    # prochazi vsechna pravidla a porovnava jejich levou stranu se vstupnim stavem, pokud jsou shodne, zjistuje zdali je prava strana ukoncujici
    def __isTerminating(self, state, checked):
        for rule in self.rules:
            # pokud uz jsem pravidlo prochazel, pokracuj
            if rule in checked:
                continue

            # pokud se rovna jejich leva strana
            if state == rule.left:
                checked.append(rule)
                # pokud je prava strana koncovy stav, tak je stav ukoncujici
                if rule.right in self.final:
                    return True
                # rekurzivne kontroluje, zdali je prava strana ukoncujicim stavem
                if self.__isTerminating(rule.right, checked):
                    return True

        return False

    # spocita a vrati pocet vsech neukoncujicich stavu
    def __getNonTerminating(self):
        nonT = []
        self.nonTerminating = None
        for state in self.states:
            # pomocne pole pro uchovani zkontrolovanych pravidel - kvuli deadlocku
            checked = []
            if state in self.final:
                continue
            if self.__isTerminating(state, checked):
                continue

            # ulozeni neukoncujiciho stavu
            nonT.append(state)

        return nonT

    # odstrani dane stavy vcetne vsech pravidel obsahujici tyto stavy
    def __removeStates(self, toRemove):
        # prochazi vsechny stavy k odstraneni
        for state in toRemove:
            # odstraneni z mnoziny stavu
            self.states.remove(state)

            # odstraneni stavu, pokud je pocatecnim stavem
            if state == self.start:
                self.start = None
            
            # odstraneni stavu, pokud je v mnozine ukoncujicich stavu
            if state in self.final:
                self.final.remove(state)

            # ulozeni pravidel, ktera obsahuji odstranovany stav
            rulesToRemove = []
            for rule in self.rules:
                if rule.left == state or rule.right == state:
                    rulesToRemove.append(rule)

            # odstraneni pravidel, ktera obsahuji stav k odstraneni
            for rule in rulesToRemove:
                self.rules.remove(rule)

    # prevede DKA na uplny DKA
    def __makeCompleteFA(self, case):
        # urceni zpusobu zapsani stavu past
        if case:
            qFalse = "qfalse"

        else:
            qFalse = "qFALSE"

        # ulozeni do mnoziny stavu
        self.states.append(qFalse)

        # ulozeni pravidel qFALSE a -> qFALSE, kde a je z mnoziny vstupnich symbolu
        for symbol in self.symbols:
            self.rules.append(Rule(qFalse + symbol + "->" + qFalse))

        # pokud neni pocatecni stav, stane se jim qFALSE
        if self.start == None:
            self.start = qFalse

        # prochazi vsechny stavy a kontroluje, jestli kazdy stav ma od kazdeho symbolu jedno pravidlo
        for state in self.states:
            for symbol in self.symbols:
                exist = False
                # pokud je nalezeno pravidlo odpovidajici stavu a symbolu, nastavi se priznak jeho existence
                for rule in self.rules:
                    if state + " " + symbol  == rule.leftSide():
                        exist = True
                        break
                
                # pokud neni pravidlo pro dany symbol, prida se nove do stavu qFALSE
                if not exist:
                    self.rules.append(Rule(state + symbol + "->" + qFalse))

        return True





    # minimalizuje automat
    def minimize(self, minimize):
        if minimize == False:
            return

        setQm = []       # vysledna mnozina Qm
        finalStates = [] # mnozina koncovych stavu
        otherStates = [] # mnozina stavu Q - F

        # naplenni koncovych stavu
        for end in self.final:
            finalStates.append(end)

        #naplneni stavu Q - F
        for state in self.states:
            if state not in self.final:
                otherStates.append(state)
        # pripojeni danych mnozin do vysledne mnoziny
        setQm.append(finalStates)
        setQm.append(otherStates)
        
        # rozstepeni mnozin
        while True:
            # docasna mnozina pro porovnani, zdali se neco nezmenilo
            tmpSet = list(setQm)
            setQm = self.__splitSets(setQm)

            if tmpSet == setQm:
                break   

        self.__mergeStates(setQm)

    # rozstepi mnozinu koncovych stavu a mnozinu ostatnich stavu dle pravidel minimalizace
    def __splitSets(self, Sets):
        # prochazi vsechny mnoziny
        for s in Sets:
            # postupne prochazi jednotlive symboly vstupni abecedy
            for symbol in self.symbols:
                ruleSet = []
                # prochazi vsechny stavy mnoziny
                for state in s:
                    # prochazi seznam pravidel a uklada si pravidla, kde se leva strana rovna dane kombinace vychoziho stavu a symbolu
                    for rule in self.rules:
                        if state + " " + symbol == rule.leftSide():
                            ruleSet.append(rule)

                # vytvori nove mnoziny stavu podle danych pravidel
                Sets = self.__createNewSets(Sets, ruleSet)

        Sets = self.__removeRedudancy(Sets)
        return Sets

    # rozdeli podle predane mnoziny pravidel stavy do 2 mnozin: ty, ktere maji pravou stranu ze stejne mnoziny a ty, které ne
    def __createNewSets(self, Sets, rules):
        # pokud je pouze jedno pravidlo, pridej levou stranu do mnoziny (pokud tam jiz neni)
        if len(rules) == 1:
            if [rules[0].left] not in Sets:
                Sets.append([rules[0].left])
            # vrat vyslednou mnozinu
            return Sets

        # prochazi vsechny mnoziny a hleda tu, ktera obsahuje pravou stranu prvniho pravidla
        for s in Sets:
            # mnozina stavu, ktere maji pravou stranu ze stejne mnoziny
            newSet1 = [rules[0].left]
            # mnozina stavu, ktere maji pravou stranu z jine mnoziny
            newSet2 = []
            if rules[0].right in s:
                # prochazi zbyla pravidla a podle prislusnosti prave strany k urcite mnozine je ulozi bud do newSet1 nebo do newSet2
                for rule in rules[1:]:
                    if rule.right in s:
                        newSet1.append(rule.left)

                    else:
                        newSet2.append(rule.left)

                if newSet1 not in Sets:
                    Sets.append(newSet1)
                
                if newSet2 not in Sets and len(newSet2) != 0:
                    Sets.append(newSet2)

                break

        return Sets

    # zajisti platnost (Q = Q1 sjednoceni Q2 ... sjednoceni Qn) a (pruniky Q1 a Q2 a ... a Qn se rovnaji prazdne mnozine)
    def __removeRedudancy(self, Sets):
        # serazeni podle poctu prvku v mnozine od nejmensiho po nejvetsi
        Sets.sort(key = len)

        # prochazi jednotlive mnoziny pocinaje nejmensi a z ostatnich mnozin odtranuje stavy obsazene v teto mnozine
        pos = 0
        while pos < len(Sets):
            for state in Sets[pos]:
                for SET in Sets[pos + 1:]:
                    if state in SET:
                        SET.remove(state)

            pos += 1

        # odstrani vznikle prazdne mnoziny
        while [] in Sets:
            Sets.remove([])

        return Sets

    def __mergeStates(self, Sets):
        newStates = []
        newRules = []
        # prochazi vsechny mnoziny
        for s in Sets:
            # pokud je v mnozine pouze jeden stav, pridej ho
            if len(s) == 1:
                newStates.append(s[0])
                continue
            # jinak serad mnozinu a sluc jeji stavy
            s.sort()
            merged = ""
            for state in s:
                merged += state + "_"
            merged = merged[0 : len(merged) - 1]
            # pridej slouceny stav
            newStates.append(merged)

            # nahrazeni dilcich stavu stavem sloucenym ve vsech pravidlech, kde se vyskytuji
            for rule in self.rules:
                if rule.left in s:
                    rule.left = merged

                if rule.right in s:
                    rule.right = merged

            # nahrazeni pocatecniho stavu
            if self.start in s:
                self.start = merged

            # nahrazeni koncovych stavu
            newFinal = []
            for state in self.final:
                # pokud je nejaky koncovy stav z mnoziny slucovanych stavu
                if state in s:
                    # pokud tam ten slouceny jeste neni
                    if merged not in newFinal:
                        newFinal.append(merged)
                
                else:
                    newFinal.append(state)

            self.final = newFinal

        # odstrani vicenasobny vyskyt pravidla, ktery vznikl diky slouceni
        for rule in self.rules:
            included = False
            for rule2 in self.rules:
                # odstran pravidlo, protoze uz tam je
                if rule.equal(rule2) and included:
                    self.rules.remove(rule2)
                # prvni vyskyt pravidla, nastav priznak
                if rule.equal(rule2):
                    included = True

        self.states = newStates

    # zjisti, zdali je zadaný retezec retezcem jazyka prijimaneho timto automatem
    # musi projit cely retezec a skoncit v nejakem koncovem stavu
    def analyzeString(self, string):
        # pokud to neni pozadovano, nic neprovede a vrati se
        if string == False:
            return -2

        # nastaveni pocatecniho stavu
        state = self.start

        for char in string:
            symbol = "'" + char + "'"
            # kontrola, zdali je znak retezce ve vstupni abecede
            if symbol not in self.symbols:
                print("Symbol: '" + char + "' is not included in the input alphabet!", file=sys.stderr)
                sys.exit(1)

            # postupne prochazeni stavu automatu
            for rule in self.rules:
                if rule.left == state and rule.symbol == symbol:
                    state = rule.right
                    break

        # v poradku, pokud je vysledny stav koncovy    
        if state in self.final:
            return 1

        else:
            return 0