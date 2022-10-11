/**
 * @file   larcoreobj/SummaryData/GeometryConfigurationInfo.h
 * @brief  Description of the current configuration of detector geometry.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   May 13, 2020
 *
 * This is a header-only library.
 */

#ifndef LARCOREOBJ_SUMMARYDATA_GEOMETRYCONFIGURATIONINFO_H
#define LARCOREOBJ_SUMMARYDATA_GEOMETRYCONFIGURATIONINFO_H

// C/C++ standard library
#include <iosfwd>
#include <string>

// -----------------------------------------------------------------------------
namespace sumdata {
  struct GeometryConfigurationInfo;
  std::ostream& operator<<(std::ostream&, GeometryConfigurationInfo const&);
} // namespace sumdata

/**
 * @brief  Description of the current configuration of detector geometry.
 *
 * This data class contains enough information to determine if the detector
 * geometry configuration it describes is compatible with another one.
 *
 * The version of the data is explicitly included, so that the software can
 * determine which is the most complete test possible when the version of the
 * input is older than the current one.
 *
 * The information in this object should be as compact as possible to avoid
 * bloating the data files it is stored into.
 * Currently, three categories of information are included:
 *
 * * the version of this data (just a monotonically increasing integral value);
 * * the full configuration of `Geometry` service as a string;
 * * selected information for easier access
 *
 * @note The logic determining the compatibility between two
 *   `sumdata::GeometryConfigurationInfo` objects is not part of the object
 *   definition and it is delegated to the appropriate algorithms.
 *
 *
 * Versions
 * =========
 *
 * 1. legacy version; includes
 *     * the version (`1`)
 *     * detector name (`geo::Geometry::DetectorName()`)
 * 2. includes version 1 information and:
 *     * the configuration of `geo::Geometry` service as a FHiCL string
 *
 *
 */
struct sumdata::GeometryConfigurationInfo {

  using DataVersion_t = unsigned int; ///< Type used for the version of data.

  /// Value denoting an invalid data version.
  static constexpr DataVersion_t InvalidDataVersion = 0U;

  /// Version of the data in this object (`0` is invalid version).
  DataVersion_t dataVersion = InvalidDataVersion;

  /// `geo::Geometry` service configuration, as FHiCL table.
  std::string geometryServiceConfiguration;

  ///< Name of the geometry (`geo::GeometryCore::DetectorName()`).
  std::string detectorName;

  /// Protocol: whether the data content is valid.
  bool isDataValid() const noexcept { return dataVersion != InvalidDataVersion; }

}; // sumdata::GeometryConfigurationInfo

// -----------------------------------------------------------------------------

#endif // LARCOREOBJ_SUMMARYDATA_GEOMETRYCONFIGURATIONINFO_H
