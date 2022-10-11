////////////////////////////////////////////////////////////////////////
/// \file larcoreobj/SummaryData/RunData.h
///
/// Definition of object to store run related information
///
/// \author  brebel@fnal.gov
////////////////////////////////////////////////////////////////////////
#ifndef LARCOREOBJ_SUMMARYDATA_RUNDATA_H
#define LARCOREOBJ_SUMMARYDATA_RUNDATA_H

#include <string>

namespace sumdata {

  class RunData {

  public:
    RunData(); // Default constructor

    /// What to do when multiple `RunData` objects are found for the same run.
    ///
    /// @throws std::runtime_error if `other` has a different `DetName()`
    void aggregate(RunData const& other);

  private:
    std::string fDetName; ///< Detector name.

  public:
    explicit RunData(std::string const& detectorName);
    std::string const& DetName() const;

  }; // RunData
} // namespace sumdata

inline std::string const& sumdata::RunData::DetName() const
{
  return fDetName;
}

#endif // LARCOREOBJ_SUMMARYDATA_RUNDATA_H
