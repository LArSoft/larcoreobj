/**
 * @file   larcoreobj/SimpleTypesAndConstants/geo_vectors.h
 * @brief  Definitions of geometry vector data types.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   March 23, 2017
 * 
 * This library depends on ROOT GenVector.
 * In the CET link list in `CMakeLists.txt`, link to `${ROOT_GENVECTOR}`.
 * 
 * Additional utilities are available from `geo_vectors_utils.h`.
 */

#ifndef LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_VECTORS_H
#define LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_VECTORS_H

// ROOT libraries
#include "Math/GenVector/CoordinateSystemTags.h"
#include "Math/GenVector/Cartesian3D.h"
#include "Math/GenVector/PositionVector3D.h"
#include "Math/GenVector/DisplacementVector3D.h"
#include "Math/GenVector/Rotation3D.h"


namespace geo {
  
  ///@{
  ///@name Generic vector types.
  
  /// Type of 3D displacement vector.
  /// @tparam T data type for coordinate representation
  /// @tparam C coordinate system tag (default: global coordinates)
  template <typename T, typename C = ROOT::Math::GlobalCoordinateSystemTag>
  using GenVector3DBase_t
    = ROOT::Math::DisplacementVector3D<ROOT::Math::Cartesian3D<T>, C>;
  
  /// Type of 3D point.
  /// @tparam T data type for coordinate representation
  /// @tparam C coordinate system tag (default: global coordinates)
  template <typename T, typename C = ROOT::Math::GlobalCoordinateSystemTag>
  using GenPoint3DBase_t
    = ROOT::Math::PositionVector3D<ROOT::Math::Cartesian3D<T>, C>;
  
  /// Type of 3D displacement vector with representation in double precision.
  /// @tparam C coordinate system tag
  template <typename C>
  using Vector3DBase_t = GenVector3DBase_t<double, C>;
  
  /// Type of 3D point with representation in double precision.
  template <typename C>
  /// @tparam C coordinate system tag
  using Point3DBase_t = GenPoint3DBase_t<double, C>;
  
  /// @}
  
  /// @{
  /**
   * @name Vector types for the standard LArSoft geometry.
   * 
   * LArSoft geometry provides two main types of vectors in 3D space:
   * 
   * 1. `geo::Point_t` to describe an absolute position in global coordinates
   * 2. `geo::Vector_t` to describe a displacement or direction (or momentum!)
   * 
   * Both vectors are supposed to represent:
   * 
   * * centimeters in double precision when used on the real geometry space
   * * in the global coordinate system, which is represented by the tag
   *   `geo::GlobalCoords`.
   * 
   * These types constitute the basic objects the geometry works with.
   * 
   * All interfaces should support them.
   * 
   * @note As this requires some transition, please report any interface missing
   *       support for these types by opening a "necessary maintenance" request
   *       in the LArSoft issue tracker at
   *       https://cdcvs.fnal.gov/redmine/projects/larsoft/issues .
   *       Even if there are plenty.
   * 
   * The same type of vectors, but in a different coordinate system
   * representation, can be obtained by using `geo::Point3DBase_t` template:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * using LocalPoint_t = geo::Point3DBase_t<LocalCoordinateTag>;
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * (`geo::Vector3DBase_t` is also available).
   * If a single precision vector is desired, the most general
   * `geo::GenPoint3DBase_t` and `geo::GenVector3DBase_t` are also available:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * using PointF_t = geo::GenPoint3DBase_t<float>;
   * using VectorF_t = geo::GenVector3DBase_t<float>;
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * 
   */
  
  /// Type used for coordinates and distances. They are measured in centimeters.
  using Length_t = double;
  
  /**
   * @brief Tag for vectors in the global coordinate system.
   * 
   * A vector tagged as "global" is expected to be represented in the global
   * (or "world") coordinate system.
   * 
   * That system is the one the detector geometry is described in, and it is
   * defined by the GDML detector description.
   * The linear coordinates are described in _centimeters_.
   * 
   */
  using GlobalCoords = ROOT::Math::GlobalCoordinateSystemTag;
  
