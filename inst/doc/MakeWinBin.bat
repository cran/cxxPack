rem Check directory paths for accuracy
rem
set PATH=C:\Rtools\bin;C:\Program Files\R\R-2.3.1\bin;C:\Program Files\HTML Help Workshop;C:\Dev-Cpp\bin;%PATH%
set MINGW_ROOT=C:\Dev-Cpp
set RHOME=C:\Program Files\R\R-2.3.1
Rcmd build --no-vignettes --force --binary --use-zip %1
