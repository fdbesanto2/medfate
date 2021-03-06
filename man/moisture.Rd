\encoding{UTF-8}
\name{tissuemoisture}
\alias{moisture_symplasticRWC}
\alias{moisture_symplasticPsi}
\alias{moisture_apoplasticRWC}
\alias{moisture_apoplasticPsi}
\alias{moisture_tissueRWC}
\alias{moisture_tissueFMC}
\alias{moisture_cohortFMC}
\alias{moisture_pressureVolumeCurvePlot}
\alias{moisture_turgorLossPoint}
\title{
Tissue moisture functions
}
\description{
Set of functions used to calculate tissue moisture from water potential and viceversa.
}
\usage{
moisture_symplasticRWC(psiSym, pi0, epsilon)
moisture_symplasticPsi(RWC, pi0, epsilon)
moisture_apoplasticRWC(psiApo, c, d)
moisture_apoplasticPsi(RWC, c, d)
moisture_turgorLossPoint(pi0, epsilon) 
moisture_tissueRWC(psiSym, pi0, epsilon, 
                   psiApo, c, d, 
                   af, femb = 0)
moisture_tissueFMC(RWC, density, d0 = 1.54)
moisture_cohortFMC(spwb)
moisture_pressureVolumeCurvePlot(x, segment="leaf", 
                    fraction = "all",
                    psiVec =  seq(-0.1, -8.0, by =-0.01),
                    speciesNames = FALSE)                    
}
\arguments{
  \item{psiSym, psiApo}{Symplastic or apoplastic water potential (MPa).}
  \item{RWC}{Relative water content [0-1].}
  \item{pi0}{Full turgor osmotic potential (MPa).}
  \item{epsilon}{Bulk modulus of elasticity (MPa).}
  \item{c,d}{Parameters of the xylem vulnerability curve.}
  \item{af}{Apoplastic fraction (proportion) in the segment (e.g. leaf or stem).}
  \item{femb}{Fraction of embolized conduits.}
  \item{x}{An object of class \code{\link{spwbInput}}.}
  \item{segment}{Segment whose relative water content curve to plot, either \code{"stem"} or \code{"leaf"}.}
  \item{fraction}{ Tissue fraction, either \code{"symplastic"}, \code{"apoplastic"} or \code{"all"}.}
  \item{psiVec}{Vector of water potential values to evaluate for the pressure-volume curve.}
  \item{speciesNames}{A flag to indicate the use of species names instead of cohort names in plots.}
  \item{density}{Tissue density (g·cm-1).}
  \item{d0}{Matric density (g·cm-1).}
  \item{spwb}{Object returned by function \code{\link{spwb}} or \code{\link{growth}}.}
}
\details{
Details of the tissue moisture calculations are given in a vignette. Function \code{moisture_cohortFMC()} calculates the fuel moisture content of leaves and twigs of each cohort, from the results of soil water balance.
}
\value{
Values returned for each function are:
\itemize{
\item{\code{moisture_symplasticRWC}: Relative water content [0-1] of the symplastic fraction.}
\item{\code{moisture_apoplasticRWC}: Relative water content [0-1] of the apoplastic fraction.}
\item{\code{moisture_symplasticWaterPotential}: Water potential (in MPa) of the symplastic fraction.}
\item{\code{moisture_apoplasticWaterPotential}: Water potential (in MPa) of the apoplastic fraction.}
\item{\code{moisture_turgorLossPoint}: Water potential (in MPa) corresponding to turgor loss point.}
\item{\code{moisture_segmentRWC}: Segment relative water content [0-1].}
}
}
\references{
Bartlett, M.K., Scoffoni, C., Sack, L. 2012. The determinants of leaf turgor loss point and prediction of drought tolerance of species and biomes: a global meta-analysis. Ecology Letters 15: 393–405.

\enc{Hölttä}{Holtta}, T., Cochard, H., Nikinmaa, E., Mencuccini, M. 2009. Capacitive effect of cavitation in xylem conduits: Results from a dynamic model. Plant, Cell and Environment 32: 10–21.

Martin-StPaul, N., Delzon, S., Cochard, H. 2017. Plant resistance to drought depends on timely stomatal closure. Ecology Letters 20: 1437–1447.
}
\author{
Miquel De \enc{Cáceres}{Caceres} Ainsa, CTFC
}
\seealso{
\code{\link{hydraulics_psi2K}}, \code{\link{hydraulics_supplyFunctionPlot}}, \code{\link{spwb}}, \code{\link{soil}}
}
\examples{
psi = seq(-10,0, by=0.1)
rwc_s = rep(NA, length(psi))
for(i in 1:length(psi)) rwc_s[i] = moisture_symplasticRWC(psi[i],-3,12)
plot(psi, rwc_s, type="l", xlab="Water potential (MPa)", ylab = "Symplasmic RWC")
}
