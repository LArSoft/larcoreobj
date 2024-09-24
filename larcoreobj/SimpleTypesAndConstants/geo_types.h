/**
 * @defgroup Geometry Detector geometry information
 * @file   larcoreobj/SimpleTypesAndConstants/geo_types.h
 * @brief  Definition of data types for geometry description.
 * @author Brian Rebel (brebel@fnal.gov)
 * @see    larcoreobj/SimpleTypesAndConstants/geo_types.cxx
 * @ingroup Geometry
 *
 * This library depends only on standard C++.
 *
 */

#ifndef LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_TYPES_H
#define LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_TYPES_H

// C++ standard libraries
#include <climits>
#include <cmath>
#include <iosfwd> // std::ostream
#include <limits> // std::numeric_limits<>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>

namespace geo::details {
  /// Write the argument into a string
  template <typename T>
  std::string writeToString(T const& value);

  template <typename T, typename = void>
  struct has_parent : std::false_type {};

  template <typename T>
  struct has_parent<T, std::void_t<typename T::ParentID_t>> : std::true_type {};

  /// Whether `ID` represents an element on top of the hierarchy.
  template <typename ID>
  constexpr bool isTopGeoElementID = !has_parent<ID>::value;

  template <typename T>
  constexpr auto index_impl(std::size_t& index)
  {
    if constexpr (has_parent<T>::value) { index_impl<typename T::ParentID_t>(++index); }
  }

  template <typename T>
  constexpr auto const index_for()
  {
    std::size_t index{};
    index_impl<T>(index);
    return index;
  }

  template <typename ID, std::size_t Index, typename = void>
  struct AbsIDtypeStruct;

  template <std::size_t Index, typename ID>
  using AbsIDtype = typename AbsIDtypeStruct<ID, Index>::type;

  template <std::size_t Index, typename ID>
  constexpr auto getAbsIDindex(ID const& id)
  {
    static_assert(Index <= ID::Level, "Index not available for this type.");
    if constexpr (Index == ID::Level)
      return id.deepestIndex();
    else
      return getAbsIDindex<Index>(id.parentID());
  }

  template <std::size_t Index, typename ID>
  auto& getAbsIDindex(ID& id)
  {
    static_assert(Index <= ID::Level, "Index not available for this type.");
    if constexpr (Index == ID::Level)
      return id.deepestIndex();
    else
      return getAbsIDindex<Index>(id.parentID());
  }

} // namespace geo::details

// BEGIN Geometry --------------------------------------------------------------
/**
 * @addtogroup Geometry
 * @see `geo::GeometryCore`
 *
 * The description of the detector as seen by LArSoft is accessed via LArSoft
 * "geometry system".
 *
 * A high level explanation of the geometry description is present in the
 * official [LArSoft collaboration web site](http://larsoft.org), at the bottom
 * of which further sources are detailed.
 *
 * The geometry system revolves around the `geo::GeometryCore` service provider,
 * which is the hub to access all the geometry information.
 * The information accessible via this interface include:
 * * geometric description of the TPC components (wires, cryostats...)
 * * geometric description of the optical detectors
 * * geometric description of the "auxiliary" detectors (i.e. every detector
 *   which is not the two above)
 * * a ROOT representation of the detector geometry
 * * relation between readout channels and physical components (especially TPC
 *   readout channels vs. wires)
 * * tools to navigate this information
 * * ... and a load more stuff.
 *
 * The service provider can be obtained including `larcore/Geometry/Geometry.h`
 * header and calling:
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
 * auto const& geom = *(lar::providerFrom<geo::Geometry>());
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 */
/// @{
namespace geo {

  // --- BEGIN -- Geometry enumerators -----------------------------------------
  /// @name Geometry enumerators
  /// @{

  enum class Coordinate { X, Y, Z };
  std::ostream& operator<<(std::ostream& os, Coordinate);
  constexpr int to_int(Coordinate const coord) noexcept
  {
    return static_cast<int>(coord);
  }

