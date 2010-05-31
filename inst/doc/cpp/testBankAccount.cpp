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
    SEXP ret = R_NilValue;
    BEGIN_RCPP
    BankAccount *p = new BankAccount(Rcpp::as<std::string>(name),
				     Rcpp::as<int>(id),
				     Rcpp::as<double>(balance));
    Rcpp::XPtr<BankAccount> xp(p, true);
    ret = xp;
    END_RCPP
    return ret;
}

/**
 * BankAccount deposit method.
 */
RcppExport SEXP testBankDeposit(SEXP xp_, SEXP amt) {
    SEXP ret = R_NilValue;
    BEGIN_RCPP
    Rcpp::XPtr<BankAccount> xp(xp_);
    double oldval = xp->balance;
    xp->balance += Rcpp::as<double>(amt);
    Rcpp::List rl;
    rl["name"] = Rcpp::wrap(xp->name);
    rl["oldbal"] = Rcpp::wrap(oldval);
    rl["curbal"] = Rcpp::wrap(xp->balance);
    ret = rl;
    END_RCPP
    return ret;
}

/**
 * BankAccount show balance method.
 */
RcppExport SEXP testBankShow(SEXP xp_) {
    SEXP ret = R_NilValue;
    BEGIN_RCPP
    Rcpp::XPtr<BankAccount> xp(xp_);
    Rcpp::List rl;
    rl["name"] = Rcpp::wrap(xp->name);
    rl["id"] = Rcpp::wrap(xp->id);
    rl["curbal"] = Rcpp::wrap(xp->balance);
    ret = rl;
    END_RCPP
    return ret;
}
