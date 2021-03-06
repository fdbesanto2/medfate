# Generated by using Rcpp::compileAttributes() -> do not edit by hand
# Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

biophysics_radiationDiurnalPattern <- function(t, daylength) {
    .Call(`_medfate_radiationDiurnalPattern`, t, daylength)
}

biophysics_temperatureDiurnalPattern <- function(t, tmin, tmax, tminPrev, tmaxPrev, tminNext, daylength) {
    .Call(`_medfate_temperatureDiurnalPattern`, t, tmin, tmax, tminPrev, tmaxPrev, tminNext, daylength)
}

biophysics_leafTemperature <- function(absRad, airTemperature, u, E, leafWidth = 1.0) {
    .Call(`_medfate_leafTemperature`, absRad, airTemperature, u, E, leafWidth)
}

biophysics_waterDynamicViscosity <- function(temp) {
    .Call(`_medfate_waterDynamicViscosity`, temp)
}

carbon_sugarStarchDynamicsLeaf <- function(sugarConc, starchConc, eqSugarConc) {
    .Call(`_medfate_sugarStarchDynamicsLeaf`, sugarConc, starchConc, eqSugarConc)
}

carbon_sugarStarchDynamicsStem <- function(sugarConc, starchConc, eqSugarConc) {
    .Call(`_medfate_sugarStarchDynamicsStem`, sugarConc, starchConc, eqSugarConc)
}

carbon_osmoticWaterPotential <- function(sugarConc, temp, nonSugarConc) {
    .Call(`_medfate_osmoticWaterPotential`, sugarConc, temp, nonSugarConc)
}

carbon_sugarConcentration <- function(osmoticWP, temp, nonSugarConc) {
    .Call(`_medfate_sugarConcentration`, osmoticWP, temp, nonSugarConc)
}

carbon_relativeSapViscosity <- function(sugarConc, temp) {
    .Call(`_medfate_relativeSapViscosity`, sugarConc, temp)
}

carbon_leafStructuralBiomass <- function(LAI, N, SLA) {
    .Call(`_medfate_leafStructuralBiomass`, LAI, N, SLA)
}

carbon_leafStarchCapacity <- function(LAI, N, SLA, leafDensity) {
    .Call(`_medfate_leafStarchCapacity`, LAI, N, SLA, leafDensity)
}

carbon_sapwoodStructuralBiomass <- function(SA, H, Z, woodDensity) {
    .Call(`_medfate_sapwoodStructuralBiomass`, SA, H, Z, woodDensity)
}

carbon_sapwoodStructuralLivingBiomass <- function(SA, H, Z, woodDensity, vessel2sapwood) {
    .Call(`_medfate_sapwoodStructuralLivingBiomass`, SA, H, Z, woodDensity, vessel2sapwood)
}

carbon_sapwoodStarchCapacity <- function(SA, H, Z, woodDensity, vessel2sapwood) {
    .Call(`_medfate_sapwoodStarchCapacity`, SA, H, Z, woodDensity, vessel2sapwood)
}

.criticalFirelineIntensity <- function(CBH, M) {
    .Call(`_medfate_criticalFirelineIntensity`, CBH, M)
}

fire_FCCS <- function(FCCSpropsSI, MliveSI = as.numeric( c(90, 90, 60)), MdeadSI = as.numeric( c(6, 6, 6, 6, 6)), slope = 0.0, windSpeedSI = 11.0) {
    .Call(`_medfate_FCCSbehaviour`, FCCSpropsSI, MliveSI, MdeadSI, slope, windSpeedSI)
}

fire_Rothermel <- function(modeltype, wSI, sSI, delta, mx_dead, hSI, mSI, u, windDir, slope, aspect) {
    .Call(`_medfate_rothermel`, modeltype, wSI, sSI, delta, mx_dead, hSI, mSI, u, windDir, slope, aspect)
}

plant_ID <- function(x) {
    .Call(`_medfate_cohortIDs`, x)
}

plant_parameter <- function(x, SpParams, parName) {
    .Call(`_medfate_cohortNumericParameter`, x, SpParams, parName)
}

plant_characterParameter <- function(x, SpParams, parName) {
    .Call(`_medfate_cohortCharacterParameter`, x, SpParams, parName)
}

plant_species <- function(x) {
    .Call(`_medfate_cohortSpecies`, x)
}

plant_speciesName <- function(x, SpParams) {
    .Call(`_medfate_cohortSpeciesName`, x, SpParams)
}

.treeBasalArea <- function(N, dbh) {
    .Call(`_medfate_treeBasalArea`, N, dbh)
}

plant_basalArea <- function(x) {
    .Call(`_medfate_cohortBasalArea`, x)
}

species_basalArea <- function(x, SpParams) {
    .Call(`_medfate_speciesBasalArea`, x, SpParams)
}

plant_largerTreeBasalArea <- function(x) {
    .Call(`_medfate_cohortLargerTreeBasalArea`, x)
}

stand_basalArea <- function(x) {
    .Call(`_medfate_standBasalArea`, x)
}

plant_density <- function(x, SpParams, mode = "MED") {
    .Call(`_medfate_cohortDensity`, x, SpParams, mode)
}

species_density <- function(x, SpParams, mode = "MED") {
    .Call(`_medfate_speciesDensity`, x, SpParams, mode)
}

plant_height <- function(x) {
    .Call(`_medfate_cohortHeight`, x)
}

.shrubCrownRatio <- function(SP, SpParams) {
    .Call(`_medfate_shrubCrownRatio`, SP, SpParams)
}

plant_crownRatio <- function(x, SpParams, mode = "MED") {
    .Call(`_medfate_cohortCrownRatio`, x, SpParams, mode)
}

plant_crownBaseHeight <- function(x, SpParams, mode = "MED") {
    .Call(`_medfate_cohortCrownBaseHeight`, x, SpParams, mode)
}

plant_crownLength <- function(x, SpParams, mode = "MED") {
    .Call(`_medfate_cohortCrownLength`, x, SpParams, mode)
}