  /// Enumerate the possible plane projections
  typedef enum _plane_proj {
    kU,      ///< Planes which measure U.
    kV,      ///< Planes which measure V.
    kW,      ///< Planes which measure W (third view for Bo, MicroBooNE, etc).
    kZ = kW, ///< Planes which measure Z direction.
    kY,      ///< Planes which measure Y direction.
    kX,      ///< Planes which measure X direction.
    k3D,     ///< 3-dimensional objects, potentially hits, clusters, prongs, etc.
    kUnknown ///< Unknown view.
  } View_t;

  typedef enum _plane_orient {
    kHorizontal, ///< Planes that are in the horizontal plane.
    kVertical    ///< Planes that are in the vertical plane (e.g. ArgoNeuT).
  } Orient_t;

  typedef enum _plane_sigtype {
    kInduction,  ///< Signal from induction planes.
    kCollection, ///< Signal from collection planes.
    kMysteryType ///< Who knows?
  } SigType_t;

  /**
   * @brief Drift sign: positive or negative
   */
  enum class DriftSign {
    Unknown,  ///< Drift direction is unknown.
    Positive, ///< Drift towards positive values.
    Negative  ///< Drift towards negative values.
  };
  std::ostream& operator<<(std::ostream& os, DriftSign);

  constexpr int to_int(DriftSign const sign)
  {
    switch (sign) {
    case DriftSign::Positive: return 1;
    case DriftSign::Negative: return -1;
    case DriftSign::Unknown: break;
    }
    return 0;
  }

  struct DriftAxis {
    Coordinate coordinate;
    DriftSign sign;
  };
  bool operator==(DriftAxis a, DriftAxis b);
  bool operator!=(DriftAxis a, DriftAxis b);
  std::ostream& operator<<(std::ostream& os, DriftAxis);

  /// @}
  // --- END -- Geometry enumerators -------------------------------------------

  /// @{
  /// @name Geometry element IDs

  /// The data type to uniquely identify a cryostat.
  struct CryostatID {
    using CryostatID_t = unsigned int; ///< Type for the ID number.

    // not constexpr because we would need an implementation file to define it
    /// Special code for an invalid ID.
    static constexpr CryostatID_t InvalidID = std::numeric_limits<CryostatID_t>::max();

    bool isValid = false;              ///< Whether this ID points to a valid element.
    CryostatID_t Cryostat = InvalidID; ///< Index of cryostat.

    /// Default constructor: an invalid cryostat.
    constexpr CryostatID() = default;

    /// Constructor: valid ID of cryostat with index c
    explicit constexpr CryostatID(CryostatID_t c) : isValid(true), Cryostat(c) {}

    /// Constructor: valid ID of cryostat with index c
    constexpr CryostatID(CryostatID_t c, bool valid) : isValid(valid), Cryostat(c) {}

    static constexpr auto first() { return CryostatID{0, true}; }
    /// @{
    /// @name ID validity

    /// Returns true if the ID is valid
    explicit constexpr operator bool() const { return isValid; }

    /// Sets the validity of the ID.
    void setValidity(bool valid) { isValid = valid; }

    /// Sets the ID as valid.
    void markValid() { setValidity(true); }

    /// Sets the ID as invalid.
    void markInvalid() { setValidity(false); }
    /// @}

    // comparison operators are out of class

    //@{
    /// Human-readable representation of the cryostat ID.
    std::string toString() const { return details::writeToString(*this); }
    explicit operator std::string() const { return toString(); }
    //@}

    // the following four methods are useful for (templated) abstraction
    /// Returns the value of the deepest ID available (cryostat's).
    constexpr auto const& deepestIndex() const { return Cryostat; }
    /// Returns the deepest ID available (cryostat's).
    auto& deepestIndex() { return Cryostat; }
    /// Returns the index level `Index` of this type.
    template <std::size_t Index>
    constexpr auto getIndex() const;

    /// Level of this element.
    static constexpr auto Level = details::index_for<CryostatID>();

