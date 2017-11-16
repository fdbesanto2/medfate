#include <numeric>
#include "lightextinction.h"
#include "windextinction.h"
#include "hydraulics.h"
#include "biophysicsutils.h"
#include "forestutils.h"
#include "photosynthesis.h"
#include "phenology.h"
#include "transpiration.h"
#include "soil.h"
#include <Rcpp.h>
#include <meteoland.h>
using namespace Rcpp;

const double SIGMA_W = 5.67*pow(10,-8.0);
const double Cp_JKG = 1013.86; // J * kg^-1 * ºC^-1

// [[Rcpp::export(".er")]]
NumericVector er(IntegerVector DOY, double ERconv=0.05, double ERsyn = 0.2){
  int nDays = DOY.size();
  NumericVector ER=rep(0.0,nDays);
  for(int i=0;i<nDays;i++){
    if((DOY[i]<=120)|(DOY[i]>=335)) {
      ER[i] = ERsyn;
    } else {
      ER[i] = ERconv;
    }
  }
  return(ER);
  
}


// [[Rcpp::export("swb.SoilEvaporation")]]
double soilevaporation(double DEF,double PETs, double Gsoil){
  double t = pow(DEF/Gsoil, 2.0);
  double Esoil = 0.0;
  Esoil = std::min(Gsoil*(sqrt(t+1)-sqrt(t)), PETs);
  return(Esoil);
}
// [[Rcpp::export(".infiltrationDay")]]
double infiltrationDay(double NetPrec, double Ssoil) {
  double I = 0;
  if(NetPrec>0.2*Ssoil) {
    I = NetPrec-(pow(NetPrec-0.2*Ssoil,2.0)/(NetPrec+0.8*Ssoil));
  } else {
    I = NetPrec;
  }
  return(I);
}


// [[Rcpp::export(".interceptionGashDay")]]
double interceptionGashDay(double Precipitation, double Cm, double p, double ER=0.05) {
    double I = 0.0;
    double PG = (-Cm/(ER*(1.0-p)))*log(1.0-ER); //Precipitation need to saturate the canopy
    if(Cm==0.0 || p==1.0) PG = 0.0; //Avoid NAs
    if(Precipitation>PG) {
      I = (1-p)*PG + (1-p)*ER*(Precipitation-PG);
    } else {
      I = (1-p)*Precipitation;
    }
    return(I);
}


// Soil water balance with simple hydraulic model
// [[Rcpp::export(".swbDay1")]]
List swbDay1(List x, List soil, double tday, double pet, double rain, double er, double runon=0.0, 
             bool verbose = false) {

  //Control parameters
  List control = x["control"];
  bool cavitationRefill = control["cavitationRefill"];

  
  
  //Soil input
  NumericVector W = soil["W"];
  NumericVector psi = soil["psi"];
  NumericVector dVec = soil["dVec"];
  NumericVector Theta_FC = soil["Theta_FC"];
  NumericVector macro = soil["macro"];
  NumericVector rfc = soil["rfc"];
  NumericVector clay = soil["clay"];
  NumericVector sand = soil["sand"];
  NumericVector om = soil["om"];
  NumericVector Water_FC = waterFC(soil);
  int nlayers = W.size();
  

  //Vegetation input
  DataFrame above = Rcpp::as<Rcpp::DataFrame>(x["above"]);
  NumericVector LAIlive = Rcpp::as<Rcpp::NumericVector>(above["LAI_live"]);
  NumericVector LAIphe = Rcpp::as<Rcpp::NumericVector>(above["LAI_expanded"]);
  NumericVector LAIdead = Rcpp::as<Rcpp::NumericVector>(above["LAI_dead"]);
  NumericVector H = Rcpp::as<Rcpp::NumericVector>(above["H"]);
  NumericVector CR = Rcpp::as<Rcpp::NumericVector>(above["CR"]);
  int numCohorts = LAIphe.size();

  //Root distribution input
  List below = Rcpp::as<Rcpp::List>(x["below"]);
  NumericMatrix V = Rcpp::as<Rcpp::NumericMatrix>(below["V"]);

  //Parameters  
  DataFrame paramsBase = Rcpp::as<Rcpp::DataFrame>(x["paramsBase"]);
  NumericVector Sgdd = Rcpp::as<Rcpp::NumericVector>(paramsBase["Sgdd"]);
  NumericVector kPAR = Rcpp::as<Rcpp::NumericVector>(paramsBase["k"]);
  NumericVector gRainIntercept = Rcpp::as<Rcpp::NumericVector>(paramsBase["g"]);
  
  DataFrame paramsTransp = Rcpp::as<Rcpp::DataFrame>(x["paramsTransp"]);
  NumericVector Psi_Extract = Rcpp::as<Rcpp::NumericVector>(paramsTransp["Psi_Extract"]);
  NumericVector WUE = Rcpp::as<Rcpp::NumericVector>(paramsTransp["WUE"]);
  NumericVector pRootDisc = Rcpp::as<Rcpp::NumericVector>(paramsTransp["pRootDisc"]);
  
  //Communication vectors
  NumericVector transpiration = Rcpp::as<Rcpp::NumericVector>(x["Transpiration"]);
  NumericVector photosynthesis = Rcpp::as<Rcpp::NumericVector>(x["Photosynthesis"]);
  NumericVector pEmb = Rcpp::as<Rcpp::NumericVector>(x["ProportionCavitated"]);
  


  //Determine whether leaves are out (phenology) and the adjusted Leaf area
  NumericVector Phe(numCohorts);
  double s = 0.0, LAIcell = 0.0, Cm = 0.0;
  for(int c=0;c<numCohorts;c++) {
    Phe[c]=LAIphe[c]/LAIlive[c]; //Phenological status
    s += (kPAR[c]*(LAIphe[c]+LAIdead[c]));
    LAIcell += LAIphe[c]+LAIdead[c];
    Cm += (LAIphe[c]+LAIdead[c])*gRainIntercept[c]; //LAI dead also counts on interception
  }
  NumericVector CohASWRF = cohortAbsorbedSWRFraction(LAIphe,  LAIdead, H, CR, kPAR);
  NumericVector CohPAR = parcohortC(H, LAIphe, LAIdead, kPAR, CR)/100.0;
  double LgroundPAR = exp((-1)*s);
  double LgroundSWR = 1.0 - std::accumulate(CohASWRF.begin(),CohASWRF.end(),0.0);
  
  //Hydrologic input
  double NetPrec = 0.0, Infiltration= 0.0, Runoff= 0.0, DeepDrainage= 0.0;
  if(rain>0.0) {
    //Interception
    NetPrec = rain - interceptionGashDay(rain,Cm,LgroundPAR,er);
    //Net Runoff and infiltration
    Infiltration = infiltrationDay(NetPrec+runon, Water_FC[0]);
    Runoff = (NetPrec+runon) - Infiltration;
    //Input of the first soil layer is infiltration
    double VI = Infiltration;
    double Wn;
    //Update topsoil layer
    for(int l=0;l<nlayers;l++) {
      if((dVec[l]>0) & (VI>0)) {
        Wn = W[l]*Water_FC[l] + VI*(1.0-macro[l]); //Update water volume
        VI = VI*macro[l] + std::max(Wn - Water_FC[l],0.0); //Update VI, adding the excess to the infiltrating water (saturated flow)
        W[l] = std::min(std::max(0.0,Wn/Water_FC[l]),1.0); //Update theta
      } 
      DeepDrainage = VI; //Reset deep drainage
    }
  }
  for(int l=0;l<nlayers;l++) psi[l] = theta2psi(clay[l], sand[l], W[l]*Theta_FC[l], om[l]);


  //Proportion of transpiration that absorbed by each plant cohort (old version)
  // NumericVector PP = CohLight*LAIphe;
  // NumericVector f = PP/std::accumulate(PP.begin(),PP.end(),0.0); 
  // if(LAIcell==0.0) f = rep(0.0,numCohorts); //Avoids NaN values

  //Apply fractions to potential evapotranspiration
  //Maximum canopy transpiration
  //    Tmax = PET[i]*(-0.006*pow(LAIcell[i],2.0)+0.134*LAIcell[i]+0.036); //From Granier (1999)
  double Tmax = pet*(-0.006*pow(LAIcell,2.0)+0.134*LAIcell); //From Granier (1999)
  double PETsoil = pet*LgroundSWR;

  //Fraction of Tmax attributed to each plant cohort
  double pabs = std::accumulate(CohASWRF.begin(),CohASWRF.end(),0.0);
  NumericVector TmaxCoh(numCohorts);
  if(pabs>0.0) TmaxCoh = Tmax*(CohASWRF/pabs);
  
  //Actual plant transpiration
  NumericMatrix EplantCoh(numCohorts, nlayers);
  NumericMatrix PsiRoot(numCohorts, nlayers);
  NumericVector PlantPsi(numCohorts, NA_REAL);
  NumericVector Eplant(numCohorts, 0.0);
  NumericVector DDS(numCohorts, 0.0);
  NumericVector EplantVec(nlayers, 0.0);
  NumericVector Kl, epc, Vl;
  double WeibullShape=3.0;
  for(int l=0;l<nlayers;l++) {
    Kl = Psi2K(psi[l], Psi_Extract, WeibullShape);
 
    //Limit Kl due to previous cavitation
    if(!cavitationRefill) for(int c=0;c<numCohorts;c++) Kl[c] = std::min(Kl[c], 1.0-pEmb[c]);
    //Limit Kl to minimum value for root disconnection
    Vl = V(_,l);
    epc = pmax(TmaxCoh*Kl*Vl,0.0);
    for(int c=0;c<numCohorts;c++) {
      PsiRoot(c,l) = psi[l]; //Set initial guess of root potential to soil values
      //If relative conductance is smaller than the value for root disconnection
      //Set root potential to minimum value before disconnection and transpiration from that layer to zero
      if(Kl[c]<pRootDisc[c]) { 
        PsiRoot(c,l) = K2Psi(pRootDisc[c],Psi_Extract[c],WeibullShape);
        Kl[c] = pRootDisc[c]; //So that layer stress does not go below pRootDisc
        epc[c] = 0.0; //Set transpiration from layer to zero
      }
    }
    
    EplantCoh(_,l) = epc;
    Eplant = Eplant + epc;
    EplantVec[l] = std::accumulate(epc.begin(),epc.end(),0.0);
    DDS = DDS + Phe*(Vl*(1.0 - Kl)); //Add stress from the current layer
  }
  for(int c=0;c<numCohorts;c++) {
    PlantPsi[c] = averagePsi(PsiRoot(c,_), V(c,_), WeibullShape, Psi_Extract[c]);
    if(!cavitationRefill) {
      pEmb[c] = std::max(DDS[c], pEmb[c]); //Track current embolism if no refill
      DDS[c] = pEmb[c];
    }
  }

  //Evaporation from bare soil
  double Gsoil = soil["Gsoil"];
  double Ksoil = soil["Ksoil"];
  double Esoil = soilevaporation((Water_FC[0]*(1.0 - W[0])), PETsoil, Gsoil);
  NumericVector EsoilVec(nlayers,0.0);//Exponential decay to divide bare soil evaporation among layers
  for(int l=0;l<nlayers;l++) {
    double cumAnt = 0.0;
    double cumPost = 0.0;
    for(int l2=0;l2<l;l2++) cumAnt +=dVec[l2];
    cumPost = cumAnt+dVec[l];
    if(l<(nlayers-1)) EsoilVec[l] = Esoil*(exp(-Ksoil*cumAnt)-exp(-Ksoil*cumPost));
    else EsoilVec[l] = Esoil*exp(-Ksoil*cumAnt);
    
  }
  // Rcout<<Esoil<<" "<<  std::accumulate(EsoilVec.begin(),EsoilVec.end(),0.0)<<"\n";

  //Apply decrease in soil layers
  for(int l=0;l<nlayers;l++) W[l] =std::max(W[l]-(EplantVec[l]+EsoilVec[l])/Water_FC[l],0.0);


  double alpha = std::max(std::min(tday/20.0,1.0),0.0);
  //For comunication with growth and landscape water balance
  for(int c=0;c<numCohorts;c++) {
    transpiration[c] = Eplant[c];
    photosynthesis[c] = alpha*WUE[c]*transpiration[c];
  }

  List l = List::create(_["PET"] = pet, _["NetPrec"] = NetPrec, _["Runon"] = runon, _["Infiltration"] = Infiltration, _["Runoff"] = Runoff, _["DeepDrainage"] = DeepDrainage,
                        _["LAIcell"] = LAIcell, _["Cm"] = Cm, _["Lground"] = LgroundPAR,
                        _["EsoilVec"] = EsoilVec, _["EplantVec"] = EplantVec, _["psiVec"] = psi,
                        _["EplantCoh"] = Eplant, _["psiCoh"] = PlantPsi, _["DDS"] = DDS);
  return(l);
}