plant_foliarBiomass <- function(x, SpParams, gdd = NA_real_, mode = "MED") {
    .Call(`_medfate_cohortFoliarBiomass`, x, SpParams, gdd, mode)
}

species_foliarBiomass <- function(x, SpParams, gdd = NA_real_, mode = "MED") {
    .Call(`_medfate_speciesFoliarBiomass`, x, SpParams, gdd, mode)
}

stand_foliarBiomass <- function(x, SpParams, gdd = NA_real_, mode = "MED") {
    .Call(`_medfate_standFoliarBiomass`, x, SpParams, gdd, mode)
}

.shrubCover <- function(x, excludeMinHeight = 0.0) {
    .Call(`_medfate_shrubCover`, x, excludeMinHeight)
}

plant_cover <- function(x) {
    .Call(`_medfate_cohortCover`, x)
}

species_cover <- function(x, SpParams) {
    .Call(`_medfate_speciesCover`, x, SpParams)
}

.shrubCrownPhytovolume <- function(SP, Cover, H, CR, SpParams) {
    .Call(`_medfate_shrubCrownPhytovolume`, SP, Cover, H, CR, SpParams)
}

plant_phytovolume <- function(x, SpParams) {
    .Call(`_medfate_cohortPhytovolume`, x, SpParams)
}

species_phytovolume <- function(x, SpParams) {
    .Call(`_medfate_speciesPhytovolume`, x, SpParams)
}

stand_phytovolume <- function(x, SpParams) {
    .Call(`_medfate_standPhytovolume`, x, SpParams)
}

plant_fuel <- function(x, SpParams, gdd = NA_real_, includeDead = TRUE, mode = "MED") {
    .Call(`_medfate_cohortFuel`, x, SpParams, gdd, includeDead, mode)
}

species_fuel <- function(x, SpParams, gdd = NA_real_, includeDead = TRUE, mode = "MED") {
    .Call(`_medfate_speciesFuel`, x, SpParams, gdd, includeDead, mode)
}

stand_fuel <- function(x, SpParams, gdd = NA_real_, includeDead = TRUE, mode = "MED") {
    .Call(`_medfate_standFuel`, x, SpParams, gdd, includeDead, mode)
}

plant_equilibriumLeafLitter <- function(x, SpParams, AET = 800) {
    .Call(`_medfate_cohortEquilibriumLeafLitter`, x, SpParams, AET)
}

plant_equilibriumSmallBranchLitter <- function(x, SpParams, smallBranchDecompositionRate = 0.81) {
    .Call(`_medfate_cohortEquilibriumSmallBranchLitter`, x, SpParams, smallBranchDecompositionRate)
}

plant_LAI <- function(x, SpParams, gdd = NA_real_, mode = "MED") {
    .Call(`_medfate_cohortLAI`, x, SpParams, gdd, mode)
}

species_LAI <- function(x, SpParams, gdd = NA_real_, mode = "MED") {
    .Call(`_medfate_speciesLAI`, x, SpParams, gdd, mode)
}

stand_LAI <- function(x, SpParams, gdd = NA_real_, mode = "MED") {
    .Call(`_medfate_standLAI`, x, SpParams, gdd, mode)
}

.LAIdistributionVectors <- function(z, LAI, H, CR) {
    .Call(`_medfate_LAIdistributionVectors`, z, LAI, H, CR)
}

.LAIdistribution <- function(z, x, SpParams, gdd = NA_real_, mode = "MED") {
    .Call(`_medfate_LAIdistribution`, z, x, SpParams, gdd, mode)
}

.LAIprofileVectors <- function(z, LAI, H, CR) {
    .Call(`_medfate_LAIprofileVectors`, z, LAI, H, CR)
}

.LAIprofile <- function(z, x, SpParams, gdd = NA_real_, mode = "MED") {
    .Call(`_medfate_LAIprofile`, z, x, SpParams, gdd, mode)
}

forest2aboveground <- function(x, SpParams, gdd = NA_real_, mode = "MED") {
    .Call(`_medfate_forest2aboveground`, x, SpParams, gdd, mode)
}

forest2belowground <- function(x, soil, SpParams) {
    .Call(`_medfate_forest2belowground`, x, soil, SpParams)
}

.fuelConditions <- function(airTemp, airHumidity, fuelRadiation, fuelWindSpeed) {
    .Call(`_medfate_fuelConditions`, airTemp, airHumidity, fuelRadiation, fuelWindSpeed)
}

.EMCdesorption <- function(fuelTemperature, fuelHumidity) {
    .Call(`_medfate_EMCdesorption`, fuelTemperature, fuelHumidity)
}

.EMCadsorption <- function(fuelTemperature, fuelHumidity) {
    .Call(`_medfate_EMCadsorption`, fuelTemperature, fuelHumidity)
}

.EMCSimard <- function(fuelTemperature, fuelHumidity) {
    .Call(`_medfate_EMCSimard`, fuelTemperature, fuelHumidity)
}

.woodyFuelProfile <- function(z, x, SpParams, gdd = NA_real_, mode = "MED") {
    .Call(`_medfate_woodyFuelProfile`, z, x, SpParams, gdd, mode)
}

.layerCohortFuelLoading <- function(minHeight, maxHeight, cohortLoading, H, CR) {
    .Call(`_medfate_layerCohortFuelLoading`, minHeight, maxHeight, cohortLoading, H, CR)
}

.layerFuelLoading <- function(minHeight, maxHeight, cohortLoading, H, CR) {
    .Call(`_medfate_layerFuelLoading`, minHeight, maxHeight, cohortLoading, H, CR)
}

.layerLAI <- function(minHeight, maxHeight, cohortLAI, H, CR) {
    .Call(`_medfate_layerLAI`, minHeight, maxHeight, cohortLAI, H, CR)
}

.layerFuelAverageSpeciesParameter <- function(spParName, minHeight, maxHeight, x, SpParams, gdd = NA_real_) {
    .Call(`_medfate_layerFuelAverageSpeciesParameter`, spParName, minHeight, maxHeight, x, SpParams, gdd)
}

