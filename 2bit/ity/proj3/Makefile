 # 
 # Projekt: Cislo 3
 # Autor:   Tomas Aubrecht
 # Datum:   1.4.2016
 # 

NAZEV=proj3

all: 	$(NAZEV).tex
	latex $(NAZEV).tex
	latex $(NAZEV).tex
	dvips -t a4 $(NAZEV).dvi
	ps2pdf -sPAPERSIZE=a4 $(NAZEV).ps

clean:
	rm -f $(NAZEV).dvi $(NAZEV).ps $(NAZEV).aux $(NAZEV).log