// Soil water balance with Sperry hydraulic and stomatal conductance models
// [[Rcpp::export(".swbDay2")]]
List swbDay2(List x, List soil, double tmin, double tmax, double rhmin, double rhmax, double rad, double wind, 
             double latitude, double elevation, double slope, double aspect, double solarConstant, double delta, 
             double rain, double er, double runon=0.0, bool verbose = false) {
  
  //Control parameters
  List control = x["control"];
  List numericParams = control["numericParams"];
  double psiStep = numericParams["psiStep"];
  double psiMax = numericParams["psiMax"];
  int ntrial = numericParams["ntrial"];
  int maxNsteps  = numericParams["maxNsteps"];
  double psiTol = numericParams["psiTol"];
  double ETol = numericParams["ETol"];

  bool cavitationRefill = control["cavitationRefill"];
  String canopyMode = Rcpp::as<Rcpp::String>(control["canopyMode"]);
  int ntimesteps = control["ndailysteps"];
  int hydraulicCostFunction = control["hydraulicCostFunction"];
  double verticalLayerSize = control["verticalLayerSize"];
    
  //Soil input
  NumericVector W = soil["W"];
  NumericVector psi = soil["psi"];
  NumericVector dVec = soil["dVec"];
  NumericVector Theta_FC = soil["Theta_FC"];
  NumericVector macro = soil["macro"];
  NumericVector rfc = soil["rfc"];
  NumericVector clay = soil["clay"];
  NumericVector sand = soil["sand"];
  NumericVector om = soil["om"];
  NumericVector Water_FC = waterFC(soil);
  NumericVector Tsoil = soil["Temp"]; 
  int nlayers = W.size();

  

  //Vegetation input
  DataFrame above = Rcpp::as<Rcpp::DataFrame>(x["above"]);
  NumericVector LAIlive = Rcpp::as<Rcpp::NumericVector>(above["LAI_live"]);
  NumericVector LAIphe = Rcpp::as<Rcpp::NumericVector>(above["LAI_expanded"]);
  NumericVector LAIdead = Rcpp::as<Rcpp::NumericVector>(above["LAI_dead"]);
  NumericVector H = Rcpp::as<Rcpp::NumericVector>(above["H"]);
  NumericVector CR = Rcpp::as<Rcpp::NumericVector>(above["CR"]);
  int numCohorts = LAIlive.size();

  //Canopy params
  List canopyParams = Rcpp::as<Rcpp::List>(x["canopy"]);
  
  //Root distribution input
  List below = Rcpp::as<Rcpp::List>(x["below"]);
  NumericMatrix V =Rcpp::as<Rcpp::NumericMatrix>(below["V"]);
  NumericMatrix VCroot_kmax= Rcpp::as<Rcpp::NumericMatrix>(below["VCroot_kmax"]);
  NumericMatrix VGrhizo_kmax= Rcpp::as<Rcpp::NumericMatrix>(below["VGrhizo_kmax"]);
  
  //Base parameters
  DataFrame paramsBase = Rcpp::as<Rcpp::DataFrame>(x["paramsBase"]);
  NumericVector Sgdd = Rcpp::as<Rcpp::NumericVector>(paramsBase["Sgdd"]);
  NumericVector albedo = Rcpp::as<Rcpp::NumericVector>(paramsBase["albedo"]);
  NumericVector kPAR = Rcpp::as<Rcpp::NumericVector>(paramsBase["k"]);
  NumericVector gRainIntercept = Rcpp::as<Rcpp::NumericVector>(paramsBase["g"]);

  //Transpiration parameters
  DataFrame paramsTransp = Rcpp::as<Rcpp::DataFrame>(x["paramsTransp"]);
  NumericVector Gwmin = Rcpp::as<Rcpp::NumericVector>(paramsTransp["Gwmin"]);
  NumericVector Gwmax = Rcpp::as<Rcpp::NumericVector>(paramsTransp["Gwmax"]);
  NumericVector VCstem_kmax = Rcpp::as<Rcpp::NumericVector>(paramsTransp["VCstem_kmax"]);
  NumericVector VCstem_c = Rcpp::as<Rcpp::NumericVector>(paramsTransp["VCstem_c"]);
  NumericVector VCstem_d = Rcpp::as<Rcpp::NumericVector>(paramsTransp["VCstem_d"]);
  NumericVector VCroot_c = paramsTransp["VCroot_c"];
  NumericVector VCroot_d = paramsTransp["VCroot_d"];
  NumericVector Vmax298 = paramsTransp["Vmax298"];
  NumericVector Jmax298 = paramsTransp["Jmax298"];
  NumericVector pRootDisc = Rcpp::as<Rcpp::NumericVector>(paramsTransp["pRootDisc"]);
  
  //Comunication with outside
  NumericVector transpiration = Rcpp::as<Rcpp::NumericVector>(x["Transpiration"]);
  NumericVector photosynthesis = Rcpp::as<Rcpp::NumericVector>(x["Photosynthesis"]);
  NumericVector pEmb = Rcpp::as<Rcpp::NumericVector>(x["ProportionCavitated"]);
  
  NumericVector VG_n = Rcpp::as<Rcpp::NumericVector>(soil["VG_n"]);
  NumericVector VG_alpha = Rcpp::as<Rcpp::NumericVector>(soil["VG_alpha"]);
  
  
  double latrad = latitude * (PI/180.0);
  if(NumericVector::is_na(aspect)) aspect = 0.0;
  double asprad = aspect * (PI/180.0);
  if(NumericVector::is_na(slope)) slope = 0.0;
  double slorad = slope * (PI/180.0);

  //Step in seconds
  double tstep = 86400.0/((double) ntimesteps);
  
  //Day length (latitude in radians), atmospheric pressure, CO2 concentration
  double tauday = meteoland::radiation_daylengthseconds(latrad,  slorad,asprad, delta); 
  double Patm = meteoland::utils_atmosphericPressure(elevation);
  double Catm = 386.0;
  
  //Daily average water vapor pressure at the atmosphere
  double vpatm = meteoland::utils_averageDailyVP(tmin, tmax, rhmin,rhmax);
  
  //Daily cloud cover
  double cloudcover = 0.0;
  if(rain >0.0) cloudcover = 1.0;
  bool clearday = (rain==0);
  
  //Instantaneous direct and diffuse shorwave radiation
  DataFrame ddd = meteoland::radiation_directDiffuseDay(solarConstant, latrad, slorad, asprad, delta,
                                                        rad, clearday, ntimesteps);
  NumericVector solarElevation = ddd["SolarElevation"]; //in radians
  NumericVector solarHour = ddd["SolarHour"]; //in radians
  NumericVector SWR_direct = ddd["SWR_direct"]; //in kW·m-2
  NumericVector SWR_diffuse = ddd["SWR_diffuse"]; //in kW·m-2
  NumericVector PAR_direct = ddd["PAR_direct"]; //in kW·m-2
  NumericVector PAR_diffuse = ddd["PAR_diffuse"]; //in kW·m-2
  
  //Instantaneous air temperature (above canopy) and longwave radiation
  NumericVector Tatm(ntimesteps), lwdr(ntimesteps), Tcan(ntimesteps, NA_REAL), Tsunrise(ntimesteps);
  NumericVector LEcan_heat(ntimesteps), Hcan_heat(ntimesteps), G1_heat(ntimesteps), LWRcanout(ntimesteps), SWRLWRcanin(ntimesteps), Ebal(ntimesteps);
  NumericVector SWRLWRsoilin(ntimesteps), LWRsoilout(ntimesteps), Ebalsoil(ntimesteps);
  NumericMatrix Tsoil_mat(ntimesteps, nlayers);
  for(int n=0;n<ntimesteps;n++) {
    //From solar hour (radians) to seconds from sunrise
    Tsunrise[n] = (solarHour[n]*43200.0/PI)+ (tauday/2.0) +(tstep/2.0); 
    
    //Calculate instantaneous temperature and light conditions
    Tatm[n] = temperatureDiurnalPattern(Tsunrise[n], tmin, tmax, tauday);
    //Longwave sky diffuse radiation
    lwdr[n] = meteoland::radiation_skyLongwaveRadiation(Tatm[n], vpatm, cloudcover);
  }
  Tcan[0] = canopyParams["Temp"]; //Take canopy temperature from previous day
  Tsoil_mat(0,_) = Tsoil;

  //Leaf phenology and the adjusted leaf area index
  NumericVector Phe(numCohorts);
  double LAIcell = 0.0, Cm = 0.0, canopyHeight = 0.0, LAIcellmax = 0.0;
  for(int c=0;c<numCohorts;c++) {
    Phe[c]=LAIphe[c]/LAIlive[c]; //Phenological status
    LAIcell += (LAIphe[c]+LAIdead[c]);
    LAIcellmax += (LAIlive[c]+LAIdead[c]);
    Cm += (LAIphe[c]+LAIdead[c])*gRainIntercept[c]; //LAI dead also counts on interception
    if(canopyHeight<H[c]) canopyHeight = H[c];
  }
  int nz = ceil(canopyHeight/verticalLayerSize); //Number of vertical layers

  NumericVector z(nz+1,0.0);
  NumericVector zmid(nz);
  for(int i=1;i<=nz;i++) {
    z[i] = z[i-1] + verticalLayerSize;
    zmid[i-1] = (verticalLayerSize/2.0) + verticalLayerSize*((double) (i-1));
  }
  NumericMatrix LAIme = LAIdistribution(z, LAIphe, H, CR);
  NumericMatrix LAImd = LAIdistribution(z, LAIdead, H, CR);


  //Light extinction and absortion by time steps
  List lightExtinctionAbsortion = instantaneousLightExtinctionAbsortion(LAIme, LAImd, 
                                                                        kPAR, albedo,
                                                                        ddd,  lwdr,
                                                                        ntimesteps,  canopyMode);
  
  List abs_PAR_SL_list = lightExtinctionAbsortion["PAR_SL"];
  List abs_SWR_SL_list = lightExtinctionAbsortion["SWR_SL"];
  List abs_PAR_SH_list = lightExtinctionAbsortion["PAR_SH"];
  List abs_SWR_SH_list = lightExtinctionAbsortion["SWR_SH"];
  List abs_LWR_SL_list = lightExtinctionAbsortion["LWR_SL"];
  List abs_LWR_SH_list = lightExtinctionAbsortion["LWR_SH"];
  NumericVector fsunlit = lightExtinctionAbsortion["fsunlit"];
  NumericVector abs_SWR_can = lightExtinctionAbsortion["SWR_can"];
  NumericVector abs_SWR_soil = lightExtinctionAbsortion["SWR_soil"];
  NumericVector abs_LWR_can = lightExtinctionAbsortion["LWR_can"];
  NumericVector abs_LWR_soil = lightExtinctionAbsortion["LWR_soil"];
  NumericVector emm_LWR_soil(ntimesteps,0.0);
  double kb = lightExtinctionAbsortion["kb"];  //Proportion of sunlit extinction coefficient
  // double gbf = lightExtinctionAbsortion["gbf"]; //Ground fractions
  // double gdf = lightExtinctionAbsortion["gdf"];
  
  //Hydrologic input
  double NetPrec = 0.0, Infiltration= 0.0, Runoff= 0.0, DeepDrainage= 0.0;
  double propCover = 1.0-exp((-1)*kb*LAIcell);
  if(rain>0.0) {
    //Interception
    NetPrec = rain - interceptionGashDay(rain,Cm,propCover,er);
    //Net Runoff and infiltration
    Infiltration = infiltrationDay(NetPrec+runon, Water_FC[0]);
    Runoff = (NetPrec+runon) - Infiltration;
    //Input of the first soil layer is infiltration
    double VI = Infiltration;
    double Wn;
    //Update topsoil layer
    for(int l=0;l<nlayers;l++) {
      if((dVec[l]>0) & (VI>0)) {
        Wn = W[l]*Water_FC[l] + VI*(1.0-macro[l]); //Update water volume
        VI = VI*macro[l] + std::max(Wn - Water_FC[l],0.0); //Update VI, adding the excess to the infiltrating water (saturated flow)
        W[l] = std::min(std::max(0.0,Wn/Water_FC[l]),1.0); //Update theta
      } 
      DeepDrainage = VI; //Reset deep drainage
    }
  }
  for(int l=0;l<nlayers;l++) {
    psi[l] = theta2psi(clay[l], sand[l], W[l]*Theta_FC[l], om[l]);
    // Rcout<<psi[l]<<" ";
  }
  // Rcout<<"\n";

  // Rcout<<rad<<" "<< solarElevation[0]<<" "<<SWR_direct[0]<<"\n";



  //Wind extinction profile
  if(NumericVector::is_na(wind)) wind = 2.0; 
  NumericVector zWind;
  if(canopyMode=="multilayer") {
    zWind = windExtinctionProfile(zmid, wind, LAIcell, canopyHeight);
  } else if(canopyMode=="sunshade"){
    zWind = windExtinctionCohort(H,CR, wind,LAIcell, canopyHeight);
  }
  

  //Hydraulics: determine layers where the plant is connected
  IntegerVector nlayerscon(numCohorts,0);
  LogicalMatrix layerConnected(numCohorts, nlayers);
  for(int c=0;c<numCohorts;c++) {
    nlayerscon[c] = 0;
    for(int l=0;l<nlayers;l++) {
      if(V(c,l)>0.0) {
        double pRoot = xylemConductance(psi[l], 1.0, VCroot_c[c], VCroot_d[c]); //Relative conductance in the root
        layerConnected(c,l)= (pRoot>=pRootDisc[c]);
        if(layerConnected(c,l)) nlayerscon[c]=nlayerscon[c]+1;
      } else {
        layerConnected(c,l) = false;
      }
    }
  }

  //Hydraulics: build supply function
  List supplyNetworks(numCohorts);
  for(int c=0;c<numCohorts;c++) {
    // Copy values from connected layers
    NumericVector Vc = NumericVector(nlayerscon[c]);
    NumericVector VCroot_kmaxc = NumericVector(nlayerscon[c]);
    NumericVector VGrhizo_kmaxc = NumericVector(nlayerscon[c]);
    NumericVector psic = NumericVector(nlayerscon[c]);
    NumericVector VG_nc = NumericVector(nlayerscon[c]);
    NumericVector VG_alphac= NumericVector(nlayerscon[c]);
    int cnt=0;
    for(int l=0;l<nlayers;l++) {
      if(layerConnected(c,l)) {
        Vc[cnt] = V(c,l);
        VCroot_kmaxc[cnt] = VCroot_kmax(c,l);
        VGrhizo_kmaxc[cnt] = VGrhizo_kmax(c,l);
        psic[cnt] = psi[l];
        VG_nc[cnt] = VG_n[l];
        VG_alphac[cnt] = VG_alpha[l];
        cnt++;
      }
    }
    // Build supply function
    double psiCav = 0.0;
    if(!cavitationRefill) {
      psiCav = xylemPsi(1.0-pEmb[c], 1.0, VCstem_c[c], VCstem_d[c]);//find water potential corresponding to this percentage of conductance loss
    }
    double minFlow = std::max(0.0,1000.0*(Gwmin[c]*(tmin+tmax)/2.0)/Patm);
    // Rcout<<minFlow<<"\n";
    supplyNetworks[c] = supplyFunctionNetwork(psic,
                                             VGrhizo_kmaxc,VG_nc,VG_alphac,
                                             VCroot_kmaxc, VCroot_c[c],VCroot_d[c],
                                             VCstem_kmax[c], VCstem_c[c],VCstem_d[c], 
                                             psiCav,
                                             minFlow, maxNsteps, psiStep, psiMax , ntrial, psiTol, ETol);
  }

  //Transpiration and photosynthesis
  NumericVector psiBk(nlayers);
  for(int l=0;l<nlayers;l++) psiBk[l] = psi[l]; //Store initial soil water potential
  NumericVector PlantPsi(numCohorts);
  NumericMatrix K(numCohorts, nlayers);
  NumericVector Eplant(numCohorts);
  NumericVector EplantVec(nlayers);
  NumericVector DDS(numCohorts);
  NumericMatrix Rninst(numCohorts,ntimesteps);
  NumericMatrix Qinst(numCohorts,ntimesteps);
  NumericMatrix Einst(numCohorts, ntimesteps);
  NumericMatrix Aninst(numCohorts, ntimesteps);
  NumericMatrix PsiPlantinst(numCohorts, ntimesteps);
  NumericVector minPsiLeaf(numCohorts,0.0), minPsiRoot(numCohorts,0.0); //Minimum potentials experienced
  for(int c=0;c<numCohorts;c++) {
    photosynthesis[c] = 0.0;
    transpiration[c] = 0.0;
  }
  
  for(int n=0;n<ntimesteps;n++) { //Time loop
    NumericVector EplantVecInstant(nlayers,0.0); //Transpiration extracted from each layers, taking into account all cohorts 
    for(int c=0;c<numCohorts;c++) { //Plant cohort loop
      if(nlayerscon[c]>0) {//If the plant is connected to at least one layer
        List supply = supplyNetworks[c];
        NumericMatrix ElayersMat = supply["Elayers"];
        NumericVector PsiLeafVec = supply["PsiPlant"];
        NumericVector PsiRootVec = supply["PsiRoot"];
        
        
        //Constant properties through time steps
        NumericVector Vmax298layer(nz), Jmax298layer(nz);
        NumericVector SLarealayer(nz), SHarealayer(nz);
        NumericVector QSH(nz), QSL(nz), absRadSL(nz), absRadSH(nz);
        double sn =0.0;
        for(int i=(nz-1);i>=0.0;i--) {
          //Effect of nitrogen concentration decay through the canopy
          double fn = exp(-0.713*(sn+LAIme(i,c)/2.0)/sum(LAIme(_,c)));
          sn+=LAIme(i,c);
          SLarealayer[i] = LAIme(i,c)*fsunlit[i];
          SHarealayer[i] = LAIme(i,c)*(1.0-fsunlit[i]);
          Vmax298layer[i] = Vmax298[c]*fn;
          Jmax298layer[i] = Jmax298[c]*fn;
        }
        double Vmax298SL= 0.0,Vmax298SH= 0.0,Jmax298SL= 0.0,Jmax298SH= 0.0;
        double SLarea = 0.0, SHarea = 0.0;
        if(canopyMode=="sunshade"){
          for(int i=0;i<nz;i++) {
            SLarea +=SLarealayer[i];
            SHarea +=SHarealayer[i];
            Vmax298SL +=Vmax298layer[i]*LAIme(i,c)*fsunlit[i];
            Jmax298SL +=Jmax298layer[i]*LAIme(i,c)*fsunlit[i];
            Vmax298SH +=Vmax298layer[i]*LAIme(i,c)*(1.0-fsunlit[i]);
            Jmax298SH +=Jmax298layer[i]*LAIme(i,c)*(1.0-fsunlit[i]);
          }
        }
        
        //Long-wave radiation due to canopy temperature
        if(NumericVector::is_na(Tcan[n])) Tcan[n] = Tatm[n]; //If missing take above-canopy air temperature
        if(NumericVector::is_na(Tsoil[0])) {//Initialize Soil temperature (to minimum air temperature) if missing
          for(int l=0;l<nlayers; l++) {
            Tsoil[l] = Tatm[n];
          }
          Tsoil_mat(n,_) = Tsoil; 
        }
        double lwcan = SIGMA_W*pow(Tcan[n]+273.16,4.0);
          
        //Photosynthesis function
        List photo;
        if(canopyMode=="multilayer"){
          //Retrieve Light extinction
          NumericMatrix absPAR_SL = abs_PAR_SL_list[n];
          NumericMatrix absPAR_SH = abs_PAR_SH_list[n];
          NumericMatrix absSWR_SL = abs_SWR_SL_list[n];
          NumericMatrix absSWR_SH = abs_SWR_SH_list[n];
          NumericMatrix absLWR_SL = abs_LWR_SL_list[n];
          NumericMatrix absLWR_SH = abs_LWR_SH_list[n];
          for(int i=0;i<nz;i++) {
            QSL[i] = irradianceToPhotonFlux(absPAR_SL(i,c));
            QSH[i] = irradianceToPhotonFlux(absPAR_SH(i,c));
            //Add absorved short wave and long wave radiation (from sky and canopy)
            absRadSL[i] = absSWR_SL(i,c) + absLWR_SL(i,c) + SLarealayer[i]*0.97*lwcan;
            absRadSH[i] = absSWR_SH(i,c)+ absLWR_SH(i,c) + SHarealayer[i]*0.97*lwcan;
          }
          photo = multilayerPhotosynthesisFunction(supply, Catm, Patm, Tcan[n], vpatm, 
                                                   SLarealayer, SHarealayer, 
                                                   zWind, absRadSL, absRadSH, 
                                                   QSL, QSH,
                                                   Vmax298layer,Jmax298layer,
                                                   Gwmin[c], Gwmax[c]);
        } else if(canopyMode=="sunshade"){
          //Retrieve Light extinction
          NumericVector absPAR_SL = abs_PAR_SL_list[n];
          NumericVector absPAR_SH = abs_PAR_SH_list[n];
          NumericVector absSWR_SL = abs_SWR_SL_list[n];
          NumericVector absSWR_SH = abs_SWR_SH_list[n];
          NumericVector absLWR_SL = abs_LWR_SL_list[n];
          NumericVector absLWR_SH = abs_LWR_SH_list[n];
          
          photo = sunshadePhotosynthesisFunction(supply, Catm, Patm,Tcan[n], vpatm, 
                                                 SLarea, SHarea, 
                                                 zWind[c], 
                                                 absSWR_SL[c] + absLWR_SL[c] + SLarea*0.97*lwcan, 
                                                 absSWR_SH[c] + absLWR_SH[c] + SHarea*0.97*lwcan, 
                                                 irradianceToPhotonFlux(absPAR_SL[c]), irradianceToPhotonFlux(absPAR_SH[c]),
                                                 Vmax298SL, Vmax298SH,
                                                 Jmax298SL, Jmax298SH,
                                                 Gwmin[c], Gwmax[c]);
        }
        NumericVector An = photo["NetPhotosynthesis"];
        
        //Profit maximization
        List PM = profitMaximization(supply, photo,  hydraulicCostFunction, VCstem_kmax[c]);

        //Scale photosynthesis
        int iPM = PM["iMaxProfit"];
        photosynthesis[c] +=pow(10,-6)*12.01017*An[iPM]*tstep; 
        Aninst(c,n) = An(iPM);
        
        //Scale transpiration to cohort level
        NumericVector Ecn(nlayerscon[c],0.0);
        Einst(c,n) = 0.0;
        for(int lc=0;lc<nlayerscon[c];lc++) {
          Ecn[lc] = ElayersMat(iPM,lc)*0.001*0.01802*LAIphe[c]*tstep; //Scale
          Einst(c,n) +=Ecn[lc]; //add flow from all layers to estimate plant transpiration
        }
        
        //Do not allow negative plant transpiration values
        if(Einst(c,n)<0.0) {
          Einst(c,n) = 0.0;
          for(int lc=0;lc<nlayerscon[c];lc++) Ecn[lc] = 0.0;
          if(verbose) Rcout<<"N";
        }
        //Add to daily plant cohort transpiration
        Eplant[c] +=Einst(c,n);
        
        //Store the minimum water potential of the day (i.e. mid-day)
        minPsiLeaf[c] = std::min(minPsiLeaf[c],PsiLeafVec[iPM]);
        minPsiRoot[c] = std::min(minPsiRoot[c],PsiRootVec[iPM]);
        PsiPlantinst(c,n) = PsiLeafVec[iPM]; //Store instantaneous plant potential

        //Copy transpiration from connected layers to transpiration from soil layers
        int cnt = 0;
        for(int l=0;l<nlayers;l++) {
          if(layerConnected(c,l)) {
            EplantVec[l] += Ecn[cnt]; //Add to cummulative transpiration from layers
            EplantVecInstant[l] +=Ecn[cnt]; //Add to instantaneous transpiration from layers
            cnt++;
          } 
        }
      }
    } //End of cohort loop
    
    //Substract plant transpiration from soil moisture and calculate maximum difference in soil psi
    double maxDif = 0;
    for(int l=0;l<nlayers;l++) {
      W[l] =std::min(std::max(W[l]-(EplantVecInstant[l]/Water_FC[l]),0.0),1.0);
      psi[l] = theta2psi(clay[l], sand[l], W[l]*Theta_FC[l], om[l]);
      maxDif = std::max(maxDif, std::abs(psi[l]-psiBk[l]));
    }
    //Determine if supply functions need to be recalculated (maxDif > 0.1 MPa = 100 kPa)
    if(maxDif>0.1) {
      if(verbose) Rcout<<"R";
      for(int c=0;c<numCohorts;c++) {
        // Copy values from connected layers
        NumericVector Vc = NumericVector(nlayerscon[c]);
        NumericVector VCroot_kmaxc = NumericVector(nlayerscon[c]);
        NumericVector VGrhizo_kmaxc = NumericVector(nlayerscon[c]);
        NumericVector psic = NumericVector(nlayerscon[c]);
        NumericVector VG_nc = NumericVector(nlayerscon[c]);
        NumericVector VG_alphac= NumericVector(nlayerscon[c]);
        int cnt=0;
        for(int l=0;l<nlayers;l++) {
          if(layerConnected(c,l)) {
            Vc[cnt] = V(c,l);
            VCroot_kmaxc[cnt] = VCroot_kmax(c,l);
            VGrhizo_kmaxc[cnt] = VGrhizo_kmax(c,l);
            psic[cnt] = psi[l];
            VG_nc[cnt] = VG_n[l];
            VG_alphac[cnt] = VG_alpha[l];
            cnt++;
          }
        }
        // Build supply function
        double psiCav = 0.0;
        if(!cavitationRefill) {
          psiCav = xylemPsi(1.0-pEmb[c], 1.0, VCstem_c[c], VCstem_d[c]);//find water potential corresponding to this percentage of conductance loss
        }
        double minFlow = std::max(0.0,1000.0*(Gwmin[c]*(tmin+tmax)/2.0)/Patm);
        supplyNetworks[c] = supplyFunctionNetwork(psic,
                                                  VGrhizo_kmaxc,VG_nc,VG_alphac,
                                                  VCroot_kmaxc, VCroot_c[c],VCroot_d[c],
                                                  VCstem_kmax[c], VCstem_c[c],VCstem_d[c], 
                                                  psiCav,
                                                  minFlow, maxNsteps, psiStep, psiMax , ntrial, psiTol, ETol);
      }
      for(int l=0;l<nlayers;l++) psiBk[l] = psi[l];//Reset backup psi
    }
    
    //Soil longwave emmission
    emm_LWR_soil[n] =  SIGMA_W*pow(Tsoil[0]+273.16,4.0);
    
    //Surface absorved radiation
    SWRLWRcanin[n] = abs_SWR_can[n]+abs_LWR_can[n];
    //Latent heat (PROBLEM: does not include interception)
    LEcan_heat[n] = pow(10.0,6.0)*meteoland::utils_latentHeatVaporisation(Tcan[n])*sum(EplantVecInstant)/tstep; 
    //Canopy longwave emmission
    LWRcanout[n] = 0.97*SIGMA_W*pow(Tcan[n]+273.16,4.0);
    //Canopy convective heat exchange
    double ra = aerodynamicResistance(canopyHeight,wind); //Aerodynamic resistance to convective heat transfer
    Hcan_heat[n] = (meteoland::utils_airDensity(Tatm[n],Patm)*Cp_JKG*(Tcan[n]-Tatm[n]))/ra;
    //Soil-canopy turbulent heat exchange
    double wind2m = windSpeedMassmanExtinction(200.0, wind, LAIcell, canopyHeight);
    double rasoil = aerodynamicResistance(15.0,wind2m); //Aerodynamic resistance to convective heat transfer from soil
    double Hcansoil = (meteoland::utils_airDensity(Tcan[n],Patm)*Cp_JKG*(Tcan[n]-Tsoil[0]))/rasoil;
    //Soil LWR emmission
    LWRsoilout[n] = 0.97*SIGMA_W*pow(Tcan[n]+273.16,4.0);
    //Soil-canopy heat exchange
    G1_heat[n] = (LWRcanout[n] - LWRsoilout[n])*propCover + Hcansoil; //Only include a fraction equal to absorption
    
      
    //Canopy temperature changes
    Ebal[n] = SWRLWRcanin[n]-LWRcanout[n] - LEcan_heat[n] - Hcan_heat[n] - G1_heat[n];
    double canopyThermalCapacity = LAIcellmax*1000000.0;
    double Tcannext = Tcan[n]+ (tstep*Ebal[n]/canopyThermalCapacity);
    if(LAIcell==0.0) Tcannext = Tatm[n];
    //Apply changes
    if(n<(ntimesteps-1)) {
      Tcan[n+1] = Tcannext;
    }
    
    
    // //Soil input
    SWRLWRsoilin[n] =abs_SWR_soil[n] + abs_LWR_soil[n]+ (LWRcanout[n]*propCover);
    //Soil energy balance
    Ebalsoil[n] = SWRLWRsoilin[n] + Hcansoil - LWRsoilout[n]; //Here we use all LWRsoilout to include LWR escaping to atmosphere
    //Soil temperature changes
    NumericVector soilTchange = soilTemperatureChange(dVec, Tsoil, sand, clay, W, Theta_FC, Ebalsoil[n]);
    for(int l=0;l<nlayers;l++) Tsoil[l] = Tsoil[l] + (soilTchange[l]*tstep);
    if(n<(ntimesteps-1)) {
      Tsoil_mat(n+1,_)= Tsoil;
    }
    
    // Rcout<<n<<", Tatm: "<< Tatm[n]<< " Tcan: "<<Tcan[n]<<" soilT1 "<< Tsoil[0]<<"\n";
    

    //save canopy temperature
    canopyParams["Temp"] = Tcannext;
    
  } //End of timestep loop
  
  //Plant daily drought stress (from root collar mid-day water potential)
  for(int c=0;c<numCohorts;c++) {
    if(nlayerscon[c]>0) {
      PlantPsi[c] = minPsiLeaf[c];
      DDS[c] = Phe[c]*(1.0-xylemConductance(minPsiRoot[c], 1.0, VCstem_c[c], VCstem_d[c]));
      transpiration[c] = Eplant[c]; 
      
      //If there is no refill of cavitated conduits store the current level of xylem embolism
      if(!cavitationRefill) {
        pEmb[c] = std::max(DDS[c], pEmb[c]);
        DDS[c] = pEmb[c];
      }
    } else {
      DDS[c] = std::max(Phe[c]*(1.0 - pRootDisc[c]), pEmb[c]); //Disconnection limits the maximum stress
      transpiration[c] = 0.0; //No transpiration
      photosynthesis[c] = 0.0; //No photosynthesis
      PlantPsi[c] = xylemPsi(pRootDisc[c],1.0, VCroot_c[c], VCroot_d[c]); //Water potential is determined by pRootDisc
    }
  }
  
  
  //Potential soil evaporation
  double Rnground = 0.0;
  for(int n=0;n<ntimesteps;n++) {
    Rnground += (abs_SWR_soil[n] + abs_LWR_soil[n] - emm_LWR_soil[n])*tstep; //kJ·m-2 accumulate net soil radiation balance
  }
  Rnground = std::max(0.0,Rnground)/pow(10.0,3.0); //from kJ·m-2 to MJ·m-2
  double PETsoil = meteoland::penmanmonteith(200.0, elevation, tmin, tmax, rhmin, rhmax, Rnground, wind);
  //Evaporation from bare soil
  double Gsoil = soil["Gsoil"];
  double Ksoil = soil["Ksoil"];
  double Esoil = soilevaporation((Water_FC[0]*(1.0 - W[0])), PETsoil, Gsoil);
  NumericVector EsoilVec(nlayers,0.0);//Exponential decay to divide bare soil evaporation among layers
  for(int l=0;l<nlayers;l++) {
    double cumAnt = 0.0;
    double cumPost = 0.0;
    for(int l2=0;l2<l;l2++) cumAnt +=dVec[l2];
    cumPost = cumAnt+dVec[l];
    if(l<(nlayers-1)) EsoilVec[l] = Esoil*(exp(-Ksoil*cumAnt)-exp(-Ksoil*cumPost));
    else EsoilVec[l] = Esoil*exp(-Ksoil*cumAnt);
    W[l] =std::min(std::max(W[l]-(EsoilVec[l]/Water_FC[l]),0.0),1.0);
    psi[l] = theta2psi(clay[l], sand[l], W[l]*Theta_FC[l], om[l]);
  }
  

  DataFrame Tinst = DataFrame::create(_["SolarHour"] = solarHour, 
                                      _["Tatm"] = Tatm, _["Tcan"] = Tcan, _["Tsoil"] = Tsoil_mat,
                                      _["Rcanin"] = SWRLWRcanin, _["Rcanout"] = LWRcanout,
                                      _["LEcan"] = LEcan_heat, _["Hcan"] = Hcan_heat, _["G"] = G1_heat, _["Ebalcan"] = Ebal, _["Rsoilin"] = SWRLWRsoilin, _["Rsoilout"] = LWRsoilout,
                                      _["Ebalsoil"] = Ebalsoil);
  List l = List::create(_["PET"] = NA_REAL, _["NetPrec"] = NetPrec, _["Runon"] = runon, _["Infiltration"] = Infiltration, _["Runoff"] = Runoff, _["DeepDrainage"] = DeepDrainage,
                        _["LAIcell"] = LAIcell, _["Cm"] = Cm, _["Lground"] = 1.0-propCover,
                        _["EsoilVec"] = EsoilVec, _["EplantVec"] = EplantVec, _["psiVec"] = psi,
                        _["EplantCoh"] = Eplant, _["psiCoh"] = PlantPsi, _["DDS"] = DDS,
                        _["Einst"]=Einst, _["Aninst"]=Aninst,
                        _["PsiPlantinst"] = PsiPlantinst, _["Tinst"] = Tinst);
  return(l);
}

