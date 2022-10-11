/**
 * @file   readout_types.h
 * @brief  Classes identifying readout-related concepts
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   June 19th, 2015
 */

#ifndef LARCOREOBJ_SIMPLETYPESANDCONSTANTS_READOUT_TYPES_H
#define LARCOREOBJ_SIMPLETYPESANDCONSTANTS_READOUT_TYPES_H

// LArSoft libraries
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

// C/C++ standard libraries
#include <iosfwd> // std::ostream

namespace readout {

  namespace details {

    template <typename T>
    inline std::string writeToString(T const& value)
    {
      return geo::details::writeToString(value);
    }

    template <std::size_t Index, typename ID>
    using AbsIDtype = geo::details::AbsIDtype<Index, ID>;

    template <std::size_t UpIndex, typename ID>
    using RelIDtype = geo::details::RelIDtype<UpIndex, ID>;

    template <std::size_t Index, typename ID>
    inline constexpr auto getAbsIDindex(ID const& id)
    {
      return geo::details::getAbsIDindex<Index, ID>(id);
    }

    template <std::size_t Index, typename ID>
    inline auto& getAbsIDindex(ID& id)
    {
      return geo::details::getAbsIDindex<Index, ID>(id);
    }

  } // namespace details

  /// Numerical description of geometry element "level".
  /// The "detector" level is noticeably missing.
  struct ElementLevel {

    using Level_t = geo::ElementLevel::Level_t;

    static constexpr Level_t Cryostat = 0U;
    static constexpr Level_t TPCset = 1U;
    static constexpr Level_t ReadoutPlane = 2U;
    static constexpr Level_t Channel = 3U;
    static constexpr Level_t NLevels = 4U;

  }; // struct ElementLevel

  // We define our own cryostat ID as an alias of the one from geometry
  using CryostatID = geo::CryostatID;

  /**
   * @brief Class identifying a set of TPC sharing readout channels.
   *
   * This identifier provides the cryostat location and a number representing
   * the set of TPCs.
   * This set is defined by not sharing readout channels with any other TPC
   * outside the set.
   */
  struct TPCsetID : public CryostatID {
    using TPCsetID_t = unsigned short; ///< Type for the ID number.

    using ThisID_t = TPCsetID;     ///< Type of this ID.
    using ParentID_t = CryostatID; ///< Type of the parent ID.

    /// Type of the ID with the specified level `L`.
    template <std::size_t L>
    using ID_t = details::AbsIDtype<L, ThisID_t>;

    /// Type of the ID `A` levels above this one.
    template <std::size_t A>
    using UpperID_t = details::RelIDtype<A, ThisID_t>;

    /// Special code for an invalid ID.
    static constexpr TPCsetID_t InvalidID = std::numeric_limits<TPCsetID_t>::max();

    /// Index of the TPC set within its cryostat.
    TPCsetID_t TPCset = InvalidID;

    /// Default constructor: an invalid TPC set ID.
    constexpr TPCsetID() = default;

    /// Constructor: TPC set with index `s` in the cryostat identified by
    /// `cryoid`.
    constexpr TPCsetID(CryostatID const& cryoid, TPCsetID_t s) : CryostatID(cryoid), TPCset(s) {}

    /// Constructor: TPC set with index `s` in the cryostat index `c`.
    constexpr TPCsetID(CryostatID_t c, TPCsetID_t s) : CryostatID(c), TPCset(s) {}

    // comparison operators are out of class

    //@{
    /// Human-readable representation of the TPC set ID.
    std::string toString() const { return details::writeToString(*this); }
    explicit operator std::string() const { return toString(); }
    //@}

    // the following two methods are useful for (templated) abstraction
    /// Returns the value of the deepest ID available (TPC set's).
    constexpr auto const& deepestIndex() const { return TPCset; }
    /// Returns the deepest ID available (TPC set's).
    auto& deepestIndex() { return TPCset; }
    /// Return the parent ID of this one (a cryostat ID).
    constexpr ParentID_t const& parentID() const { return *this; }
    /// Return the parent ID of this one (a cryostat ID).
    ParentID_t& parentID() { return *this; }
    /// Returns the index level `Index` of this type.
    template <std::size_t Index = 0U>
    constexpr auto getIndex() const;
    /// Returns the index level `Index` of this type.
    template <std::size_t Index = 0U>
    auto& writeIndex();
    /// Returns the index `Above` levels higher than `Level`.
    template <std::size_t Above>
    constexpr auto getRelIndex() const;