    /// Return the value of the invalid ID as a r-value
    static constexpr CryostatID_t getInvalidID() { return CryostatID::InvalidID; }

  }; // struct CryostatID

  /// The data type to uniquely identify a optical detector.
  struct OpDetID : public CryostatID {
    using OpDetID_t = unsigned int; ///< Type for the ID number.

    using ParentID_t = CryostatID; ///< Type of the parent ID.

    // not constexpr because we would need an implementation file to define it
    /// Special code for an invalid ID.
    static constexpr OpDetID_t InvalidID = std::numeric_limits<OpDetID_t>::max();

    /// Index of the optical detector within its cryostat.
    OpDetID_t OpDet = InvalidID;

    /// Default constructor: an invalid optical detector ID.
    constexpr OpDetID() = default;

    /// Constructor: optical detector with index `o` in the cryostat identified by
    /// `cryoid`
    constexpr OpDetID(CryostatID const& cryoid, OpDetID_t o) : CryostatID(cryoid), OpDet(o) {}

    /// Constructor: opdtical detector with index `o` in the cryostat index `c`
    constexpr OpDetID(CryostatID_t c, OpDetID_t o) : CryostatID(c), OpDet(o) {}

    static constexpr OpDetID first() { return OpDetID{CryostatID::first(), 0}; }

    // comparison operators are out of class

    //@{
    /// Human-readable representation of the optical detector ID.
    std::string toString() const { return details::writeToString(*this); }
    explicit operator std::string() const { return toString(); }
    //@}

    // the following two methods are useful for (templated) abstraction
    /// Returns the value of the deepest ID available (OpDet's).
    constexpr auto const& deepestIndex() const { return OpDet; }
    /// Returns the deepest ID available (OpDet's).
    auto& deepestIndex() { return OpDet; }
    /// Return the parent ID of this one (a cryostat ID).
    constexpr ParentID_t const& parentID() const { return *this; }
    constexpr ParentID_t& parentID() { return *this; }
    /// Returns the index level `Index` of this type.
    template <std::size_t Index>
    constexpr auto getIndex() const;

    /// Conversion to CryostatID (for convenience of notation).
    constexpr CryostatID const& asCryostatID() const { return parentID(); }
    constexpr CryostatID& asCryostatID() { return parentID(); }

    /// Level of this element.
    static constexpr auto Level = details::index_for<OpDetID>();

    /// Return the value of the invalid optical detector ID as a r-value
    static constexpr OpDetID_t getInvalidID() { return OpDetID::InvalidID; }

  }; // struct OpDetID

  /// The data type to uniquely identify a TPC.
  struct TPCID : public CryostatID {
    using TPCID_t = unsigned int; ///< Type for the ID number.

    using ParentID_t = CryostatID; ///< Type of the parent ID.

    // not constexpr because we would need an implementation file to define it
    /// Special code for an invalid ID.
    static constexpr TPCID_t InvalidID = std::numeric_limits<TPCID_t>::max();

    TPCID_t TPC = InvalidID; ///< Index of the TPC within its cryostat.

    /// Default constructor: an invalid TPC ID.
    constexpr TPCID() = default;

    /// Constructor: TPC with index t in the cryostat identified by cryoid
    constexpr TPCID(CryostatID const& cryoid, TPCID_t t) : CryostatID(cryoid), TPC(t) {}

    /// Constructor: TPC with index t in the cryostat index c
    constexpr TPCID(CryostatID_t c, TPCID_t t) : CryostatID(c), TPC(t) {}

    TPCID next() const { return {Cryostat, TPC + 1}; }

    static constexpr auto first() { return TPCID{CryostatID::first(), 0}; }
    static constexpr auto first(CryostatID const& id) { return TPCID{id, 0}; }

    // comparison operators are out of class

    //@{
    /// Human-readable representation of the TPC ID.
    std::string toString() const { return details::writeToString(*this); }
    explicit operator std::string() const { return toString(); }
    //@}

