/**
 * @file   larcoreobj/SimpleTypesAndConstants/geo_types.cxx
 * @brief  Definition of data types for geometry description (implementation).
 * @see    larcoreobj/SimpleTypesAndConstants/geo_types.h
 * @ingroup Geometry
 *
 * This library is currently header-only and depends only on standard C++.
 *
 */

// header library
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

// C++ standard libraries
#include <stdexcept> // std::logic_error

// -----------------------------------------------------------------------------
std::string geo::SignalTypeName(geo::SigType_t sigType)
{
  switch (sigType) {
  case geo::kInduction: return "induction";
  case geo::kCollection: return "collection";
  case geo::kMysteryType: return "unknown";
  } // switch
  throw std::logic_error("geo::SignalTypeName(): unexpected signal type #" +
                         std::to_string(static_cast<int>(sigType)));
} // geo::SignalTypeName()

// -----------------------------------------------------------------------------
