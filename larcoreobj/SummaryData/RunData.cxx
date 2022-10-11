////////////////////////////////////////////////////////////////////////
/// \file larcoreobj/SummaryData/RunData.cxx
///
/// Definition of object to store run related information
///
/// \author  brebel@fnal.gov
////////////////////////////////////////////////////////////////////////

#include "larcoreobj/SummaryData/RunData.h"

#include <stdexcept> // std::runtime_error

namespace sumdata {

  //---------------------------------------------------------
  RunData::RunData() : fDetName("nodetectorname") {}

  //---------------------------------------------------------
  RunData::RunData(std::string const& detectorName) : fDetName(detectorName) {}

  //---------------------------------------------------------
  void RunData::aggregate(RunData const& other)
  {

    // Each run is required to have the same detector name.
    // This might be a problem for Monte Carlo jobs which tend to use the same
    // run number for everything.
    if (other.DetName() != DetName()) {
      throw std::runtime_error("The same run sees different detector setups: '" + DetName() +
                               "' and '" + other.DetName());
    }
  } // RunData::aggregate()

  //---------------------------------------------------------

}
