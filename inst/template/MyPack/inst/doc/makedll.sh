# Make ./cpp/name.dll from ./cpp/name.cpp
if [ "$1" = "" ]; then
    echo "Usage: sh ./makedll.sh name"
    exit
fi
cd cpp
name=$1
PKG_CPPFLAGS=$(${R_HOME}/bin/Rscript -e "cxxPack:::CxxFlags()")
PKG_LIBS=$(${R_HOME}/bin/Rscript -e "cxxPack:::LdFlags()")
CXX=$(${R_HOME}/bin/R CMD config CXX)
if [ "$CXX" = "g++" ]; then
   PKG_CPPFLAGS="-Wno-variadic-macros ${PKG_CPPFLAGS}"
fi
export PKG_CPPFLAGS PKG_LIBS
${R_HOME}/bin/R CMD SHLIB $name.cpp
cd ..
