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

  // We define our own cryostat ID as an alias of the one from geometry
  using CryostatID = geo::CryostatID;

  /**
   * @brief Class identifying a set of TPC sharing readout channels.
   *
   * This identifier provides the cryostat location and a number representing the set of
   * TPCs.  This set is defined by not sharing readout channels with any other TPC outside
   * the set.
   */
  struct TPCsetID : public CryostatID {
    using TPCsetID_t = unsigned short; ///< Type for the ID number.

    using ParentID_t = CryostatID; ///< Type of the parent ID.

    /// Special code for an invalid ID.
    static constexpr TPCsetID_t InvalidID = std::numeric_limits<TPCsetID_t>::max();

    /// Index of the TPC set within its cryostat.
    TPCsetID_t TPCset = InvalidID;

    /// Default constructor: an invalid TPC set ID.
    constexpr TPCsetID() = default;

    /// Constructor: TPC set with index `s` in the cryostat identified by `cryoid`.
    constexpr TPCsetID(CryostatID const& cryoid, TPCsetID_t s) : CryostatID(cryoid), TPCset(s) {}

    /// Constructor: TPC set with index `s` in the cryostat index `c`.
    constexpr TPCsetID(CryostatID_t c, TPCsetID_t s) : CryostatID(c), TPCset(s) {}

    static constexpr auto first() { return TPCsetID{CryostatID::first(), 0}; }
    static constexpr auto first(CryostatID const& id) { return TPCsetID{id, 0}; }

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
    constexpr ParentID_t& parentID() { return *this; }
    /// Returns the index level `Index` of this type.
    template <std::size_t Index>
    constexpr auto getIndex() const;

    /// Conversion to TPCsetID (for convenience of notation).
    constexpr CryostatID const& asCryostatID() const { return parentID(); }
    constexpr CryostatID& asCryostatID() { return parentID(); }

    /// Level of this element.
    static constexpr auto Level = geo::details::index_for<TPCsetID>();

    /// Return the value of the invalid TPC ID as a r-value.
    static constexpr TPCsetID_t getInvalidID() { return TPCsetID::InvalidID; }

  }; // struct TPCsetID

  /**
   * @brief Class identifying a set of planes sharing readout channels
   *
   * This identifier provides the TPC set location and a number representing the set of
   * planes.  This set is defined by not sharing readout channels with any other plane
   * outside the set.
   *
   * The name stands for "readout plane".
   */
  struct ROPID : public TPCsetID {
    using ROPID_t = unsigned int; ///< Type for the ID number.

    using ParentID_t = TPCsetID; ///< Type of the parent ID.

    /// Special code for an invalid ID.
    static constexpr ROPID_t InvalidID = std::numeric_limits<ROPID_t>::max();

    ROPID_t ROP = InvalidID; ///< Index of the readout plane within its TPC set.

    /// Default constructor: an invalid plane ID.
    constexpr ROPID() = default;

    /// Constructor: readout plane with index `r` in the TPC set identified by `tpcsetid`.
    constexpr ROPID(TPCsetID const& tpcsetid, ROPID_t r) : TPCsetID(tpcsetid), ROP(r) {}

    /// Constructor: readout plane with index `r` in the cryostat index `c`, TPC set index
    /// `s`.
    constexpr ROPID(CryostatID_t c, TPCsetID_t s, ROPID_t r) : TPCsetID(c, s), ROP(r) {}

    static constexpr auto first() { return ROPID{TPCsetID::first(), 0}; }
    static constexpr auto first(CryostatID const& id) { return ROPID{TPCsetID::first(id), 0}; }
    static constexpr auto first(TPCsetID const& id) { return ROPID{id, 0}; }

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
    constexpr ParentID_t& parentID() { return *this; }
    /// Returns the index level `Index` of this type.
    template <std::size_t Index>
    constexpr auto getIndex() const;

    /// Conversion to ROPID (for convenience of notation).
    constexpr TPCsetID const& asTPCsetID() const { return parentID(); }
    constexpr TPCsetID& asTPCsetID() { return parentID(); }

    /// Level of this element.
    static constexpr auto Level = geo::details::index_for<ROPID>();

    /// Return the value of the invalid ROP ID as a r-value.
    static constexpr ROPID_t getInvalidID() { return ROPID::InvalidID; }

  }; // struct ROPID

  //----------------------------------------------------------------------------
  //--- ID output operators
  //---
  /// Generic output of TPCsetID to stream
  inline std::ostream& operator<<(std::ostream& out, TPCsetID const& sid)
  {
    out << sid.asCryostatID() << " S:" << sid.TPCset;
    return out;
  }

  inline std::ostream& operator<<(std::ostream& out, ROPID const& rid)
  {
    out << rid.asTPCsetID() << " R:" << rid.ROP;
    return out;
  }

} // namespace readout

//------------------------------------------------------------------------------
//--- template implementation
//------------------------------------------------------------------------------
template <std::size_t Index>
constexpr auto readout::TPCsetID::getIndex() const
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
}

//------------------------------------------------------------------------------
template <std::size_t Index>
constexpr auto readout::ROPID::getIndex() const
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
}

//------------------------------------------------------------------------------

#endif // LARCOREOBJ_SIMPLETYPESANDCONSTANTS_READOUT_TYPES_H
