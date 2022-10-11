////////////////////////////////////////////////////////////////////////
/// \file  larcoreobj/SimpleTypesAndConstants/PhysicalConstants.h
/// \brief Collection of Physical constants used in LArSoft
///
///
/// \author brebel@fnal.gov
////////////////////////////////////////////////////////////////////////
#ifndef UTIL_PHYSICALCONSTANTS_H
#define UTIL_PHYSICALCONSTANTS_H

/**
 * @brief Generic namespace of utility functions generally independent of LArSoft.
 *
 * Some physical constants are also included here.
 * As a reminder, the "standard" units in LArSoft are:
 * * energy: GeV
 * * time: ns
 * * space: cm
 *
 */
namespace util {

  /**
   * @{
   * @name Recombination factor coefficients (NIM).
   * @see sim::ISCalculationSeparate::CalculateIonizationAndScintillation()
   *
   * Recombination factor coefficients come from Nucl.Instrum.Meth.A523:275-286,2004
   * * @f$ dE/dx @f$ is given by the voxel energy deposition, but have to convert it to MeV/cm from GeV/voxel width
   * * electric field: @f$ E @f$ in kV/cm
   * * @f$ R = A/(1 + (dE/dx)*k/E) @f$
   * * @f$ A = 0.800 \pm 0.003 @f$
   * * @f$ k = 0.0486 @f$ needs to be scaled with Electric field
   */
  constexpr double kRecombA = 0.800;  ///< _A_ constant.
  constexpr double kRecombk = 0.0486; ///< _k_ constant, in g/(MeV cm&sup2;)*kV/cm.
  /// @}
  /**
   * @{
   * @name Recombination factor coefficients (modified box, ArguNeuT JINST).
   * @see sim::ISCalculationSeparate::CalculateIonizationAndScintillation()
   *
   * Recombination factor coefficients come from Nucl.Instrum.Meth.A523:275-286,2004
   * * @f$ dE/dx @f$ is given by the voxel energy deposition, but have to convert it to MeV/cm from GeV/voxel width
   * * electric field: @f$ E @f$ in kV/cm
   * * `kModBoxB` needs to be scaled with the electric field.
   *
   */
  constexpr double kModBoxA = 0.930; ///< Modified Box Alpha
  constexpr double kModBoxB = 0.212; ///< Modified Box Beta in g/(MeV cm&sup2;)*kV/cm
  // constexpr double kWion          = 23.6e-6;  ///< ionization potenial in LAr, 23.6 eV = 1e, Wion in MeV/e
  /// @}

  // Conversion for energy deposited in GeV to number of ionization electrons produced
  constexpr double kGeVToElectrons = 4.237e7; ///< 23.6eV per ion pair, 1e9 eV/GeV

  // More constants
  /// Speed of light in vacuum in LArSoft units [cm/ns].
  constexpr double kc = 29.9792458;
  //static double khbar = 6.58211899e-22;  ///< MeVs

  // Conversion factors
  constexpr double kMeterToCentimeter = 1.e2; ///< 1 m = 100 cm
  constexpr double kCentimeterToMeter = 1. / kMeterToCentimeter;
  constexpr double kMeterToKilometer = 1.e-3; ///< 1000 m = 1 km
  constexpr double kKilometerToMeter = 1. / kMeterToKilometer;

  constexpr double keVToMeV = 1.e-6; ///< 1e6 eV = 1 MeV
  constexpr double kMeVToeV = 1. / keVToMeV;

  constexpr double kBogusD = -999.; ///< obviously bogus double value
  constexpr int kBogusI = -999;     ///< obviously bogus integer value
  constexpr float kBogusF = -999.;  ///< obviously bogus float value

  constexpr double quietCompiler =
    kBogusD * kBogusI * kBogusF * kRecombA * kRecombk * kGeVToElectrons;

  /// Returns the constant pi (up to 35 decimal digits of precision)
  template <typename T = double>
  inline constexpr T pi()
  {
    return 3.14159265358979323846264338327950288L;
  }

  /// Converts the argument angle from degrees into radians
  template <typename T>
  inline constexpr T DegreesToRadians(T angle)
  {
    return angle / 180 * pi<T>();
  }

  /// Converts the argument angle from radians into degrees (@f$ \pi \rightarrow 180 @f$)
  template <typename T>
  inline constexpr T RadiansToDegrees(T angle)
  {
    return angle / pi<T>() * 180;
  }

} // namespace util

#endif //UTIL_PHYSICALCONSTANTS_H
