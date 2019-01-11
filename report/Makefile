all:
	pdflatex report.tex; bibtex report.aux; pdflatex report.tex; pdflatex report.tex
spell:
	ispell -f ispell.words -t algorithm.tex # or *.txt for all files
clean:
	rm -rf *.bbl *.blg *.aux *.log *~ *.bak *.ps *.dvi *.log *.out *.tmp
