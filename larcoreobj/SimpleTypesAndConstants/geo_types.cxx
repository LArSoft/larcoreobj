/**
 * @file   larcoreobj/SimpleTypesAndConstants/geo_types.cxx
 * @brief  Definition of data types for geometry description (implementation).
 * @see    larcoreobj/SimpleTypesAndConstants/geo_types.h
 * @ingroup Geometry
 */

// header library
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

// C++ standard libraries
#include <ostream>
#include <stdexcept> // std::logic_error

namespace geo {

  std::ostream& operator<<(std::ostream& os, Coordinate const coordinate)
  {
    switch (coordinate) {
    case Coordinate::X: os << 'X'; break;
    case Coordinate::Y: os << 'Y'; break;
    case Coordinate::Z: os << 'Z'; break;
    }
    return os;
  }

  std::ostream& operator<<(std::ostream& os, DriftSign const sign)
  {
    switch (sign) {
    case DriftSign::Positive: os << '+'; break;
    case DriftSign::Negative: os << '-'; break;
    case DriftSign::Unknown: os << '?'; break;
    }
    return os;
  }

  bool operator==(DriftAxis const a, DriftAxis const b)
  {
    return a.coordinate == b.coordinate and a.sign == b.sign;
  }

  bool operator!=(DriftAxis const a, DriftAxis const b)
  {
    return not (a == b);
  }

  std::ostream& operator<<(std::ostream& os, DriftAxis const driftAxis)
  {
    return os << driftAxis.sign << driftAxis.coordinate;
  }

  std::string SignalTypeName(SigType_t sigType)
  {
    switch (sigType) {
    case kInduction: return "induction";
    case kCollection: return "collection";
    case kMysteryType: return "unknown";
    } // switch
    throw std::logic_error("geo::SignalTypeName(): unexpected signal type #" +
                           std::to_string(static_cast<int>(sigType)));
  }
}

// -----------------------------------------------------------------------------
