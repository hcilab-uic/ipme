TEX=pdflatex
BIB=bibtex
SRC=report
TGT=Harish_Naik_$(SRC)_Spring-2017.pdf

all: $(TGT)

$(TGT): $(SRC).tex
	$(TEX) $(SRC)
	$(BIB) $(SRC)
	$(TEX) $(SRC)
	$(TEX) $(SRC)
	cp report.pdf $(TGT)

.PHONY: clean $(TGT)
clean:
	rm -rf *.pdf
	find . -name *.dvi -o -name *.ps -o -name *.rtf -o -name *.txt -o -name *.aux -o -name *.log -o -name *.bbl -o -name *.blg -o -name *.out -o -name *.toc -o -name *~|xargs rm -f