    // the following two methods are useful for (templated) abstraction
    /// Returns the value of the deepest ID available (TPC's).
    constexpr auto const& deepestIndex() const { return TPC; }
    /// Returns the deepest ID available (TPC's).
    auto& deepestIndex() { return TPC; }
    /// Return the parent ID of this one (a cryostat ID).
    constexpr ParentID_t const& parentID() const { return *this; }
    constexpr ParentID_t& parentID() { return *this; }
    /// Returns the index level `Index` of this type.
    template <std::size_t Index>
    constexpr auto getIndex() const;

    /// Conversion to TPCID (for convenience of notation).
    constexpr CryostatID const& asCryostatID() const { return parentID(); }
    constexpr CryostatID& asCryostatID() { return parentID(); }

    /// Level of this element.
    static constexpr auto Level = details::index_for<TPCID>();

    /// Return the value of the invalid TPC ID as a r-value
    static constexpr TPCID_t getInvalidID() { return TPCID::InvalidID; }

  }; // struct TPCID

  /// The data type to uniquely identify a Plane.
  struct PlaneID : public TPCID {
    using PlaneID_t = unsigned int; ///< Type for the ID number.

    using ParentID_t = TPCID; ///< Type of the parent ID.

    // not constexpr because we would need an implementation file to define it
    /// Special code for an invalid ID.
    static constexpr PlaneID_t InvalidID = std::numeric_limits<PlaneID_t>::max();

    PlaneID_t Plane = InvalidID; ///< Index of the plane within its TPC.

    /// Default constructor: an invalid plane ID.
    constexpr PlaneID() = default;

    /// Constructor: plane with index p in the TPC identified by tpcid
    constexpr PlaneID(TPCID const& tpcid, PlaneID_t p) : TPCID(tpcid), Plane(p) {}

    /// Constructor: plane with index p in the cryostat index c, TPC index t
    constexpr PlaneID(CryostatID_t c, TPCID_t t, PlaneID_t p) : TPCID(c, t), Plane(p) {}

    static constexpr auto first() { return PlaneID{TPCID::first(), 0}; }
    static constexpr auto first(CryostatID const& id) { return PlaneID{TPCID::first(id), 0}; }
    static constexpr auto first(TPCID const& id) { return PlaneID{id, 0}; }

    // comparison operators are out of class

    //@{
    /// Human-readable representation of the plane ID.
    std::string toString() const { return details::writeToString(*this); }
    explicit operator std::string() const { return toString(); }
    //@}

    // the following two methods are useful for (templated) abstraction
    /// Returns the value of the deepest ID available (plane's).
    constexpr auto const& deepestIndex() const { return Plane; }
    /// Returns the deepest ID available (plane's).
    auto& deepestIndex() { return Plane; }
    /// Return the parent ID of this one (a TPC ID).
    constexpr ParentID_t const& parentID() const { return *this; }
    constexpr ParentID_t& parentID() { return *this; }
    /// Returns the index level `Index` of this type.
    template <std::size_t Index>
    constexpr auto getIndex() const;

    /// Conversion to PlaneID (for convenience of notation).
    constexpr TPCID const& asTPCID() const { return parentID(); }
    constexpr TPCID& asTPCID() { return parentID(); }

    /// Level of this element.
    static constexpr auto Level = details::index_for<PlaneID>();

    /// Return the value of the invalid plane ID as a r-value
    static constexpr PlaneID_t getInvalidID() { return PlaneID::InvalidID; }

  }; // struct PlaneID

  // The data type to uniquely identify a code wire segment.
  struct WireID : public PlaneID {
    using WireID_t = unsigned int; ///< Type for the ID number.

    using ParentID_t = PlaneID; ///< Type of the parent ID.

    // not constexpr because we would need an implementation file to define it
    /// Special code for an invalid ID.
    static constexpr WireID_t InvalidID = std::numeric_limits<WireID_t>::max();

    WireID_t Wire = InvalidID; ///< Index of the wire within its plane.

