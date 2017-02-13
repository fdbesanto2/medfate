#include <Rcpp.h>
#include "forestutils.h"
using namespace Rcpp;

// Below is a simple example of exporting a C++ function to R. You can
// source this function into an R session using the Rcpp::sourceCpp 
// function (or via the Source button on the editor toolbar)

// For more on using Rcpp click the Help button on the editor toolbar
/***
 * FUNCTIONS FOR LIGHT EXTINCTION
 */
double availableLight(double h, NumericVector H, NumericVector LAI_expanded, NumericVector LAI_dead, NumericVector k, NumericVector CR) {
  double s= 0.0, p=0.0;
  for(int j=0; j< H.size(); j++) {
    p = (H[j]-h)/(H[j]*CR[j]);
    if(p<0.0) p = 0.0;
    else if(p>1) p=1.0;
    s = s + k[j]*p*(LAI_expanded[j]+LAI_dead[j]);
  }
  return(100*exp((-1)*s));
}

NumericVector parcohortC(NumericVector H, NumericVector LAI_expanded,  NumericVector LAI_dead, NumericVector k, NumericVector CR){
  int n = H.size();
  NumericVector ci(n);
  for(int i=0; i<n;i++) ci[i] = availableLight( H[i]*(1.0-(1.0-CR[i])/2.0), H, LAI_expanded, LAI_dead, k, CR);
  ci.attr("names") = H.attr("names");
  return(ci);
}

// [[Rcpp::export(".parcohort")]]
NumericVector parcohort(IntegerVector SP, NumericVector H, NumericVector CR, NumericVector LAI, DataFrame SpParams){
  NumericVector kSP = SpParams["k"];  
  int n = SP.size();
  NumericVector k(n), LAI_dead(n);
  for(int i=0; i<n;i++) {
    k[i] = kSP[SP[i]];
    LAI_dead[i]=0.0;
  }
  return(parcohortC(H,LAI,LAI_dead,k,CR));
}

// [[Rcpp::export(".parheight")]]
NumericVector parheight(NumericVector heights, IntegerVector SP, NumericVector H, NumericVector CR, NumericVector LAI, DataFrame SpParams){
  int n = SP.size();
  NumericVector kSP = SpParams["k"];  
  NumericVector k(n), LAI_dead(n);
  for(int i=0; i<n;i++) {
    k[i] = kSP[SP[i]];
    LAI_dead[i]=0.0;
  }
  NumericVector AL(heights.size());
  for(int i=0; i<heights.size();i++) AL[i] = availableLight(heights[i], H,LAI,LAI_dead, k,CR);
  return(AL);
}

// [[Rcpp::export(".swrheight")]]
NumericVector swrheight(NumericVector heights, IntegerVector SP, NumericVector H, NumericVector CR, NumericVector LAI, DataFrame SpParams){
  int n = SP.size();
  NumericVector kSP = SpParams["k"];  
  NumericVector kSWR(n), LAI_dead(n);
  for(int i=0; i<n;i++) {
    kSWR[i] = kSP[SP[i]]/1.35;
    LAI_dead[i]=0.0;
  }
  NumericVector AL(heights.size());
  for(int i=0; i<heights.size();i++) AL[i] = availableLight(heights[i], H,LAI, LAI_dead, kSWR,CR);
  return(AL);
}

// [[Rcpp::export(".parExtinctionProfile")]]
NumericVector parExtinctionProfile(NumericVector z, List x, DataFrame SpParams, double gdd = NA_REAL) {
  DataFrame above = forest2aboveground(x, SpParams, gdd);
  IntegerVector SP = above["SP"];
  NumericVector H = above["H"];
  NumericVector LAI = above["LAI_expanded"];
  NumericVector CR = above["CR"];
  return(parheight(z, SP, H, CR, LAI, SpParams));
}

// [[Rcpp::export(".swrExtinctionProfile")]]
NumericVector swrExtinctionProfile(NumericVector z, List x, DataFrame SpParams, double gdd = NA_REAL) {
  DataFrame above = forest2aboveground(x, SpParams,  gdd);
  IntegerVector SP = above["SP"];
  NumericVector H = above["H"];
  NumericVector LAI = above["LAI_expanded"];
  NumericVector CR = above["CR"];
  return(swrheight(z, SP, H, CR, LAI, SpParams));
}

/**
 * Fraction of the incident SWR radiation in a layer that is absorbed
 */