// [[Rcpp::export("swb.day")]]
List swbDay(List x, List soil, CharacterVector date, int doy, double tmin, double tmax, double rhmin, double rhmax, double rad, double wind, 
            double latitude, double elevation, double slope, double aspect,  
            double rain, double runon=0.0) {
  //Control parameters
  List control = x["control"];
  bool verbose = control["verbose"];
  String transpirationMode = control["transpirationMode"];
  std::string c = as<std::string>(date[0]);
  int J = meteoland::radiation_julianDay(std::atoi(c.substr(0, 4).c_str()),std::atoi(c.substr(5,2).c_str()),std::atoi(c.substr(8,2).c_str()));
  double delta = meteoland::radiation_solarDeclination(J);
  double solarConstant = meteoland::radiation_solarConstant(J);
  double tday = meteoland::utils_averageDaylightTemperature(tmin, tmax);
  double latrad = latitude * (PI/180.0);
  double asprad = aspect * (PI/180.0);
  double slorad = slope * (PI/180.0);
  double pet = meteoland::penman(latrad, elevation, slorad, asprad, J, tmin, tmax, rhmin, rhmax, rad, wind);
  NumericVector ER = er(IntegerVector::create(doy));
  double er = ER[0];
  
  //Plant input
  DataFrame above = Rcpp::as<Rcpp::DataFrame>(x["above"]);
  DataFrame cohorts = Rcpp::as<Rcpp::DataFrame>(x["cohorts"]);
  NumericVector SP = cohorts["SP"];
  NumericVector LAI_live = above["LAI_live"];
  NumericVector LAI_expanded = above["LAI_expanded"];
  int numCohorts = SP.size();
  
  //Base parameters
  DataFrame paramsBase = Rcpp::as<Rcpp::DataFrame>(x["paramsBase"]);
  NumericVector Sgdd = paramsBase["Sgdd"];
  double tmean = meteoland::utils_averageDaylightTemperature(tmin, tmax);
  List canopyParams = x["canopy"];
  double gddday = canopyParams["gdd"];
  if((tmean-5.0 < 0.0) & (doy>180)) {
    gddday = 0.0;
  } else if (doy<180){ //Only increase in the first part of the year
    if(tmean-5.0>0.0) gddday = gddday + (tmean-5.0);
  }
  canopyParams["gdd"] = gddday;
  //Update phenological status
  NumericVector phe = leafDevelopmentStatus(Sgdd, gddday);
  for(int j=0;j<numCohorts;j++) {
    LAI_expanded[j] = LAI_live[j]*phe[j];
  }

  List s;
  if(transpirationMode=="Simple") {
    s = swbDay1(x,soil, tday, pet, rain, er, runon, verbose);
  } else {
    s = swbDay2(x,soil, tmin, tmax, rhmin, rhmax, rad, wind, latitude, elevation, slope, aspect,
                solarConstant, delta, rain, er, runon, verbose);
  }
  // Rcout<<"hola4\n";
  s["PET"] = pet;
  s["Rain"] = rain;
  return(s);
}
  
