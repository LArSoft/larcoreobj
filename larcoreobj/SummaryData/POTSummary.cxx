////////////////////////////////////////////////////////////////////////
/// \file POTSummary.cxx
///
/// Definition of object to store pot related information
///
/// \author  brebel@fnal.gov
////////////////////////////////////////////////////////////////////////

#include "larcoreobj/SummaryData/POTSummary.h"

namespace sumdata {

  //----------------------------------------------------------------------------
  void POTSummary::aggregate(POTSummary const& other)
  {
    totpot += other.totpot;
    totgoodpot += other.totgoodpot;
    totspills += other.totspills;
    goodspills += other.goodspills;
  } // POTSummary::aggregate()

  //----------------------------------------------------------------------------

} // end namespace
