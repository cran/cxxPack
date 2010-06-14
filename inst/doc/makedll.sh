# Make ./cpp/name.dll from ./cpp/name.cpp
if [ "$1" = "" ]; then
    echo "Usage: sh ./makedll.sh name [logfile]"
    exit
fi
if [ "$2" = "" ]; then
    logfile="buildlog.txt"
else
    logfile="$2"
fi
if [ "$R_HOME" = "" ]; then
    echo "R_HOME not defined"
    exit 1
fi
cd cpp
name=$1
echo "Sweave++ processing $name.cpp..." >> $logfile
PKG_CPPFLAGS=`"${R_HOME}/bin/Rscript" --vanilla -e "cxxPack:::CxxFlags()"`
PKG_LIBS=`"${R_HOME}/bin/Rscript" --vanilla -e "cxxPack:::LdFlags()"`
CXX=`"${R_HOME}/bin/R" CMD config CXX`
if [ "$CXX" = "g++" ]; then
   PKG_CPPFLAGS="-Wno-variadic-macros ${PKG_CPPFLAGS}"
fi
export PKG_CPPFLAGS PKG_LIBS
"${R_HOME}/bin/R" CMD SHLIB $name.cpp >> $logfile
cd ..
