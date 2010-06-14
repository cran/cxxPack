# Process name.Rnw (output name.pdf)
if [ "$1" = "" ]; then
    echo "Usage: sh ./makepdf.sh name"
    echo "(name=foo when file=foo.Rnw)"
    exit 1
fi
if [ "$R_HOME" = "" ]; then
    echo "R_HOME not defined"
    exit 1
fi
name=$1
#name=${1%%.*} # basename (Korn shell specific?)
"${R_HOME}/bin/R" CMD Sweave $name.Rnw
pdflatex $name
bibtex $name
pdflatex $name
pdflatex $name
#pdf $name.pdf
