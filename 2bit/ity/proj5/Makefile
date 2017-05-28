 # 
 # Typografie a publikování
 # Projekt: Číslo 5 - Prezentace
 # Autor:   Tomáš Aubrecht
 # Datum:   22.4.2016
 # 

NAZEV=proj5

all: pdf clean

pdf: 	$(NAZEV).tex
	latex $(NAZEV).tex
	latex $(NAZEV).tex
	dvips $(NAZEV).dvi
	ps2pdf $(NAZEV).ps

clean:
	rm -f $(NAZEV).dvi $(NAZEV).ps $(NAZEV).aux $(NAZEV).log $(NAZEV).out $(NAZEV).nav $(NAZEV).snm $(NAZEV).toc