NumericVector layerAbsorbedSWRFractionIncident(NumericMatrix LAIme, NumericMatrix LAImd, NumericVector kSWR) {
  int nlayer = LAIme.nrow();
  int ncoh = LAIme.ncol();
  double s;
  NumericVector f(nlayer);
  for(int l = 0;l<nlayer;l++) {
    s = 0.0;
    for(int c=0;c<ncoh; c++) s+=kSWR[c]*(LAIme(l,c)+LAImd(l,c));
    f[l] = 1.0 - exp(-1.0*s);
  }
  return(f);
}

/**
 * Fraction of the incident SWR radiation in a layer that is absorbed by live leaves of each cohort
 */
NumericMatrix cohortLayerAbsorbedSWRFractionIncident(NumericVector fi, NumericMatrix LAIme, NumericMatrix LAImd, NumericVector kSWR) {
  int nlayer = LAIme.nrow();
  int ncoh = LAIme.ncol();
  NumericMatrix fij(nlayer, ncoh); 
  double s = 0.0;
  for(int l = 0;l<nlayer;l++) {
    s = 0.0;
    for(int c=0;c<ncoh; c++) s+=kSWR[c]*(LAIme(l,c)+LAImd(l,c));
    if(s>0.0) for(int c=0;c<ncoh; c++) fij(l,c) = fi[l]*kSWR[c]*LAIme(l,c)/s;
  }
  return(fij);
}

/**
 * Fraction of the SWR radiation that is absorbed by each cohort
 */
NumericVector cohortAbsorbedSWRFraction(NumericMatrix LAIme, NumericMatrix LAImd, NumericVector kSWR) {
  NumericVector fi = layerAbsorbedSWRFractionIncident(LAIme, LAImd, kSWR);
  NumericVector fij = cohortLayerAbsorbedSWRFractionIncident(fi, LAIme, LAImd, kSWR);
  int ncoh = LAIme.ncol();
  int nlayer = LAIme.nrow();
  NumericVector fj(ncoh);
  NumericVector rem(nlayer);
  for(int i = 0;i<nlayer;i++) {
    rem[i] = 1.0;
    for(int h = (nlayer-1);h>i;h--) {
      rem[i] = rem[i]*(1.0-fi[h]);
    }
  }
  double s;
  for(int j=0;j<ncoh;j++) {
    s = 0.0;
    for(int i = 0;i<nlayer;i++) {
      s = s + fij(i,j)*rem[i];
    }
    fj[j] = s;
  }
  return(fj);
}
    
NumericVector cohortAbsorbedSWRFraction(NumericVector z, NumericVector LAI_expanded, NumericVector LAI_dead, NumericVector H, NumericVector CR, NumericVector kPAR) {
  NumericMatrix LAIme =  LAIdistribution(z, LAI_expanded, H, CR);
  NumericMatrix LAImd =  LAIdistribution(z, LAI_dead, H, CR);
  NumericVector kSWR(kPAR.size());
  for(int i=0;i<kPAR.size();i++) kSWR[i] = kPAR[i]/1.35;
  return(cohortAbsorbedSWRFraction(LAIme, LAImd, kSWR));
}
NumericVector cohortAbsorbedSWRFraction(NumericVector LAI_expanded, NumericVector LAI_dead, NumericVector H, NumericVector CR, NumericVector kPAR) {
  NumericVector z(101); //50 m in 50-cm steps
  for(int i=0;i<101;i++) z[i] = i*50.0;
  NumericMatrix LAIme =  LAIdistribution(z, LAI_expanded, H, CR);
  NumericMatrix LAImd =  LAIdistribution(z, LAI_dead, H, CR);
  NumericVector kSWR(kPAR.size());
  for(int i=0;i<kPAR.size();i++) kSWR[i] = kPAR[i]/1.35;
  return(cohortAbsorbedSWRFraction(LAIme, LAImd, kSWR));
}

// [[Rcpp::export(".cohortAbsorbedSWRFraction")]]
NumericVector cohortAbsorbedSWRFraction(NumericVector z, List x, DataFrame SpParams, double gdd = NA_REAL) {
  NumericMatrix LAIme =  LAIdistribution(z, x, SpParams, gdd);
  NumericMatrix LAImd(LAIme.nrow(), LAIme.ncol());
  int nlayer = LAIme.nrow();
  int ncoh = LAIme.ncol();
  for(int i=0;i<nlayer;i++) for(int j=0;j<ncoh;j++) LAImd(i,j)=0.0; 
  NumericVector k = cohortParameter(x, SpParams, "k");
  NumericVector kSWR(k.size());
  for(int i=0;i<k.size();i++) kSWR[i] = k[i]/1.35;
  return(cohortAbsorbedSWRFraction(LAIme, LAImd, kSWR));
}
