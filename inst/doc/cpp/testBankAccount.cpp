#include <cxxPack.hpp>
/**
 * Bank account class used to illustrate proxy pattern.
 */
class BankAccount {
public:
    std::string name;
    int id;
    double balance;
    BankAccount(std::string n, int i, double b) 
        : name(n), id(i), balance(b) {}
    ~BankAccount() { 
	Rprintf("BankAccount destructor called\n"); 
    }
};

/**
 * BankAccount open account method.
 */
RcppExport SEXP testBankOpen(SEXP name, SEXP id, SEXP balance) {
    BEGIN_RCPP
    BankAccount *p = new BankAccount(Rcpp::as<std::string>(name),
				     Rcpp::as<int>(id),
				     Rcpp::as<double>(balance));
    Rcpp::XPtr<BankAccount> xp(p, true);
    return xp;
    END_RCPP
}

/**
 * BankAccount deposit method.
 */
RcppExport SEXP testBankDeposit(SEXP xp_, SEXP amt) {
    BEGIN_RCPP
    Rcpp::XPtr<BankAccount> xp(xp_);
    double oldval = xp->balance;
    xp->balance += Rcpp::as<double>(amt);
    Rcpp::List rl;
    rl["name"] = Rcpp::wrap(xp->name);
    rl["oldbal"] = Rcpp::wrap(oldval);
    rl["curbal"] = Rcpp::wrap(xp->balance);
    return rl;
    END_RCPP
}

/**
 * BankAccount show balance method.
 */
RcppExport SEXP testBankShow(SEXP xp_) {
    BEGIN_RCPP
    Rcpp::XPtr<BankAccount> xp(xp_);
    Rcpp::List rl;
    rl["name"] = Rcpp::wrap(xp->name);
    rl["id"] = Rcpp::wrap(xp->id);
    rl["curbal"] = Rcpp::wrap(xp->balance);
    return rl;
    END_RCPP
}
