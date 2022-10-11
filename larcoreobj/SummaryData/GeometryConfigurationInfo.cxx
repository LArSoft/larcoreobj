/**
 * @file   larcoreobj/SummaryData/GeometryConfigurationInfo.cxx
 * @brief  Description of the current configuration of detector geometry.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   May 13, 2020
 * @see    larcoreobj/SummaryData/GeometryConfigurationInfo.h
 *
 * This is a header-only library.
 */

#include "larcoreobj/SummaryData/GeometryConfigurationInfo.h"

// C/C++ standard library
#include <ostream>

// -----------------------------------------------------------------------------
std::ostream& sumdata::operator<<(std::ostream& out, sumdata::GeometryConfigurationInfo const& info)
{

  if (!info.isDataValid()) return out << "Invalid geometry configuration information" << std::endl;

  out << "Geometry information version: " << info.dataVersion;

  if (info.dataVersion >= sumdata::GeometryConfigurationInfo::DataVersion_t{1}) {
    out << "\nDetector name:               '" << info.detectorName << "'";
  } // version >= 1

  if (info.dataVersion >= sumdata::GeometryConfigurationInfo::DataVersion_t{2}) {
    out << "\nFull configuration:"
        << "\n"
        << std::string(80, '-') << "\n"
        << info.geometryServiceConfiguration << "\n"
        << std::string(80, '-');
  }

  if (info.dataVersion > sumdata::GeometryConfigurationInfo::DataVersion_t{2}) {
    out << "\n[this version of code can't fully decode further information]";
  }

  return out;
} // sumdata::operator<< (GeometryConfigurationInfo const&)

// -----------------------------------------------------------------------------