.layerFuelAverageParameter <- function(minHeight, maxHeight, cohortParameter, cohortLoading, H, CR) {
    .Call(`_medfate_layerFuelAverageParameter`, minHeight, maxHeight, cohortParameter, cohortLoading, H, CR)
}

.layerFuelAverageCrownLength <- function(minHeight, maxHeight, cohortCrownLength, cohortLoading, H, CR) {
    .Call(`_medfate_layerFuelAverageCrownLength`, minHeight, maxHeight, cohortCrownLength, cohortLoading, H, CR)
}

fuel_stratification <- function(object, SpParams, gdd = NA_real_, mode = "MED", heightProfileStep = 10.0, maxHeightProfile = 5000.0, bulkDensityThreshold = 0.05) {
    .Call(`_medfate_fuelLiveStratification`, object, SpParams, gdd, mode, heightProfileStep, maxHeightProfile, bulkDensityThreshold)
}

fuel_FCCS <- function(object, ShrubCover, CanopyCover, SpParams, cohortFMC = as.numeric( c()), gdd = NA_real_, mode = "MED", heightProfileStep = 10.0, maxHeightProfile = 5000, bulkDensityThreshold = 0.05) {
    .Call(`_medfate_FCCSproperties`, object, ShrubCover, CanopyCover, SpParams, cohortFMC, gdd, mode, heightProfileStep, maxHeightProfile, bulkDensityThreshold)
}

growth_day <- function(x, soil, date, tmin, tmax, rhmin, rhmax, rad, wind, latitude, elevation, slope, aspect, prec, runon = 0.0) {
    .Call(`_medfate_growthDay`, x, soil, date, tmin, tmax, rhmin, rhmax, rad, wind, latitude, elevation, slope, aspect, prec, runon)
}

growth <- function(x, soil, meteo, latitude, elevation = NA_real_, slope = NA_real_, aspect = NA_real_) {
    .Call(`_medfate_growth`, x, soil, meteo, latitude, elevation, slope, aspect)
}

hydraulics_psi2K <- function(psi, Psi_extract, ws = 3.0) {
    .Call(`_medfate_Psi2K`, psi, Psi_extract, ws)
}

hydraulics_K2Psi <- function(K, Psi_extract, ws = 3.0) {
    .Call(`_medfate_K2Psi`, K, Psi_extract, ws)
}

hydraulics_averagePsi <- function(psi, v, c, d) {
    .Call(`_medfate_averagePsi`, psi, v, c, d)
}

hydraulics_xylemConductance <- function(psi, kxylemmax, c, d) {
    .Call(`_medfate_xylemConductance`, psi, kxylemmax, c, d)
}

hydraulics_xylemPsi <- function(kxylem, kxylemmax, c, d) {
    .Call(`_medfate_xylemPsi`, kxylem, kxylemmax, c, d)
}

hydraulics_psiCrit <- function(c, d, pCrit = 0.001) {
    .Call(`_medfate_psiCrit`, c, d, pCrit)
}

hydraulics_vanGenuchtenConductance <- function(psi, krhizomax, n, alpha) {
    .Call(`_medfate_vanGenuchtenConductance`, psi, krhizomax, n, alpha)
}

hydraulics_correctConductanceForViscosity <- function(kxylem, temp) {
    .Call(`_medfate_correctConductanceForViscosity`, kxylem, temp)
}

.Egamma <- function(psi, kxylemmax, c, d, psiCav = 0.0) {
    .Call(`_medfate_Egamma`, psi, kxylemmax, c, d, psiCav)
}

.Egammainv <- function(Eg, kxylemmax, c, d, psiCav = 0.0) {
    .Call(`_medfate_Egammainv`, Eg, kxylemmax, c, d, psiCav)
}

hydraulics_EXylem <- function(psiPlant, psiUpstream, kxylemmax, c, d, allowNegativeFlux = TRUE, psiCav = 0.0) {
    .Call(`_medfate_EXylem`, psiPlant, psiUpstream, kxylemmax, c, d, allowNegativeFlux, psiCav)
}

hydraulics_E2psiXylem <- function(E, psiUpstream, kxylemmax, c, d, psiCav = 0.0) {
    .Call(`_medfate_E2psiXylem`, E, psiUpstream, kxylemmax, c, d, psiCav)
}

hydraulics_E2psiXylemUp <- function(E, psiDownstream, kxylemmax, c, d, psiCav = 0.0) {
    .Call(`_medfate_E2psiXylemUp`, E, psiDownstream, kxylemmax, c, d, psiCav)
}

hydraulics_EVanGenuchten <- function(psiRhizo, psiSoil, krhizomax, n, alpha, l = 0.5) {
    .Call(`_medfate_EVanGenuchten`, psiRhizo, psiSoil, krhizomax, n, alpha, l)
}

hydraulics_ECrit <- function(psiUpstream, kxylemmax, c, d, pCrit = 0.001) {
    .Call(`_medfate_ECrit`, psiUpstream, kxylemmax, c, d, pCrit)
}

hydraulics_ECapacitance <- function(psi, psiPrev, PLCprev, V, fapo, c, d, pi0, eps, timestep) {
    .Call(`_medfate_ECapacitance`, psi, psiPrev, PLCprev, V, fapo, c, d, pi0, eps, timestep)
}

hydraulics_E2psiVanGenuchten <- function(E, psiSoil, krhizomax, n, alpha, psiStep = -0.0001, psiMax = -10.0) {
    .Call(`_medfate_E2psiVanGenuchten`, E, psiSoil, krhizomax, n, alpha, psiStep, psiMax)
}

hydraulics_E2psiTwoElements <- function(E, psiSoil, krhizomax, kxylemmax, n, alpha, c, d, psiCav = 0.0, psiStep = -0.0001, psiMax = -10.0) {
    .Call(`_medfate_E2psiTwoElements`, E, psiSoil, krhizomax, kxylemmax, n, alpha, c, d, psiCav, psiStep, psiMax)
}