    /// Conversion to TPCsetID (for convenience of notation).
    constexpr TPCsetID const& asTPCsetID() const { return *this; }
    /// Conversion to TPCsetID (for convenience of notation).
    TPCsetID& asTPCsetID() { return *this; }
    /// Conversion to TPCsetID (for convenience of notation).
    constexpr TPCsetID const& asConstTPCsetID() { return *this; }

    /// Returns < 0 if this is smaller than other, 0 if equal, > 0 if larger.
    constexpr int cmp(TPCsetID const& other) const
    {
      int cmp_res = CryostatID::cmp(other);
      if (cmp_res == 0) // same cryostat: compare TPC set
        return ThreeWayComparison(TPCset, other.TPCset);
      else // return the order of cryostats
        return cmp_res;
    } // cmp()

    /// Level of this element.
    static constexpr auto Level = readout::ElementLevel::TPCset;

    /// Return the value of the invalid TPC ID as a r-value.
    static constexpr TPCsetID_t getInvalidID() { return TPCsetID::InvalidID; }

  }; // struct TPCsetID

  /**
   * @brief Class identifying a set of planes sharing readout channels
   *
   * This identifier provides the TPC set location and a number representing
   * the set of planes.
   * This set is defined by not sharing readout channels with any other plane
   * outside the set.
   *
   * The name stands for "readout plane".
   */
  struct ROPID : public TPCsetID {
    using ROPID_t = unsigned int; ///< Type for the ID number.

    using ThisID_t = ROPID;      ///< Type of this ID.
    using ParentID_t = TPCsetID; ///< Type of the parent ID.

    /// Type of the ID with the specified level `L`.
    template <std::size_t L>
    using ID_t = details::AbsIDtype<L, ThisID_t>;

    /// Type of the ID `A` levels above this one.
    template <std::size_t A>
    using UpperID_t = details::RelIDtype<A, ThisID_t>;

    /// Special code for an invalid ID.
    static constexpr ROPID_t InvalidID = std::numeric_limits<ROPID_t>::max();

    ROPID_t ROP = InvalidID; ///< Index of the readout plane within its TPC set.

    /// Default constructor: an invalid plane ID.
    constexpr ROPID() = default;

    /// Constructor: readout plane with index `r` in the TPC set identified by
    /// `tpcsetid`.
    constexpr ROPID(TPCsetID const& tpcsetid, ROPID_t r) : TPCsetID(tpcsetid), ROP(r) {}

    /// Constructor: readout plane with index `r` in the cryostat index `c`,
    /// TPC set index `s`.
    constexpr ROPID(CryostatID_t c, TPCsetID_t s, ROPID_t r) : TPCsetID(c, s), ROP(r) {}

    // comparison operators are out of class

    //@{
    /// Human-readable representation of the ROP ID.
    std::string toString() const { return details::writeToString(*this); }
    explicit operator std::string() const { return toString(); }
    //@}

    // the following two methods are useful for (templated) abstraction
    /// Returns the value of the deepest ID available (readout plane's).
    constexpr auto const& deepestIndex() const { return ROP; }
    /// Returns the deepest ID available (readout plane's).
    auto& deepestIndex() { return ROP; }
    /// Return the parent ID of this one (a TPC set ID).
    constexpr ParentID_t const& parentID() const { return *this; }
    /// Return the parent ID of this one (a TPC set ID).
    ParentID_t& parentID() { return *this; }
    /// Returns the index level `Index` of this type.
    template <std::size_t Index = 0U>
    constexpr auto getIndex() const;
    /// Returns the index level `Index` of this type.
    template <std::size_t Index = 0U>
    auto& writeIndex();
    /// Returns the index `Above` levels higher than `Level`.
    template <std::size_t Above>
    constexpr auto getRelIndex() const;

    /// Conversion to ROPID (for convenience of notation).
    constexpr ROPID const& asROPID() const { return *this; }
    /// Conversion to ROPID (for convenience of notation).
    ROPID& asROPID() { return *this; }
    /// Conversion to ROPID (for convenience of notation).
    constexpr ROPID const& asConstROPID() { return *this; }

    /// Returns < 0 if this is smaller than other, 0 if equal, > 0 if larger.
    constexpr int cmp(ROPID const& other) const
    {
      int cmp_res = TPCsetID::cmp(other);
      if (cmp_res == 0) // same TPC set: compare plane
        return ThreeWayComparison(ROP, other.ROP);
      else // return the order of TPC set
        return cmp_res;
    } // cmp()

