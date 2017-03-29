/**
 * @file   geo_vectors_fhicl.h
 * @brief  Helpers for reading vectors from FHiCL files.
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   March 24, 2017
 * 
 * This library depends on ROOT GenVector.
 * In the CET link list in `CMakeLists.txt`, link to `${ROOT_GENVECTOR}`.
 * 
 */

#ifndef LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_VECTORS_FHICL_H
#define LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_VECTORS_FHICL_H

// LArSoft libraries
#include "larcoreobj/SimpleTypesAndConstants/geo_vectors.h"

// framework libraries
#include "fhiclcpp/types/Atom.h" 
#include "fhiclcpp/types/TupleAs.h" 


namespace lar {
  
  /// LArSoft utilities for interface with FHiCL and its libraries.
  namespace fhicl {
    
    namespace geo {
      
      /// Atom object for reading length (centimeters).
      using Length_t = ::fhicl::Atom<::geo::Length_t>;
      
      
      /// Atom object for reading a 3D point or vector (centimeters).
      template <typename Point>
      using Point3D = ::fhicl::TupleAs
        <Point(::geo::Length_t, ::geo::Length_t, ::geo::Length_t)>;
      
      /// Atom object for reading a geometry point (centimeters).
      using Vector_t = Point3D<::geo::Vector_t>;
      
      /// Atom object for reading a geometry vector (centimeters).
      using Point_t = Point3D<::geo::Point_t>;
      
    } // namespace geo
    
  } // namespace fhicl
  
} // namespace lar

#endif // LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_VECTORS_FHICL_H
