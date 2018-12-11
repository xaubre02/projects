BlockEditor
===========
Aplikace pro vytváření, editaci, ukládání a načítání blokových schémat.
Vytvořil Tomáš Aubrecht jako semestrální projekt do předmětu ICP v roce 2018.


Základní dovednosti aplikace:
=============================
 - vytváření blokových schémat a jejich editace
      |-> vkládání funkčních bloků a práce s nimi
      |-> spojování funkčních bloků
      |-> vkládání poznámek a jejich úpravování
      |-> detekce cyklů
      |-> kalkulace celého schématu
      |-> krokování kalkulace
      |-> kalkulace libovolných bloků

 - načítání a ukládání schémat do souborů

 - změna pozadí editoru


Soubor Makefile:
================
 - Vytvoření spustitelného souboru
     |-> make

 - Spuštění vytvořeného souboru
     |-> make run

 - Vygenerování dokumentace
     |-> make doxygen

 - Vyčištění vygenerovaného obsahu
     |-> make clean

 - Zabalení zdrojových souborů
     |-> make pack