    /// Default constructor: an invalid TPC ID.
    constexpr WireID() = default;

    /// Constructor: wire with index w in the plane identified by planeid
    constexpr WireID(PlaneID const& planeid, WireID_t w) : PlaneID(planeid), Wire(w) {}

    /// Constructor: wire with index w in cryostat index c, TPC index t, plane index p
    constexpr WireID(CryostatID_t c, TPCID_t t, PlaneID_t p, WireID_t w) : PlaneID(c, t, p), Wire(w)
    {}

    static constexpr auto first() { return WireID{PlaneID::first(), 0}; }
    static constexpr auto first(CryostatID const& id) { return WireID{PlaneID::first(id), 0}; }
    static constexpr auto first(TPCID const& id) { return WireID{PlaneID::first(id), 0}; }
    static constexpr auto first(PlaneID const& id) { return WireID{id, 0}; }

    //@{
    /// Human-readable representation of the wire ID.
    std::string toString() const { return details::writeToString(*this); }
    explicit operator std::string() const { return toString(); }
    //@}

    // the following two methods are useful for (templated) abstraction
    /// Returns the value of the deepest ID available (wire's).
    constexpr auto const& deepestIndex() const { return Wire; }
    /// Returns the deepest ID available (wire's).
    auto& deepestIndex() { return Wire; }
    /// Return the parent ID of this one (a plane ID).
    constexpr ParentID_t const& parentID() const { return *this; }
    constexpr ParentID_t& parentID() { return *this; }
    /// Returns the index level `Index` of this type.
    template <std::size_t Index>
    constexpr auto getIndex() const;

    /// Conversion to WireID (for convenience of notation).
    constexpr PlaneID const& asPlaneID() const { return parentID(); }
    constexpr PlaneID& asPlaneID() { return parentID(); }

    /// Backward compatibility; use the wire directly or a explicit cast instead
    /// @todo Remove the instances of geo::WireID::planeID() in the code
    constexpr PlaneID const& planeID() const { return *this; }

    /// Level of this element.
    static constexpr auto Level = details::index_for<WireID>();

    /// Return the value of the invalid wire ID as a r-value
    static constexpr WireID_t getInvalidID() { return WireID::InvalidID; }

  }; // struct WireID

  //----------------------------------------------------------------------------
  //--- ID output operators
  //---
  /// Generic output of CryostatID to stream
  inline std::ostream& operator<<(std::ostream& out, CryostatID const& cid)
  {
    out << "C:" << cid.Cryostat;
    return out;
  }

  /// Generic output of OpDetID to stream.
  inline std::ostream& operator<<(std::ostream& out, OpDetID const& oid)
  {
    out << oid.parentID() << " O:" << oid.OpDet;
    return out;
  }

  /// Generic output of TPCID to stream
  inline std::ostream& operator<<(std::ostream& out, TPCID const& tid)
  {
    out << tid.parentID() << " T:" << tid.TPC;
    return out;
  }

  /// Generic output of PlaneID to stream
  inline std::ostream& operator<<(std::ostream& out, PlaneID const& pid)
  {
    out << pid.parentID() << " P:" << pid.Plane;
    return out;
  }

  /// Generic output of WireID to stream
  inline std::ostream& operator<<(std::ostream& out, WireID const& wid)
  {
    out << wid.parentID() << " W:" << wid.Wire;
    return out;
  }

  /// @}
  // Geometry element IDs

  //----------------------------------------------------------------------------
  //--- ID comparison operators
  //---

  /// @{
  /// @name ID comparison operators
  /// @details The result of comparison with invalid IDs is undefined.

  /// Comparison: the IDs point to the same cryostat (validity is ignored)
  inline constexpr bool operator==(CryostatID const& a, CryostatID const& b)
  {
    return a.Cryostat == b.Cryostat;
  }

  /// Comparison: the IDs point to different cryostats (validity is ignored)
  inline constexpr bool operator!=(CryostatID const& a, CryostatID const& b)
  {
    return !(a == b);
  }

