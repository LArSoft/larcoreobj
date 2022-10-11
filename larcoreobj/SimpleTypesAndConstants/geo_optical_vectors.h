/**
 * @file   larcoreobj/SimpleTypesAndConstants/geo_optical_vectors.h
 * @brief  Definitions of vector data types for optical detectors.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   May 14, 2020
 * @ingroup Geometry
 * @see    larcoreobj/SimpleTypesAndConstants/geo_vectors.h
 *
 * This library depends on ROOT GenVector.
 * In the CET link list in `CMakeLists.txt`, link to `ROOT::GenVector`.
 */

#ifndef LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_OPTICAL_VECTORS_H
#define LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_OPTICAL_VECTORS_H

// LArSoft libraries
#include "larcoreobj/SimpleTypesAndConstants/geo_vectors.h"

namespace geo {

  /**
   * @name Optical detector vector types.
   * @ingroup Geometry
   *
   * Special tags for a generic "optical detector local coordinate system"
   * are defined here, together with data types using them.
   *
   * While the world coordinate system makes sense to be defined in this way,
   * a local system depends by definition by the object it is anchored to:
   * while two world coordinate vectors are always compatible, two local
   * coordinate vectors are compatible only if they belong to the same local
   * object. The purpose of these definitions is to declare the incompatibility
   * between world coordinate vectors and the ones local to optical detectors,
   * without attempting to claim that two of the latter are necessarily
   * compatible.
   *
   */
  /// @{

  /// The tag defining the optical detector local reference frame.
  struct OpticalLocalCoordinateSystemTag {};

  /// Type of optical local 3D displacement vector.
  /// @tparam T data type for coordinate representation
  template <typename T>
  using OpticalVector3DBase_t = GenVector3DBase_t<T, OpticalLocalCoordinateSystemTag>;

  /// Type of optical local 3D point.
  /// @tparam T data type for coordinate representation
  template <typename T>
  using OpticalPoint3DBase_t = GenPoint3DBase_t<T, OpticalLocalCoordinateSystemTag>;

  /// Type of optical 3D displacement vector with representation in
  /// double precision.
  using OpticalVector_t = OpticalVector3DBase_t<double>;

  /// Type of optical 3D point with representation in double precision.
  using OpticalPoint_t = OpticalPoint3DBase_t<double>;

  /// @}

  //----------------------------------------------------------------------------

} // namespace geo

#endif // LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_OPTICAL_VECTORS_H
