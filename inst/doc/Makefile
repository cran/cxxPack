target:
	"$(R_HOME)/bin/R" CMD Sweave cxxPackGuide.Rnw
	pdflatex cxxPackGuide 
	bibtex cxxPackGuide
	pdflatex cxxPackGuide 
	pdflatex cxxPackGuide # again to get cross refs right

clean:
	rm -f *~ *.blg *.bbl *.log *.toc *.aux cxxPackGuide.tex cpp/*.o cpp/*.dll cpp/*.so temp/* Rplots.pdf