hydraulics_E2psiBelowground <- function(E, psiSoil, krhizomax, nsoil, alphasoil, krootmax, rootc, rootd, psiIni = as.numeric( c(0)), ntrial = 10L, psiTol = 0.0001, ETol = 0.0001) {
    .Call(`_medfate_E2psiBelowground`, E, psiSoil, krhizomax, nsoil, alphasoil, krootmax, rootc, rootd, psiIni, ntrial, psiTol, ETol)
}

hydraulics_E2psiAboveground <- function(E, psiRootCrown, kstemmax, stemc, stemd, kleafmax, leafc, leafd, PLCstem) {
    .Call(`_medfate_E2psiAboveground`, E, psiRootCrown, kstemmax, stemc, stemd, kleafmax, leafc, leafd, PLCstem)
}

hydraulics_E2psiFineRootLeaf <- function(E, psiFineRoot, krootmax, rootc, rootd, kstemmax, stemc, stemd, kleafmax, leafc, leafd, PLCstem) {
    .Call(`_medfate_E2psiFineRootLeaf`, E, psiFineRoot, krootmax, rootc, rootd, kstemmax, stemc, stemd, kleafmax, leafc, leafd, PLCstem)
}

hydraulics_E2psiNetworkStem1 <- function(E, psiSoil, krhizomax, nsoil, alphasoil, krootmax, rootc, rootd, kstemmax, stemc, stemd, PLCstem, psiIni = as.numeric( c(0)), ntrial = 10L, psiTol = 0.0001, ETol = 0.0001) {
    .Call(`_medfate_E2psiNetworkStem1`, E, psiSoil, krhizomax, nsoil, alphasoil, krootmax, rootc, rootd, kstemmax, stemc, stemd, PLCstem, psiIni, ntrial, psiTol, ETol)
}

hydraulics_E2psiNetwork <- function(E, psiSoil, krhizomax, nsoil, alphasoil, krootmax, rootc, rootd, kstemmax, stemc, stemd, kleafmax, leafc, leafd, PLCstem, psiIni = as.numeric( c(0)), ntrial = 10L, psiTol = 0.0001, ETol = 0.0001) {
    .Call(`_medfate_E2psiNetwork`, E, psiSoil, krhizomax, nsoil, alphasoil, krootmax, rootc, rootd, kstemmax, stemc, stemd, kleafmax, leafc, leafd, PLCstem, psiIni, ntrial, psiTol, ETol)
}

hydraulics_supplyFunctionOneXylem <- function(psiSoil, v, kstemmax, stemc, stemd, psiCav = 0.0, maxNsteps = 200L, dE = 0.01) {
    .Call(`_medfate_supplyFunctionOneXylem`, psiSoil, v, kstemmax, stemc, stemd, psiCav, maxNsteps, dE)
}

hydraulics_supplyFunctionTwoElements <- function(Emax, psiSoil, krhizomax, kxylemmax, n, alpha, c, d, psiCav = 0.0, dE = 0.1, psiMax = -10.0) {
    .Call(`_medfate_supplyFunctionTwoElements`, Emax, psiSoil, krhizomax, kxylemmax, n, alpha, c, d, psiCav, dE, psiMax)
}

hydraulics_supplyFunctionThreeElements <- function(Emax, psiSoil, krhizomax, kxylemmax, kleafmax, n, alpha, stemc, stemd, leafc, leafd, psiCav = 0.0, dE = 0.1, psiMax = -10.0) {
    .Call(`_medfate_supplyFunctionThreeElements`, Emax, psiSoil, krhizomax, kxylemmax, kleafmax, n, alpha, stemc, stemd, leafc, leafd, psiCav, dE, psiMax)
}

hydraulics_supplyFunctionBelowground <- function(psiSoil, krhizomax, nsoil, alphasoil, krootmax, rootc, rootd, minFlow = 0.0, maxNsteps = 400L, ntrial = 10L, psiTol = 0.0001, ETol = 0.0001, pCrit = 0.001) {
    .Call(`_medfate_supplyFunctionBelowground`, psiSoil, krhizomax, nsoil, alphasoil, krootmax, rootc, rootd, minFlow, maxNsteps, ntrial, psiTol, ETol, pCrit)
}

hydraulics_supplyFunctionAboveground <- function(Erootcrown, psiRootCrown, kstemmax, stemc, stemd, kleafmax, leafc, leafd, PLCstem) {
    .Call(`_medfate_supplyFunctionAboveground`, Erootcrown, psiRootCrown, kstemmax, stemc, stemd, kleafmax, leafc, leafd, PLCstem)
}

hydraulics_supplyFunctionFineRootLeaf <- function(psiFineRoot, krootmax, rootc, rootd, kstemmax, stemc, stemd, kleafmax, leafc, leafd, PLCstem, minFlow = 0.0, maxNsteps = 400L, ETol = 0.0001, pCrit = 0.001) {
    .Call(`_medfate_supplyFunctionFineRootLeaf`, psiFineRoot, krootmax, rootc, rootd, kstemmax, stemc, stemd, kleafmax, leafc, leafd, PLCstem, minFlow, maxNsteps, ETol, pCrit)
}

hydraulics_supplyFunctionNetworkStem1 <- function(psiSoil, krhizomax, nsoil, alphasoil, krootmax, rootc, rootd, kstemmax, stemc, stemd, PLCstem, minFlow = 0.0, maxNsteps = 400L, ntrial = 200L, psiTol = 0.0001, ETol = 0.0001, pCrit = 0.001) {
    .Call(`_medfate_supplyFunctionNetworkStem1`, psiSoil, krhizomax, nsoil, alphasoil, krootmax, rootc, rootd, kstemmax, stemc, stemd, PLCstem, minFlow, maxNsteps, ntrial, psiTol, ETol, pCrit)
}

