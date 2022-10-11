////////////////////////////////////////////////////////////////////////
/// \file larcoreobj/SummaryData/POTSummary.h
///
/// Definition of object to store pot related information
///
/// \author  brebel@fnal.gov
////////////////////////////////////////////////////////////////////////
#ifndef LARCOREOBJ_SUMMARYDATA_POTSUMMARY_H
#define LARCOREOBJ_SUMMARYDATA_POTSUMMARY_H

namespace sumdata {

  class POTSummary {
  public:
    double totpot = 0.0;
    double totgoodpot = 0.0;

    int totspills = 0;
    int goodspills = 0;

    POTSummary() = default;

    void aggregate(POTSummary const& other);
  };

  template <typename Stream>
  Stream& operator<<(Stream&& o, POTSummary const& a);

} // namespace sumdata

//----------------------------------------------------------------------------
template <typename Stream>
Stream& sumdata::operator<<(Stream&& o, POTSummary const& a)
{
  o << "This sub run has " << a.totspills << " total spills with an exposure of " << a.totpot
    << " POT"
    << "\n with cuts on beam quality, there are " << a.goodspills
    << " good spills with an exposure of " << a.totgoodpot << "\n";

  return o;
} // sumdata::operator<< (POTSummary)

#endif //LARCOREOBJ_SUMMARYDATA_POTSUMMARY_H
