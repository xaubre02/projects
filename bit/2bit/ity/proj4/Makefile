 # 
 # Typografie a publikování
 # Projekt: Číslo 4 - Bibliografické citace
 # Autor:   Tomáš Aubrecht
 # Datum:   18.4.2016
 # 

NAZEV=proj4

all: 	$(NAZEV).tex
	latex $(NAZEV).tex
	bibtex $(NAZEV).aux
	latex $(NAZEV).tex
	latex $(NAZEV).tex
	dvips -t a4 $(NAZEV).dvi
	ps2pdf -sPAPERSIZE=a4 $(NAZEV).ps

clean:
	rm -f $(NAZEV).dvi $(NAZEV).ps $(NAZEV).aux $(NAZEV).log $(NAZEV).bbl $(NAZEV).blg