hydraulics_supplyFunctionNetwork <- function(psiSoil, krhizomax, nsoil, alphasoil, krootmax, rootc, rootd, kstemmax, stemc, stemd, kleafmax, leafc, leafd, PLCstem, minFlow = 0.0, maxNsteps = 400L, ntrial = 200L, psiTol = 0.0001, ETol = 0.0001, pCrit = 0.001) {
    .Call(`_medfate_supplyFunctionNetwork`, psiSoil, krhizomax, nsoil, alphasoil, krootmax, rootc, rootd, kstemmax, stemc, stemd, kleafmax, leafc, leafd, PLCstem, minFlow, maxNsteps, ntrial, psiTol, ETol, pCrit)
}

hydraulics_regulatedPsiXylem <- function(E, psiUpstream, kxylemmax, c, d, psiStep = -0.01) {
    .Call(`_medfate_regulatedPsiXylem`, E, psiUpstream, kxylemmax, c, d, psiStep)
}

hydraulics_regulatedPsiTwoElements <- function(Emax, psiSoil, krhizomax, kxylemmax, n, alpha, c, d, dE = 0.1, psiMax = -10.0) {
    .Call(`_medfate_regulatedPsiTwoElements`, Emax, psiSoil, krhizomax, kxylemmax, n, alpha, c, d, dE, psiMax)
}

hydraulics_psi2Weibull <- function(psi50, psi88) {
    .Call(`_medfate_psi2Weibull`, psi50, psi88)
}

hydraulics_maximumSoilPlantConductance <- function(krhizomax, krootmax, kstemmax, kleafmax) {
    .Call(`_medfate_maximumSoilPlantConductance`, krhizomax, krootmax, kstemmax, kleafmax)
}

hydraulics_soilPlantResistances <- function(psiSoil, psiRhizo, psiStem, PLCstem, psiLeaf, krhizomax, n, alpha, krootmax, rootc, rootd, kstemmax, stemc, stemd, kleafmax, leafc, leafd) {
    .Call(`_medfate_soilPlantResistances`, psiSoil, psiRhizo, psiStem, PLCstem, psiLeaf, krhizomax, n, alpha, krootmax, rootc, rootd, kstemmax, stemc, stemd, kleafmax, leafc, leafd)
}

hydraulics_averageRhizosphereResistancePercent <- function(krhizomax, n, alpha, krootmax, rootc, rootd, kstemmax, stemc, stemd, kleafmax, leafc, leafd, psiStep = -0.01) {
    .Call(`_medfate_averageRhizosphereResistancePercent`, krhizomax, n, alpha, krootmax, rootc, rootd, kstemmax, stemc, stemd, kleafmax, leafc, leafd, psiStep)
}

hydraulics_findRhizosphereMaximumConductance <- function(averageResistancePercent, n, alpha, krootmax, rootc, rootd, kstemmax, stemc, stemd, kleafmax, leafc, leafd) {
    .Call(`_medfate_findRhizosphereMaximumConductance`, averageResistancePercent, n, alpha, krootmax, rootc, rootd, kstemmax, stemc, stemd, kleafmax, leafc, leafd)
}

hydraulics_taperFactorSavage <- function(height) {
    .Call(`_medfate_taperFactorSavage`, height)
}

hydraulics_terminalConduitRadius <- function(height) {
    .Call(`_medfate_terminalConduitRadius`, height)
}

hydraulics_referenceConductivityHeightFactor <- function(refheight, height) {
    .Call(`_medfate_referenceConductivityHeightFactor`, refheight, height)
}

hydraulics_maximumStemHydraulicConductance <- function(xylemConductivity, refheight, Al2As, height, taper = FALSE) {
    .Call(`_medfate_maximumStemHydraulicConductance`, xylemConductivity, refheight, Al2As, height, taper)
}

hydraulics_maximumRootHydraulicConductance <- function(xylemConductivity, Al2As, v, widths, depthWidthRatio = 1.0) {
    .Call(`_medfate_maximumRootHydraulicConductance`, xylemConductivity, Al2As, v, widths, depthWidthRatio)
}

hydraulics_stemWaterCapacity <- function(Al2As, height, wd) {
    .Call(`_medfate_stemWaterCapacity`, Al2As, height, wd)
}

hydraulics_leafWaterCapacity <- function(SLA, ld) {
    .Call(`_medfate_leafWaterCapacity`, SLA, ld)
}

hydrology_erFactor <- function(doy, pet, prec, Rconv = 5.6, Rsyn = 1.5) {
    .Call(`_medfate_erFactor`, doy, pet, prec, Rconv, Rsyn)
}

hydrology_soilEvaporationAmount <- function(DEF, PETs, Gsoil) {
    .Call(`_medfate_soilEvaporationAmount`, DEF, PETs, Gsoil)
}

hydrology_soilEvaporation <- function(soil, soilFunctions, pet, LgroundSWR, modifySoil = TRUE) {
    .Call(`_medfate_soilEvaporation`, soil, soilFunctions, pet, LgroundSWR, modifySoil)
}

.hydrology_infiltrationAmount <- function(input, Ssoil) {
    .Call(`_medfate_infiltrationAmount`, input, Ssoil)
}

hydrology_infiltrationRepartition <- function(I, dVec, macro, a = -0.005, b = 3.0) {
    .Call(`_medfate_infiltrationRepartition`, I, dVec, macro, a, b)
}

.hydrology_interceptionGashDay <- function(Precipitation, Cm, p, ER = 0.05) {
    .Call(`_medfate_interceptionGashDay`, Precipitation, Cm, p, ER)
}

hydrology_snowMelt <- function(tday, rad, LgroundSWR, elevation) {
    .Call(`_medfate_snowMelt`, tday, rad, LgroundSWR, elevation)
}

hydrology_soilWaterInputs <- function(soil, soilFunctions, prec, er, tday, rad, elevation, Cm, LgroundPAR, LgroundSWR, runon = 0.0, snowpack = TRUE, modifySoil = TRUE) {
    .Call(`_medfate_soilWaterInputs`, soil, soilFunctions, prec, er, tday, rad, elevation, Cm, LgroundPAR, LgroundSWR, runon, snowpack, modifySoil)
}