  /**
   * @brief Type for representation of momenta in 3D space.
   * 
   * A vector represents a direction and intensity, or a displacement respect to
   * an unspecified starting point.
   * Vectors can be added or subtracted, resulting in still a vector.
   * Their modulus can also be scaled.
   * 
   */
  // the actual definition commented out is not understood by GenReflex
//  using Vector_t = Vector3DBase_t<GlobalCoords>;
  using Vector_t = ROOT::Math::DisplacementVector3D
    <ROOT::Math::Cartesian3D<double>, ROOT::Math::GlobalCoordinateSystemTag>;
  
  /**
   * @brief Type for representation of position in physical 3D space.
   * 
   * A point represents a position in 3D space.
   * As such, it makes no sense to add points, and the difference between two
   * points is not a point any more (it is, in fact, a `geo::Vector_t`).
   * Scaling and norm of a point also have no meaning.
   * 
   * A vector can be added to a point, resulting in another point.
   * 
   * Note that `middlePoint()` function and `MiddlePointAccumulator` class are
   * provided to facilitate the computation of a middle point using any type
   * of vector and in particular `geo::Point_t`.
   * 
   */
  // the actual definition commented out is not understood by GenReflex
//  using Point_t = Point3DBase_t<GlobalCoords>;
   using Point_t = ROOT::Math::PositionVector3D
     <ROOT::Math::Cartesian3D<double>, ROOT::Math::GlobalCoordinateSystemTag>;
  
  
  /**
   * @brief Type for representation of momenta in 3D space.
   * @tparam CoordSystemTag the coordinate system tag for this vector
   * 
   * This vector type is equivalent to `geo::Vector_t` but it's tagged as from a
   * different coordinate system.
   */
  template <typename CoordSystemTag>
  using VectorIn_t = Vector3DBase_t<CoordSystemTag>;
  
  /**
   * @brief Type for representation of positions in 3D space.
   * @tparam CoordSystemTag the coordinate system tag for this point
   * 
   * This point type is equivalent to `geo::Point_t` but it's tagged as from a
   * different coordinate system.
   */
  template <typename CoordSystemTag>
  using PointIn_t = Point3DBase_t<CoordSystemTag>;
  
  
  /// Type for representation of space rotations.
  using Rotation_t = ROOT::Math::Rotation3D;
  
  
  //--------------------------------------------------------------------------
  /// Returns a x axis vector of the specified type.
  template <typename Vector = Vector_t>
  constexpr Vector Xaxis() { return { 1.0, 0.0, 0.0 }; }
  
  /// Returns a y axis vector of the specified type.
  template <typename Vector = Vector_t>
  constexpr Vector Yaxis() { return { 0.0, 1.0, 0.0 }; }
  
  /// Returns a z axis vector of the specified type.
  template <typename Vector = Vector_t>
  constexpr Vector Zaxis() { return { 0.0, 0.0, 1.0 }; }
  
  /// Returns a origin position with a point of the specified type.
  template <typename Point = Point_t>
  constexpr Point origin() { return { 0.0, 0.0, 0.0 }; }
  
  /// @}
  
  
  //--------------------------------------------------------------------------
  /// @{
  /// @name Geometry vector output to stream.
  
  /**
   * @brief Prints a geometry vector into a stream
   * @tparam Stream type of output stream
   * @param out the output stream
   * @param p the 3D point to be printed
   * @return a reference to the output stream
   * 
   * The point is printed with the format: `{ x, y, z }`.
   * No end-of-line character is emitted.
   */
  template <typename Stream>
  Stream& operator<< (Stream&& out, Point_t const& p)
    {
      out << "{ " << p.X() << ", " << p.Y() << ", " << p.Z() << " }";
      return out;
    }
  
  /**
   * @brief Prints a geometry vector into a stream
   * @tparam Stream type of output stream
   * @param out the output stream
   * @param v the 3D vector to be printed
   * @return a reference to the output stream
   * 
   * The vector is printed with the format: `{ x, y, z }`.
   * No end-of-line character is emitted.
   */
  template <typename Stream>
  Stream& operator<< (Stream&& out, Vector_t const& v)
    {
      out << "{ " << v.X() << ", " << v.Y() << ", " << v.Z() << " }";
      return out;
    }
  
  /// @}
  
  //----------------------------------------------------------------------------

} // namespace geo


#endif // LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_VECTORS_H