NumericVector getTrackSpeciesTranspiration( NumericVector trackSpecies, NumericVector Eplant, DataFrame x) {
  int nTrackSpecies = trackSpecies.size();
  NumericVector Eplantsp(nTrackSpecies, 0.0);
  NumericVector SP = x["SP"];
  int nCoh = SP.size();
  int ts;
  for(int its =0;its<nTrackSpecies;its++) {
    ts = trackSpecies[its];
    for(int i=0;i<nCoh;i++) {
      if(SP[i]==ts) {
        Eplantsp[its] += Eplant[i];
      }
    }
  }
  return(Eplantsp);
}

NumericVector getTrackSpeciesDDS(NumericVector trackSpecies, NumericVector DDS, DataFrame x) {
  int nTrackSpecies = trackSpecies.size();
  NumericVector DDSsp(nTrackSpecies, 0.0);
  NumericVector LAI = x["LAI"];
  NumericVector SP = x["SP"];
  int nCoh = LAI.size();
  int ts;
  double laiSum;
  for(int its =0;its<nTrackSpecies;its++) {
    ts = trackSpecies[its];
    laiSum = 0.0;
    for(int i=0;i<nCoh;i++) {
      if(SP[i]==ts) {
        DDSsp[its] += DDS[i]*LAI[i];
        laiSum +=LAI[i];
      }
    }
    DDSsp = DDSsp/laiSum;
  }
  return(DDSsp);
}