hydrology_soilInfiltrationPercolation <- function(soil, soilFunctions, waterInput, drainage = TRUE, modifySoil = TRUE) {
    .Call(`_medfate_soilInfiltrationPercolation`, soil, soilFunctions, waterInput, drainage, modifySoil)
}

.incgam <- function(a, x) {
    .Call(`_medfate_incgam`, a, x)
}

.invincgam <- function(a, p, q) {
    .Call(`_medfate_invincgam`, a, p, q)
}

.gammds <- function(x, p) {
    .Call(`_medfate_gammds`, x, p)
}

.parcohort <- function(SP, H, CR, LAI, SpParams) {
    .Call(`_medfate_parcohort`, SP, H, CR, LAI, SpParams)
}

.parheight <- function(heights, SP, H, CR, LAI, SpParams) {
    .Call(`_medfate_parheight`, heights, SP, H, CR, LAI, SpParams)
}

.swrheight <- function(heights, SP, H, CR, LAI, SpParams) {
    .Call(`_medfate_swrheight`, heights, SP, H, CR, LAI, SpParams)
}

.parExtinctionProfile <- function(z, x, SpParams, gdd = NA_real_, mode = "MED") {
    .Call(`_medfate_parExtinctionProfile`, z, x, SpParams, gdd, mode)
}

.swrExtinctionProfile <- function(z, x, SpParams, gdd = NA_real_, mode = "MED") {
    .Call(`_medfate_swrExtinctionProfile`, z, x, SpParams, gdd, mode)
}

light_cohortAbsorbedSWRFraction <- function(z, x, SpParams, gdd = NA_real_) {
    .Call(`_medfate_cohortAbsorbedSWRFraction`, z, x, SpParams, gdd)
}

light_layerIrradianceFraction <- function(LAIme, LAImd, LAImx, k, alpha, trunkExtinctionFraction = 0.1) {
    .Call(`_medfate_layerIrradianceFraction`, LAIme, LAImd, LAImx, k, alpha, trunkExtinctionFraction)
}

light_layerIrradianceFractionBottomUp <- function(LAIme, LAImd, LAImx, k, alpha, trunkExtinctionFraction = 0.1) {
    .Call(`_medfate_layerIrradianceFractionBottomUp`, LAIme, LAImd, LAImx, k, alpha, trunkExtinctionFraction)
}

light_cohortSunlitShadeAbsorbedRadiation <- function(Ib0, Id0, Ibf, Idf, beta, LAIme, LAImd, kb, kd, alpha, gamma) {
    .Call(`_medfate_cohortSunlitShadeAbsorbedRadiation`, Ib0, Id0, Ibf, Idf, beta, LAIme, LAImd, kb, kd, alpha, gamma)
}

light_layerSunlitFraction <- function(LAIme, LAImd, kb) {
    .Call(`_medfate_layerSunlitFraction`, LAIme, LAImd, kb)
}

light_instantaneousLightExtinctionAbsortion <- function(LAIme, LAImd, LAImx, kPAR, alphaSWR, gammaSWR, ddd, LWR_diffuse, ntimesteps = 24L, canopyMode = "sunshade", trunkExtinctionFraction = 0.1) {
    .Call(`_medfate_instantaneousLightExtinctionAbsortion`, LAIme, LAImd, LAImx, kPAR, alphaSWR, gammaSWR, ddd, LWR_diffuse, ntimesteps, canopyMode, trunkExtinctionFraction)
}

.checkSpeciesParameters <- function(SpParams, params) {
    invisible(.Call(`_medfate_checkSpeciesParameters`, SpParams, params))
}

spwbInput <- function(above, V, soil, SpParams, control) {
    .Call(`_medfate_spwbInput`, above, V, soil, SpParams, control)
}

growthInput <- function(above, Z, V, soil, SpParams, control) {
    .Call(`_medfate_growthInput`, above, Z, V, soil, SpParams, control)
}

forest2spwbInput <- function(x, soil, SpParams, control, mode = "MED") {
    .Call(`_medfate_forest2spwbInput`, x, soil, SpParams, control, mode)
}

forest2growthInput <- function(x, soil, SpParams, control) {
    .Call(`_medfate_forest2growthInput`, x, soil, SpParams, control)
}

resetInputs <- function(x, soil) {
    invisible(.Call(`_medfate_resetInputs`, x, soil))
}

.gdd <- function(DOY, Temp, Tbase = 5.0, cum = 0.0) {
    .Call(`_medfate_gdd`, DOY, Temp, Tbase, cum)
}

pheno_leafDevelopmentStatus <- function(Sgdd, gdd, unfoldingDD = 300.0) {
    .Call(`_medfate_leafDevelopmentStatus`, Sgdd, gdd, unfoldingDD)
}

pheno_leafSenescenceStatus <- function(Ssen, sen) {
    .Call(`_medfate_leafSenescenceStatus`, Ssen, sen)
}

pheno_updatePhenology <- function(x, doy, photoperiod, tmean) {
    invisible(.Call(`_medfate_updatePhenology`, x, doy, photoperiod, tmean))
}

pheno_updateLeaves <- function(x, wind, fromGrowthModel) {
    invisible(.Call(`_medfate_updateLeaves`, x, wind, fromGrowthModel))
}

photo_GammaTemp <- function(Tleaf) {
    .Call(`_medfate_gammaTemp`, Tleaf)
}

photo_KmTemp <- function(Tleaf, Oi = 209.0) {
    .Call(`_medfate_KmTemp`, Tleaf, Oi)
}

photo_VmaxTemp <- function(Vmax298, Tleaf) {
    .Call(`_medfate_VmaxTemp`, Vmax298, Tleaf)
}

photo_JmaxTemp <- function(Jmax298, Tleaf) {
    .Call(`_medfate_JmaxTemp`, Jmax298, Tleaf)
}

