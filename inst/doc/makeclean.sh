# Cleanup all intermediate files used for vignette processing
if [ "$R_HOME" = "" ]; then
    echo "R_HOME not defined"
    exit 1
fi
shlibsuffix=`"$R_HOME/bin/Rscript" --vanilla -e "{ x = .Platform[['dynlib.ext']]; cat(x) }"`
if [ "$shlibsuffix" = "" ]; then
    echo "Warning: null shared lib suffix"
else
    rm -f cpp/*${shlibsuffix}
fi
rm -f cpp/*.o cpp/*.log
rm -f *~ *.tex *.blg *.bbl *.log *.toc *.aux Rplots.pdf temp/*
rm -rf auto
