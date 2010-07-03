(TeX-add-style-hook "cxxPackGuide"
 (lambda ()
    (LaTeX-add-bibliographies
     "cxxPack")
    (LaTeX-add-labels
     "sec.introduction"
     "sec.Sweave++"
     "sec.hello"
     "sec.dotproduct"
     "sec.package"
     "sec.linux"
     "sec.windows"
     "sec.examples"
     "sec.highfreq"
     "sec.schedule"
     "sec.gamma"
     "sec.root"
     "sec.bankaccount"
     "sec.Rcppclasses"
     "sec.nutshell"
     "fig.rcpp"
     "sec.numericvector"
     "sec.cxxPackclasses"
     "sec.cnumericvector"
     "sec.datelib"
     "sec.dataframe"
     "sec.factor"
     "sec.zooseries"
     "sec.exceptions"
     "sec.compatibility")
    (TeX-run-style-hooks
     "Sweave++"
     "nogin"
     "vmargin"
     "latex2e"
     "art10"
     "article")))