    /// Level of this element.
    static constexpr auto Level = readout::ElementLevel::ReadoutPlane;

    /// Return the value of the invalid ROP ID as a r-value.
    static constexpr ROPID_t getInvalidID() { return ROPID::InvalidID; }

  }; // struct ROPID

  //----------------------------------------------------------------------------
  //--- ID comparison operators
  //---

  /// @name Readout ID comparison operators
  /// @{
  /// @details The result of comparison with invalid IDs is undefined.

  /// Comparison: the IDs point to the same TPC set (validity is ignored)
  inline constexpr bool operator==(TPCsetID const& a, TPCsetID const& b)
  {
    return (a.asCryostatID() == b.asCryostatID()) && (a.TPCset == b.TPCset);
  } // operator== (TPCsetID, TPCsetID)

  /// Comparison: the IDs point to different TPC sets (validity is ignored)
  inline constexpr bool operator!=(TPCsetID const& a, TPCsetID const& b)
  {
    return (a.asCryostatID() != b.asCryostatID()) || (a.TPCset != b.TPCset);
  } // operator!= (TPCsetID, TPCsetID)

  /// Order TPCsetID in increasing Cryo, then TPC set
  inline constexpr bool operator<(TPCsetID const& a, TPCsetID const& b)
  {
    int cmp_res = a.asCryostatID().cmp(b);
    if (cmp_res == 0) // same cryostat: compare TPC set
      return a.TPCset < b.TPCset;
    else // return the order of cryostats
      return cmp_res < 0;
  } // operator< (TPCsetID, TPCsetID)

  /// Comparison: the IDs point to the same ROP (validity is ignored)
  inline constexpr bool operator==(ROPID const& a, ROPID const& b)
  {
    return (a.asTPCsetID() == b.asTPCsetID()) && (a.ROP == b.ROP);
  } // operator== (ROPID, ROPID)

  /// Comparison: the IDs point to different ROPs (validity is ignored)
  inline constexpr bool operator!=(ROPID const& a, ROPID const& b)
  {
    return (a.asTPCsetID() != b.asTPCsetID()) || (a.ROP != b.ROP);
  } // operator!= (ROPID, ROPID)

  /// Order ROPID in increasing Cryo, then TPC set, then ROP
  inline constexpr bool operator<(ROPID const& a, ROPID const& b)
  {
    int cmp_res = a.asTPCsetID().cmp(b);
    if (cmp_res == 0) // same TPC set: compare ROP
      return a.ROP < b.ROP;
    else // return the order of TPC set
      return cmp_res < 0;
  } // operator< (ROPID, ROPID)

  /// @}

  //----------------------------------------------------------------------------
  //--- ID output operators
  //---
  /// Generic output of TPCsetID to stream
  inline std::ostream& operator<<(std::ostream& out, TPCsetID const& sid)
  {
    out << sid.asCryostatID() << " S:" << sid.TPCset;
    return out;
  } // operator<< (TPCsetID)

  inline std::ostream& operator<<(std::ostream& out, ROPID const& rid)
  {
    out << rid.asTPCsetID() << " R:" << rid.ROP;
    return out;
  } // operator<< (ROPID)

} // namespace readout

//------------------------------------------------------------------------------
//--- template implementation
//------------------------------------------------------------------------------
template <std::size_t Index /* = 0U */>
constexpr auto readout::TPCsetID::getIndex() const
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
} // readout::TPCsetID::getIndex() const

template <std::size_t Index /* = 0U */>
auto& readout::TPCsetID::writeIndex()
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
} // readout::TPCsetID::writeIndex()

template <std::size_t Above>
constexpr auto readout::TPCsetID::getRelIndex() const
{
  static_assert(Above <= Level, "This ID type does not have the requested Index level.");
  return getIndex<Level - Above>();
} // readout::TPCsetID::getRelIndex()

//------------------------------------------------------------------------------
template <std::size_t Index /* = 0U */>
constexpr auto readout::ROPID::getIndex() const
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
} // readout::ROPID::getIndex() const

template <std::size_t Index /* = 0U */>
auto& readout::ROPID::writeIndex()
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
} // readout::ROPID::writeIndex()

template <std::size_t Above>
constexpr auto readout::ROPID::getRelIndex() const
{
  static_assert(Above <= Level, "This ID type does not have the requested Index level.");
  return getIndex<Level - Above>();
} // readout::ROPID::getRelIndex()

//------------------------------------------------------------------------------

#endif // LARCOREOBJ_SIMPLETYPESANDCONSTANTS_READOUT_TYPES_H