  /// Order cryostats with increasing ID
  inline constexpr bool operator<(CryostatID const& a, CryostatID const& b)
  {
    return a.Cryostat < b.Cryostat;
  }

  template <typename BaseID, typename GeoID>
  static constexpr bool is_base_of_strict{std::is_base_of<BaseID, GeoID>{} &&
                                          !std::is_same<BaseID, GeoID>{}};

  /// Comparison: the IDs point to same optical detector (validity is ignored)
  template <typename GeoID>
  inline constexpr std::enable_if_t<is_base_of_strict<CryostatID, GeoID>, bool> operator==(
    GeoID const& a,
    GeoID const& b)
  {
    return std::tie(a.parentID(), a.deepestIndex()) == std::tie(b.parentID(), b.deepestIndex());
  }

  /// Comparison: IDs point to different optical detectors (validity is ignored)
  template <typename GeoID>
  inline constexpr std::enable_if_t<is_base_of_strict<CryostatID, GeoID>, bool> operator!=(
    GeoID const& a,
    GeoID const& b)
  {
    return !(a == b);
  }

  /// Order OpDetID in increasing Cryo, then OpDet
  template <typename GeoID>
  inline constexpr std::enable_if_t<is_base_of_strict<CryostatID, GeoID>, bool> operator<(
    GeoID const& a,
    GeoID const& b)
  {
    return std::tie(a.parentID(), a.deepestIndex()) < std::tie(b.parentID(), b.deepestIndex());
  }

  /// @}

  //----------------------------------------------------------------------------
  struct WireIDIntersection {
    double y;         ///< y position of intersection
    double z;         ///< z position of intersection
    unsigned int TPC; ///< TPC of intersection

    // In APAs, we want this to increase in the direction wireID
    // index increases in: moving inward vertically towards y=0
    bool operator<(WireIDIntersection const& otherIntersect) const
    {
      return std::abs(y) > std::abs(otherIntersect.y);
    }

    static constexpr WireIDIntersection invalid()
    {
      return {
        std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity(), -1u};
    }
  };

  //----------------------------------------------------------------------------
  /// Returns the name of the specified signal type.
  std::string SignalTypeName(geo::SigType_t sigType);

  //----------------------------------------------------------------------------

} // namespace geo
/// @}
// END Geometry ----------------------------------------------------------------

namespace geo::details {

  //--------------------------------------------------------------------------
  template <typename ID, std::size_t Index, typename /* = void */>
  struct AbsIDtypeStruct {
    static_assert(Index <= ID::Level, "Requested ID index is not available.");
    using type = typename AbsIDtypeStruct<typename ID::ParentID_t, Index>::type;
  };

  template <typename ID, std::size_t Index>
  struct AbsIDtypeStruct<ID, Index, std::enable_if_t<(Index == ID::Level)>> {
    using type = ID;
  };

  //--------------------------------------------------------------------------
  template <typename T>
  inline std::string writeToString(T const& value)
  {
    std::ostringstream sstr;
    sstr << value;
    return sstr.str();
  }

  //--------------------------------------------------------------------------

} // namespace geo::details

//------------------------------------------------------------------------------
//--- template implementation
//------------------------------------------------------------------------------
template <std::size_t Index>
constexpr auto geo::CryostatID::getIndex() const
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
}

//------------------------------------------------------------------------------
template <std::size_t Index>
constexpr auto geo::OpDetID::getIndex() const
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
}

//------------------------------------------------------------------------------
template <std::size_t Index>
constexpr auto geo::TPCID::getIndex() const
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
}

//------------------------------------------------------------------------------
template <std::size_t Index>
constexpr auto geo::PlaneID::getIndex() const
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
}

//------------------------------------------------------------------------------
template <std::size_t Index>
constexpr auto geo::WireID::getIndex() const
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
}

//------------------------------------------------------------------------------

#endif // LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_TYPES_H