// [[Rcpp::export(".swbgridDay")]]
List swbgridDay(CharacterVector lct, List xList, List soilList, 
                IntegerVector waterO, List queenNeigh, List waterQ,
                NumericVector gddVec, NumericVector petVec, NumericVector rainVec, 
                NumericVector erVec, NumericVector trackSpecies) {
  int nX = xList.size();
  int nTrackSpecies = trackSpecies.size();
  NumericVector NetPrec(nX,NA_REAL), Runon(nX,0.0), Infiltration(nX,NA_REAL);
  NumericVector Runoff(nX,NA_REAL), DeepDrainage(nX,NA_REAL), W1(nX,NA_REAL), W2(nX,NA_REAL);
  NumericVector W3(nX,NA_REAL), Esoil(nX,NA_REAL), Eplant(nX,NA_REAL);
  NumericMatrix Transpiration(nX, nTrackSpecies), DDS(nX, nTrackSpecies);
  double runoffExport = 0.0;
  for(int i=0;i<nX;i++) {
    //get next cell in order
    int iCell = waterO[i]-1; //Decrease index!!!!
    if((lct[iCell]=="Wildland") || (lct[iCell]=="Agriculture") ) {
      DataFrame x = Rcpp::as<Rcpp::DataFrame>(xList[iCell]);
      List soil = Rcpp::as<Rcpp::List>(soilList[iCell]);
      //Run daily soil water balance for the current cell
      List res = swbDay1(x, soil, gddVec[iCell], petVec[iCell], rainVec[iCell], erVec[iCell], Runon[iCell]);
      NetPrec[iCell] = res["NetPrec"];
      Runon[iCell] = res["Runon"];
      Infiltration[iCell] = res["Infiltration"];
      Runoff[iCell] = res["Runoff"];
      DeepDrainage[iCell] = res["DeepDrainage"];
      Esoil[iCell] = sum(Rcpp::as<Rcpp::NumericVector>(res["EsoilVec"]));
      NumericVector EplantCoh = res["EplantCoh"];
      NumericVector DDScell = res["DDS"];
      Eplant[iCell] = sum(EplantCoh);
      if(nTrackSpecies>0) {
        Transpiration(iCell,_) = getTrackSpeciesTranspiration(trackSpecies, EplantCoh, x);
        DDS(iCell,_) = getTrackSpeciesDDS(trackSpecies, DDScell, x);
      }
      NumericVector W = soil["W"];
      W1[iCell] = W[0];
      W2[iCell] = W[1];
      W3[iCell] = W[2];

      //Assign runoff to runon of neighbours
      double ri =  Runoff[iCell];
      if(ri>0.0) {
        IntegerVector ni = Rcpp::as<Rcpp::IntegerVector>(queenNeigh[iCell]);
        NumericVector qi = Rcpp::as<Rcpp::NumericVector>(waterQ[iCell]);
        if(ni.size()>0) {
          for(int j=0;j<ni.size();j++) Runon[ni[j]-1] += qi[j]*ri; //decrease index
        } else {
          runoffExport += ri; //If no suitable neighbours add ri to landscape export via runoff
        }
      }
    } else if(lct[iCell]=="Rock") {//all Precipitation becomes runoff if cell is rock outcrop
      Runoff[iCell] =  Runon[iCell]+rainVec[iCell];
      double ri = Runoff[iCell];
      if(ri>0.0) {
        IntegerVector ni = Rcpp::as<Rcpp::IntegerVector>(queenNeigh[iCell]);
        NumericVector qi = Rcpp::as<Rcpp::NumericVector>(waterQ[iCell]);
        if(ni.size()>0) {
          for(int j=0;j<ni.size();j++) Runon[ni[j]-1] += qi[j]*ri;//decrease index
        } else {
          runoffExport += ri; //If no suitable neighbours add ri to landscape export via runoff
        }
      }
    } else if(lct[iCell]=="Static") {
      // static cells receive water from other cells or Precipitation
      // but do not export to the atmosphere contribute nor to other cells.
      // Hence, water balance over the landscape is achieved by
      // adding this water to the landscape export via runoff.
      runoffExport += Runon[iCell] + rainVec[iCell];
    }
  }
  DataFrame waterBalance = DataFrame::create(_["NetPrec"] = NetPrec, _["Runon"] = Runon, _["Infiltration"] = Infiltration,
                                   _["Runoff"] = Runoff, _["DeepDrainage"] = DeepDrainage,
                                   _["W1"] = W1, _["W2"] = W2, _["W3"] = W3,
                                   _["Esoil"] = Esoil, _["Eplant"] = Eplant);
  return(List::create(_["WaterBalance"] = waterBalance,
                      _["RunoffExport"] = runoffExport,
                      _["Transpiration"] = Transpiration,
                      _["DDS"] = DDS));
}



