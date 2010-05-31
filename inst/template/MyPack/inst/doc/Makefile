target:
	"$(R_HOME)/bin/R" CMD Sweave MyPackDoc.Rnw
	pdflatex MyPackDoc 
	bibtex MyPackDoc
	pdflatex MyPackDoc 
	pdflatex MyPackDoc # again to get cross refs right
#	rm -f *~ *.blg *.bbl *.log *.toc *.aux MyPackDoc.tex cpp/*.o cpp/*.dll cpp/*.so temp/* Rplots.pdf

clean:
	rm -f *~ *.blg *.bbl *.log *.toc *.aux MyPackDoc.tex cpp/*.o cpp/*.dll cpp/*.so temp/* Rplots.pdf