photo_electronLimitedPhotosynthesis <- function(Q, Ci, GT, Jmax) {
    .Call(`_medfate_electronLimitedPhotosynthesis`, Q, Ci, GT, Jmax)
}

photo_rubiscoLimitedPhotosynthesis <- function(Ci, GT, Km, Vmax) {
    .Call(`_medfate_rubiscoLimitedPhotosynthesis`, Ci, GT, Km, Vmax)
}

photo_photosynthesis <- function(Q, Catm, Gc, Tleaf, Vmax298, Jmax298, verbose = FALSE) {
    .Call(`_medfate_leafphotosynthesis`, Q, Catm, Gc, Tleaf, Vmax298, Jmax298, verbose)
}

photo_leafPhotosynthesisFunction <- function(E, Catm, Patm, Tair, vpa, u, absRad, Q, Vmax298, Jmax298, leafWidth = 1.0, refLeafArea = 1.0, verbose = FALSE) {
    .Call(`_medfate_leafPhotosynthesisFunction`, E, Catm, Patm, Tair, vpa, u, absRad, Q, Vmax298, Jmax298, leafWidth, refLeafArea, verbose)
}

photo_sunshadePhotosynthesisFunction <- function(E, Catm, Patm, Tair, vpa, SLarea, SHarea, u, absRadSL, absRadSH, QSL, QSH, Vmax298SL, Vmax298SH, Jmax298SL, Jmax298SH, leafWidth = 1.0, verbose = FALSE) {
    .Call(`_medfate_sunshadePhotosynthesisFunction`, E, Catm, Patm, Tair, vpa, SLarea, SHarea, u, absRadSL, absRadSH, QSL, QSH, Vmax298SL, Vmax298SH, Jmax298SL, Jmax298SH, leafWidth, verbose)
}

photo_multilayerPhotosynthesisFunction <- function(E, Catm, Patm, Tair, vpa, SLarea, SHarea, u, absRadSL, absRadSH, QSL, QSH, Vmax298, Jmax298, leafWidth = 1.0, verbose = FALSE) {
    .Call(`_medfate_multilayerPhotosynthesisFunction`, E, Catm, Patm, Tair, vpa, SLarea, SHarea, u, absRadSL, absRadSH, QSL, QSH, Vmax298, Jmax298, leafWidth, verbose)
}

root_conicDistribution <- function(Zcone, d) {
    .Call(`_medfate_conicDistribution`, Zcone, d)
}

root_ldrDistribution <- function(Z50, Z95, d) {
    .Call(`_medfate_ldrDistribution`, Z50, Z95, d)
}

.rootDistribution <- function(z, x) {
    .Call(`_medfate_rootDistribution`, z, x)
}

root_rootLengths <- function(v, d, depthWidthRatio = 1.0) {
    .Call(`_medfate_rootLengths`, v, d, depthWidthRatio)
}

root_xylemConductanceProportions <- function(v, d, depthWidthRatio = 1.0) {
    .Call(`_medfate_xylemConductanceProportions`, v, d, depthWidthRatio)
}

root_horizontalProportions <- function(V, LAIlive, poolOverlapFactor) {
    .Call(`_medfate_horizontalProportions`, V, LAIlive, poolOverlapFactor)
}

soil_thetaSATSX <- function(clay, sand, om = NA_real_) {
    .Call(`_medfate_thetaSATSaxton`, clay, sand, om)
}

soil_theta2psiSX <- function(clay, sand, theta, om = NA_real_) {
    .Call(`_medfate_theta2psiSaxton`, clay, sand, theta, om)
}

soil_psi2thetaSX <- function(clay, sand, psi, om = NA_real_) {
    .Call(`_medfate_psi2thetaSaxton`, clay, sand, psi, om)
}

soil_psi2thetaVG <- function(n, alpha, theta_res, theta_sat, psi) {
    .Call(`_medfate_psi2thetaVanGenuchten`, n, alpha, theta_res, theta_sat, psi)
}

soil_theta2psiVG <- function(n, alpha, theta_res, theta_sat, theta) {
    .Call(`_medfate_theta2psiVanGenuchten`, n, alpha, theta_res, theta_sat, theta)
}

soil_USDAType <- function(clay, sand) {
    .Call(`_medfate_USDAType`, clay, sand)
}

soil_vanGenuchtenParamsCarsel <- function(soilType) {
    .Call(`_medfate_vanGenuchtenParamsCarsel`, soilType)
}

soil_vanGenuchtenParamsToth <- function(clay, sand, om, bd, topsoil) {
    .Call(`_medfate_vanGenuchtenParamsToth`, clay, sand, om, bd, topsoil)
}

soil_temperatureGradient <- function(dVec, Temp) {
    .Call(`_medfate_temperatureGradient`, dVec, Temp)
}

soil_temperatureChange <- function(dVec, Temp, sand, clay, W, Theta_FC, Gdown) {
    .Call(`_medfate_temperatureChange`, dVec, Temp, sand, clay, W, Theta_FC, Gdown)
}

soil <- function(SoilParams, VG_PTF = "Toth", W = as.numeric( c(1.0)), SWE = 0.0) {
    .Call(`_medfate_soil`, SoilParams, VG_PTF, W, SWE)
}

soil_thetaFC <- function(soil, model = "SX") {
    .Call(`_medfate_thetaFC`, soil, model)
}

soil_thetaWP <- function(soil, model = "SX") {
    .Call(`_medfate_thetaWP`, soil, model)
}

soil_thetaSAT <- function(soil, model = "SX") {
    .Call(`_medfate_thetaSAT`, soil, model)
}

soil_waterFC <- function(soil, model = "SX") {
    .Call(`_medfate_waterFC`, soil, model)
}

soil_waterSAT <- function(soil, model = "SX") {
    .Call(`_medfate_waterSAT`, soil, model)
}

soil_waterWP <- function(soil, model = "SX") {
    .Call(`_medfate_waterWP`, soil, model)
}

soil_waterExtractable <- function(soil, model = "SX", minPsi = -5.0) {
    .Call(`_medfate_waterExtractable`, soil, model, minPsi)
}

