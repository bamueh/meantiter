
#include <RcppArmadillo.h>

//' Calculate log likelihood of single fitted HI titer
//' 
//' This is the base function for performing a maximum likelihood calculation for a 
//' single fitted HI titer given upper and lower limits of the measured value.
//'
//' @param max_titer The upper bound of the measured titer.
//' @param min_titer The lower bound of the measured titer.
//' @param pred_titer The predicted titer.
//' @param error_sd The standard deviation of the error.
//'
//' @details This function simply calculates to log likelihood of a predicted measurement 
//' given the upper and lower bounds of the measurement, the main assumption being that the 
//' associated error is normally distributed.
//'
//' @return Returns the log-likelihood of the measured titer given the measured titer 
//' bounds and error standard deviation supplied.
double titer_prediction_negll(
    const double &max_titer,
    const double &min_titer,
    const double &pred_titer,
    const double &error_sd
) {
  
  if (max_titer == min_titer) {
    return(
      R::dnorm4(max_titer,pred_titer,error_sd,1)
    );
  } else {
    return(
      R::logspace_sub(
        R::pnorm5(max_titer,pred_titer,error_sd,1,1),
        R::pnorm5(min_titer,pred_titer,error_sd,1,1)
      )
    );
  }
  
}

//' Calculate the total negative log-likelihood of a mean titer
//' 
//' This is a base function to sum the total _negative_ log likelihood of a mean titer.
//' 
//' @param max_titers Numeric vector of the upper bounds of the measured titers.
//' @param min_titers Numeric vector of the lower bounds of the measured titers.
//' @param predicted_mean The predicted mean titer.
//' @param titer_sd The expected standard deviation of titers.
//' 
//' @details This function calculates the total negative log-likelihood of a predicted mean 
//' titer given a set of titers. The main assumption is that both measurement error and 
//' titer variation are normally distributed. Note that the argument \code{titer_sd} is the 
//' total expected standard deviation of the titer set, i.e. measurement error plus titer 
//' variation.
//' 
// [[Rcpp::export]]
double calc_mean_titer_negll(
    const double &predicted_mean,
    const arma::vec &max_titers,
    const arma::vec &min_titers,
    const double &titer_sd) {
  
  double total_negll = 0;
  for(arma::uword i = 0; i < min_titers.n_elem; ++i) {
    total_negll -= titer_prediction_negll(
      max_titers(i),
      min_titers(i),
      predicted_mean,
      titer_sd
    );
  }
  return(total_negll);
  
}


// Calculate the total negative log-likelihood of a mean titer where the standard deviation 
// is unknown. This is an internal function used by the function mean_titer.
// 
// [[Rcpp::export]]
double calc_mean_titer_negll_by_par(
    const arma::vec &pars,
    const arma::vec &max_titers,
    const arma::vec &min_titers,
    double titer_sd
) {
  
  // Take sd titer from parameters if NA
  if (!std::isfinite(titer_sd)) titer_sd = pars(1);
  
  // Return the negative log likelihood
  return(
    calc_mean_titer_negll(
      pars(0),
      max_titers,
      min_titers,
      titer_sd
    )
  );
  
}

// [[Rcpp::export]]
double calc_mean_titer_ci_by_par(
    const arma::vec &pars,
    const arma::vec &max_titers,
    const arma::vec &min_titers,
    double titer_sd,
    double target_negll
) {
  
  return(
    std::abs(
      calc_mean_titer_negll_by_par(
        pars,
        max_titers,
        min_titers,
        titer_sd
      ) - target_negll
    )
  );
  
}