void checkswbInput(List x, List soil, String transpirationMode) {
  if(!x.containsElementNamed("above")) stop("above missing in swbInput");
  DataFrame above = Rcpp::as<Rcpp::DataFrame>(x["above"]);
  if(!above.containsElementNamed("LAI_live")) stop("LAI_live missing in swbInput$above");
  if(!above.containsElementNamed("CR")) stop("CR missing in swbInput$above");
  if(!above.containsElementNamed("H")) stop("H missing in swbInput$above");
  
  if(!x.containsElementNamed("below")) stop("below missing in swbInput");
  List below = Rcpp::as<Rcpp::List>(x["below"]);
  if(!below.containsElementNamed("V")) stop("V missing in swbInput$below");
  if(transpirationMode=="Complex"){
    if(!below.containsElementNamed("VGrhizo_kmax")) stop("VGrhizo_kmax missing in swbInput$below");
    if(!below.containsElementNamed("VCroot_kmax")) stop("VCroot_kmax missing in swbInput$below");
  }  
  
  if(!x.containsElementNamed("paramsBase")) stop("paramsBase missing in swbInput");
  DataFrame paramsBase = Rcpp::as<Rcpp::DataFrame>(x["paramsBase"]);
  if(!paramsBase.containsElementNamed("Sgdd")) stop("Sgdd missing in swbInput$paramsBase");
  if(!paramsBase.containsElementNamed("k")) stop("k missing in swbInput$paramsBase");
  if(!paramsBase.containsElementNamed("g")) stop("g missing in swbInput$paramsBase");
  
  if(!x.containsElementNamed("paramsTransp")) stop("paramsTransp missing in swbInput");
  DataFrame paramsTransp = Rcpp::as<Rcpp::DataFrame>(x["paramsTransp"]);
  if(!paramsTransp.containsElementNamed("pRootDisc")) stop("pRootDisc missing in swbInput$paramsTransp");
  if(transpirationMode=="Simple") {
    if(!paramsTransp.containsElementNamed("Psi_Extract")) stop("Psi_Extract missing in swbInput$paramsTransp");
    if(!paramsTransp.containsElementNamed("WUE")) stop("WUE missing in swbInput$paramsTransp");
  } else if(transpirationMode=="Complex") {
    if(!paramsTransp.containsElementNamed("VCstem_kmax")) stop("VCstem_kmax missing in swbInput$paramsTransp");
    if(!paramsTransp.containsElementNamed("VCstem_c")) stop("VCstem_c missing in swbInput$paramsTransp");
    if(!paramsTransp.containsElementNamed("VCstem_d")) stop("VCstem_d missing in swbInput$paramsTransp");
    if(!paramsTransp.containsElementNamed("VCroot_c")) stop("VCroot_c missing in swbInput$paramsTransp");
    if(!paramsTransp.containsElementNamed("VCroot_d")) stop("VCroot_d missing in swbInput$paramsTransp");
    if(!paramsTransp.containsElementNamed("Gwmax")) stop("Gwmax missing in swbInput$paramsTransp");
    if(!paramsTransp.containsElementNamed("Vmax298")) stop("Vmax298 missing in swbInput$paramsTransp");
    if(!paramsTransp.containsElementNamed("Jmax298")) stop("Jmax298 missing in swbInput$paramsTransp");
  }
  if(transpirationMode=="Complex") {
    if(!soil.containsElementNamed("VG_n")) stop("VG_n missing in soil");
    if(!soil.containsElementNamed("VG_alpha")) stop("VG_alpha missing in soil");
  }
  if(!soil.containsElementNamed("W")) stop("W missing in soil");
  if(!soil.containsElementNamed("psi")) stop("psi missing in soil");
  if(!soil.containsElementNamed("dVec")) stop("dVec missing in soil");
  if(!soil.containsElementNamed("Theta_FC")) stop("Theta_FC missing in soil");
  if(!soil.containsElementNamed("macro")) stop("macro missing in soil");
  if(!soil.containsElementNamed("clay")) stop("clay missing in soil");
  if(!soil.containsElementNamed("sand")) stop("sand missing in soil");
}