soil_theta <- function(soil, model = "SX") {
    .Call(`_medfate_theta`, soil, model)
}

soil_water <- function(soil, model = "SX") {
    .Call(`_medfate_water`, soil, model)
}

soil_rockWeight2Volume <- function(pWeight, bulkDensity, rockDensity = 2.3) {
    .Call(`_medfate_rockWeight2Volume`, pWeight, bulkDensity, rockDensity)
}

soil_psi <- function(soil, model = "SX") {
    .Call(`_medfate_psi`, soil, model)
}

soil_waterTableDepth <- function(soil, model = "SX") {
    .Call(`_medfate_waterTableDepth`, soil, model)
}

soil_thermalCapacity <- function(soil, model = "SX") {
    .Call(`_medfate_thermalCapacity`, soil, model)
}

soil_thermalConductivity <- function(soil, model = "SX") {
    .Call(`_medfate_thermalConductivity`, soil, model)
}

spwb_day <- function(x, soil, date, tmin, tmax, rhmin, rhmax, rad, wind, latitude, elevation, slope, aspect, prec, runon = 0.0) {
    .Call(`_medfate_spwbDay`, x, soil, date, tmin, tmax, rhmin, rhmax, rad, wind, latitude, elevation, slope, aspect, prec, runon)
}

spwb <- function(x, soil, meteo, latitude, elevation = NA_real_, slope = NA_real_, aspect = NA_real_) {
    .Call(`_medfate_spwb`, x, soil, meteo, latitude, elevation, slope, aspect)
}

pwb <- function(x, soil, meteo, W, latitude, elevation = NA_real_, slope = NA_real_, aspect = NA_real_, canopyEvaporation = numeric(0), snowMelt = numeric(0), soilEvaporation = numeric(0)) {
    .Call(`_medfate_pwb`, x, soil, meteo, W, latitude, elevation, slope, aspect, canopyEvaporation, snowMelt, soilEvaporation)
}

moisture_turgorLossPoint <- function(pi0, epsilon) {
    .Call(`_medfate_turgorLossPoint`, pi0, epsilon)
}

moisture_symplasticRWC <- function(psiSym, pi0, epsilon) {
    .Call(`_medfate_symplasticRelativeWaterContent`, psiSym, pi0, epsilon)
}

moisture_symplasticPsi <- function(RWC, pi0, epsilon) {
    .Call(`_medfate_symplasticWaterPotential`, RWC, pi0, epsilon)
}

moisture_apoplasticRWC <- function(psiApo, c, d) {
    .Call(`_medfate_apoplasticRelativeWaterContent`, psiApo, c, d)
}

moisture_apoplasticPsi <- function(RWC, c, d) {
    .Call(`_medfate_apoplasticWaterPotential`, RWC, c, d)
}

moisture_tissueRWC <- function(psiSym, pi0, epsilon, psiApo, c, d, af, femb = 0.0) {
    .Call(`_medfate_tissueRelativeWaterContent`, psiSym, pi0, epsilon, psiApo, c, d, af, femb)
}

moisture_tissueFMC <- function(RWC, density, d0 = 1.54) {
    .Call(`_medfate_tissueFMC`, RWC, density, d0)
}

moisture_cohortFMC <- function(spwb) {
    .Call(`_medfate_cohortFMC`, spwb)
}

transp_profitMaximization <- function(supplyFunction, photosynthesisFunction, Gwmin, Gwmax, gainModifier = 1.0, costModifier = 1.0, costWater = "dEdP") {
    .Call(`_medfate_profitMaximization`, supplyFunction, photosynthesisFunction, Gwmin, Gwmax, gainModifier, costModifier, costWater)
}

transp_transpirationSperry <- function(x, soil, meteo, day, latitude, elevation, slope, aspect, canopyEvaporation = 0.0, snowMelt = 0.0, soilEvaporation = 0.0, stepFunctions = NA_integer_, modifyInputX = TRUE, modifyInputSoil = TRUE) {
    .Call(`_medfate_transpirationSperry`, x, soil, meteo, day, latitude, elevation, slope, aspect, canopyEvaporation, snowMelt, soilEvaporation, stepFunctions, modifyInputX, modifyInputSoil)
}

transp_transpirationGranier <- function(x, soil, meteo, day, modifyInputX = TRUE, modifyInputSoil = TRUE) {
    .Call(`_medfate_transpirationGranier`, x, soil, meteo, day, modifyInputX, modifyInputSoil)
}

.windSpeedAtCanopyHeight <- function(wind20H, canopyHeight) {
    .Call(`_medfate_windSpeedAtCanopyHeight`, wind20H, canopyHeight)
}

.unshelteredMidflameWindSpeed <- function(wind20H, fuelBedHeight) {
    .Call(`_medfate_unshelteredMidflameWindSpeed`, wind20H, fuelBedHeight)
}

.shelteredMidflameWindSpeed <- function(wind20H, crownFillProportion, topCanopyHeight) {
    .Call(`_medfate_shelteredMidflameWindSpeed`, wind20H, crownFillProportion, topCanopyHeight)
}

fuel_windAdjustmentFactor <- function(topShrubHeight, bottomCanopyHeight, topCanopyHeight, canopyCover) {
    .Call(`_medfate_windAdjustmentFactor`, topShrubHeight, bottomCanopyHeight, topCanopyHeight, canopyCover)
}

.windSpeedAtHeightOverCanopy <- function(z, wind20H, canopyHeight) {
    .Call(`_medfate_windSpeedAtHeightOverCanopy`, z, wind20H, canopyHeight)
}

.windExtinctionProfile <- function(z, wind20H, LAIc, canopyHeight) {
    .Call(`_medfate_windExtinctionProfile`, z, wind20H, LAIc, canopyHeight)
}

# Register entry points for exported C++ functions
methods::setLoadAction(function(ns) {
    .Call('_medfate_RcppExport_registerCCallable', PACKAGE = 'medfate')
})