// [[Rcpp::export("swb")]]
List swb(List x, List soil, DataFrame meteo, double latitude = NA_REAL, double elevation = NA_REAL, double slope = NA_REAL, double aspect = NA_REAL) {
  List control = x["control"];
  String transpirationMode = control["transpirationMode"];
  bool verbose = control["verbose"];
  
  checkswbInput(x, soil, transpirationMode);

  //Meteorological input    
  NumericVector MinTemperature, MaxTemperature;
  NumericVector MinRelativeHumidity, MaxRelativeHumidity;
  NumericVector PET;
  NumericVector Radiation, WindSpeed;
  IntegerVector DOY = meteo["DOY"];
  NumericVector Precipitation = meteo["Precipitation"];
  NumericVector MeanTemperature = meteo["MeanTemperature"];
  int numDays = Precipitation.size();
  if(transpirationMode=="Simple") {
    PET = meteo["PET"];
  } else if(transpirationMode=="Complex") {
    if(NumericVector::is_na(latitude)) stop("Value for 'latitude' should not be missing.");
    if(NumericVector::is_na(elevation)) stop("Value for 'elevation' should not be missing.");
    MinTemperature = meteo["MinTemperature"];
    MaxTemperature = meteo["MaxTemperature"];
    MinRelativeHumidity = meteo["MinRelativeHumidity"];
    MaxRelativeHumidity = meteo["MaxRelativeHumidity"];
    Radiation = meteo["Radiation"];
    WindSpeed = meteo["WindSpeed"];
    PET = NumericVector(numDays);
  }
  CharacterVector dateStrings = meteo.attr("row.names");
  
  //Canpopy parameters
  List canopyParams = x["canopy"];
  
  //Calculate GDD (taking into account initial sum)
  NumericVector GDD = gdd(DOY, MeanTemperature, 5.0, canopyParams["gdd"]);
  
  NumericVector ER = er(DOY);
  
  
  //Plant input
  DataFrame above = Rcpp::as<Rcpp::DataFrame>(x["above"]);
  DataFrame cohorts = Rcpp::as<Rcpp::DataFrame>(x["cohorts"]);
  NumericVector SP = cohorts["SP"];
  NumericVector LAI_live = above["LAI_live"];
  NumericVector LAI_expanded = above["LAI_expanded"];
  int numCohorts = SP.size();
  

  //Base parameters
  DataFrame paramsBase = Rcpp::as<Rcpp::DataFrame>(x["paramsBase"]);
  NumericVector Sgdd = paramsBase["Sgdd"];
  
  //Soil input
  NumericVector W = soil["W"];
  int nlayers = W.size();
  NumericVector Water_FC = waterFC(soil);

  //Water balance output variables
  NumericVector Esoil(numDays);
  NumericVector LAIcell(numDays);
  NumericVector Cm(numDays);
  NumericVector Lground(numDays);
  NumericVector Runoff(numDays);
  NumericVector NetPrec(numDays);
  NumericVector Interception(numDays);
  NumericVector Infiltration(numDays);
  NumericVector DeepDrainage(numDays);
  NumericMatrix Eplantdays(numDays, nlayers);
  NumericMatrix Wdays(numDays, nlayers); //Soil moisture content in relation to field capacity
  NumericMatrix psidays(numDays, nlayers);
  NumericMatrix MLdays(numDays, nlayers);
  NumericVector MLTot(numDays, 0.0);
  
  //Temperature output variables
  NumericVector Tatm_mean(numDays, NA_REAL);
  NumericVector Tatm_min(numDays, NA_REAL);
  NumericVector Tatm_max(numDays, NA_REAL);
  NumericVector Tcan_mean(numDays, NA_REAL);
  NumericVector Tcan_min(numDays, NA_REAL);
  NumericVector Tcan_max(numDays, NA_REAL);
  NumericVector Tsoil_mean(numDays, NA_REAL);
  NumericVector Tsoil_min(numDays, NA_REAL);
  NumericVector Tsoil_max(numDays, NA_REAL);
  
  //Plant output variables
  NumericMatrix PlantPsi(numDays, numCohorts);
  NumericMatrix PlantStress(numDays, numCohorts);
  NumericMatrix PlantTranspiration(numDays, numCohorts);
  NumericMatrix PlantPhotosynthesis(numDays, numCohorts);
  NumericVector EplantCohTot(numCohorts, 0.0);
  
  
  Wdays(0,_) = W;

  if(verbose) {
    for(int l=0;l<nlayers;l++) Rcout << "W"<<(l+1)<<"i:"<< round(100*W[l])/100<<" ";
    Rcout<<"\n";
  }

  if(verbose) Rcout << "Daily balance:";
  NumericVector Eplanttot(numDays,0.0);
  List s;
  for(int i=0;i<numDays;i++) {
      if(verbose) Rcout<<".";
      canopyParams["gdd"] = GDD[i];//Update phenological status
      NumericVector phe = leafDevelopmentStatus(Sgdd, GDD[i]);
      for(int j=0;j<numCohorts;j++) {
        LAI_expanded[j] = LAI_live[j]*phe[j];
      }
      //Transpiration
      if(transpirationMode=="Simple") {
        s = swbDay1(x, soil, MeanTemperature[i], PET[i], Precipitation[i], ER[i], 0.0, false); //No Runon in simulations for a single cell
      } else if(transpirationMode=="Complex") {
        std::string c = as<std::string>(dateStrings[i]);
        int J = meteoland::radiation_julianDay(std::atoi(c.substr(0, 4).c_str()),std::atoi(c.substr(5,2).c_str()),std::atoi(c.substr(8,2).c_str()));
        double delta = meteoland::radiation_solarDeclination(J);
        double solarConstant = meteoland::radiation_solarConstant(J);
        s = swbDay2(x, soil, MinTemperature[i], MaxTemperature[i], 
                         MinRelativeHumidity[i], MaxRelativeHumidity[i], Radiation[i], WindSpeed[i], 
                         latitude, elevation, slope, aspect, solarConstant, delta, Precipitation[i], ER[i], 0.0, verbose);
        PET[i] = s["PET"];
        DataFrame Tinst = Rcpp::as<Rcpp::DataFrame>(s["Tinst"]);
        NumericVector Tcan = Rcpp::as<Rcpp::NumericVector>(Tinst["Tcan"]);
        NumericVector Tsoil = Rcpp::as<Rcpp::NumericVector>(Tinst["Tsoil.1"]);
        int ntimesteps = Tcan.length();
        Tatm_min[i] = MinTemperature[i];
        Tatm_max[i] = MaxTemperature[i];
        Tatm_mean[i] = MeanTemperature[i];
        Tcan_min[i] = min(Tcan);
        Tcan_max[i] = max(Tcan);
        Tcan_mean[i] = sum(Tcan)/((double) ntimesteps);
        Tsoil_min[i] = min(Tsoil);
        Tsoil_max[i] = max(Tsoil);
        Tsoil_mean[i] = sum(Tsoil)/((double) ntimesteps);
      }
      Lground[i] = s["Lground"];
      Esoil[i] = sum(Rcpp::as<Rcpp::NumericVector>(s["EsoilVec"]));
      LAIcell[i] = s["LAIcell"];
      Cm[i] = s["Cm"];
      DeepDrainage[i] = s["DeepDrainage"];
      Infiltration[i] = s["Infiltration"];
      Runoff[i] = s["Runoff"];
      NetPrec[i] = s["NetPrec"];
      Interception[i] = Precipitation[i]-NetPrec[i];
      NumericVector psi = s["psiVec"];
      psidays(i,_) = psi;

      NumericVector EplantCoh = s["EplantCoh"];
      NumericVector EplantVec = s["EplantVec"];
      Eplantdays(i,_) = EplantVec;
      PlantPhotosynthesis(i,_) = Rcpp::as<Rcpp::NumericVector>(x["Photosynthesis"]);
      PlantTranspiration(i,_) = EplantCoh;
      PlantStress(i,_) = Rcpp::as<Rcpp::NumericVector>(s["DDS"]);
      PlantPsi(i,_) = Rcpp::as<Rcpp::NumericVector>(s["psiCoh"]);
      EplantCohTot = EplantCohTot + EplantCoh;
      Eplanttot[i] = sum(EplantCoh);
      
      if(i<(numDays-1)) Wdays(i+1,_) = W;
  }
  if(verbose) Rcout << "done\n";
  
  for(int l=0;l<nlayers;l++) {
    MLdays(_,l) = Wdays(_,l)*Water_FC[l]; 
    MLTot = MLTot + MLdays(_,l);
  }

  NumericVector Etot = Eplanttot+Esoil;
  
  if(verbose) {
    double Precipitationsum = sum(Precipitation);
    double NetPrecsum = sum(NetPrec);
    double Interceptionsum = sum(Interception);
    double Esoilsum = sum(Esoil);
    double Runoffsum  = sum(Runoff);
    double Infiltrationsum  = sum(Infiltration);
    double DeepDrainagesum = sum(DeepDrainage);

    double Eplantsum = sum(Eplanttot);
    
    Rcout<<"Total Precipitation (mm) "  <<round(Precipitationsum) <<"\n";
    Rcout<<"Interception (mm) " << round(Interceptionsum)  <<" Net Prec (mm) " << round(NetPrecsum) <<"\n";
    Rcout<<"Infiltration (mm) " << round(Infiltrationsum)  <<
      " Runoff (mm) " << round(Runoffsum) <<
        " Deep drainage (mm) "  << round(DeepDrainagesum)  <<"\n";
    Rcout<<"Esoil (mm) " << round(Esoilsum) <<
      " Eplant (mm) "  <<round(Eplantsum) <<"\n";
    for(int l=0;l<nlayers;l++) Rcout << "W"<<(l+1)<<"f:"<< round(100*W[l])/100<<" ";
    Rcout<<"\n";
    Rcout<<"Final volume: "<< round(MLTot[numDays-1])<<"\n\n";
    
  }
  if(verbose) Rcout<<"Building SWB and DWB output ...";
  
   Rcpp::DataFrame SWB = DataFrame::create(_["W"]=Wdays, _["ML"]=MLdays,_["MLTot"]=MLTot,_["psi"]=psidays);
   Rcpp::DataFrame DWB = DataFrame::create(_["LAIcell"]=LAIcell, _["Cm"]=Cm, _["Lground"] = Lground, _["PET"]=PET, 
                                           _["Precipitation"] = Precipitation, _["NetPrec"]=NetPrec,_["Infiltration"]=Infiltration, _["Runoff"]=Runoff, _["DeepDrainage"]=DeepDrainage, 
                                           _["Etot"]=Etot,_["Esoil"]=Esoil,
                                           _["Eplanttot"]=Eplanttot,
                                           _["Eplant"]=Eplantdays);
  
   SWB.attr("row.names") = meteo.attr("row.names") ;
   DWB.attr("row.names") = meteo.attr("row.names") ;
  if(verbose) Rcout<<"plant output ...";
  
  PlantTranspiration.attr("dimnames") = List::create(meteo.attr("row.names"), above.attr("row.names"));
  PlantStress.attr("dimnames") = List::create(meteo.attr("row.names"), above.attr("row.names")) ;
  PlantPsi.attr("dimnames") = List::create(meteo.attr("row.names"), above.attr("row.names")) ;
  PlantPhotosynthesis.attr("dimnames") = List::create(meteo.attr("row.names"), above.attr("row.names")) ;
  
  if(verbose) Rcout<<"list ...";

  DataFrame DTB = DataFrame::create(_["Tatm_mean"] = Tatm_mean, _["Tatm_min"] = Tatm_min, _["Tatm_max"] = Tatm_max,
                                    _["Tcan_mean"] = Tcan_mean, _["Tcan_min"] = Tcan_min, _["Tcan_max"] = Tcan_max,
                                    _["Tsoil_mean"] = Tsoil_mean, _["Tsoil_min"] = Tsoil_min, _["Tsoil_max"] = Tsoil_max);  
  List l = List::create(Named("control") = control,
                        Named("cohorts") = clone(cohorts),
                        Named("NumSoilLayers") = nlayers,
                        Named("DailyBalance")=DWB, 
                        Named("SoilWaterBalance")=SWB,
                        Named("TemperatureBalance")= DTB,
                        Named("PlantTranspiration") = PlantTranspiration,
                        Named("PlantPhotosynthesis") = PlantPhotosynthesis,
                        Named("PlantPsi") = PlantPsi, 
                        Named("PlantStress") = PlantStress);
  l.attr("class") = CharacterVector::create("swb","list");
  if(verbose) Rcout<<"done.\n";
  return(l);
}
