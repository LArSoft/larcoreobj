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

namespace geo {
  namespace details {
    /// Write the argument into a string
    template <typename T>
    std::string writeToString(T const& value);

    /// Whether `ID` represents an element on top of the hierarchy.
    template <typename ID>
    constexpr bool isTopGeoElementID = std::is_void_v<typename ID::ParentID_t>;

    template <typename ID>
    constexpr std::size_t geoElementLevel()
    {
      if constexpr (isTopGeoElementID<ID>)
        return 0U;
      else
        return geoElementLevel<typename ID::ParentID_t>() + 1U;
    } // geoElementLevel()

    template <typename ID, std::size_t Index, typename = void>
    struct AbsIDtypeStruct;

    template <std::size_t Index, typename ID>
    using AbsIDtype = typename AbsIDtypeStruct<ID, Index>::type;

    template <typename ID, std::size_t UpIndex>
    struct RelIDtypeStruct;

    template <std::size_t UpIndex, typename ID>
    using RelIDtype = typename RelIDtypeStruct<ID, UpIndex>::type;

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

    template <std::size_t UpIndex, typename ID>
    auto getRelIDindex(ID const& id)
    {
      static_assert(UpIndex <= ID::Level, "Index not available for this type.");
      if constexpr (UpIndex == 0)
        return id.deepestIndex();
      else
        return getRelIDindex<UpIndex - 1U>(id.parentID());
    }

  } // namespace details
} // namespace geo

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

  typedef enum coordinates {
    kXCoord, ///< X coordinate.
    kYCoord, ///< Y coordinate.
    kZCoord  ///< Z coordinate.
  } Coord_t;

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
   * @brief Drift direction: positive or negative
   *
   * Do not use this type to distinguish different drift axes: e.g., negative
   * x drift and negative z drift are both by `kNeg`.
   */
  typedef enum driftdir {
    kUnknownDrift, ///< Drift direction is unknown.
    kPos,          ///< Drift towards positive values.
    kNeg,          ///< Drift towards negative values.
    kPosX = kPos,  ///< Drift towards positive X values.
    kNegX = kNeg   ///< Drift towards negative X values.
  } DriftDirection_t;

  /// Numerical description of geometry element "level".
  /// The "detector" level is noticeably missing.
  struct ElementLevel {

    using Level_t = std::size_t;

    static constexpr Level_t Cryostat = 0U;
    static constexpr Level_t OpDet = 1U;
    static constexpr Level_t TPC = 1U;
    static constexpr Level_t Plane = 2U;
    static constexpr Level_t Wire = 3U;
    static constexpr Level_t NLevels = 4U;

  }; // struct ElementLevel

  /// @}
  // --- END -- Geometry enumerators -------------------------------------------

  /// @{
  /// @name Geometry element IDs

  /// The data type to uniquely identify a cryostat.
  struct CryostatID {
    using CryostatID_t = unsigned int; ///< Type for the ID number.

    using ThisID_t = CryostatID; ///< Type of this ID.
    using ParentID_t = void;     ///< Type of the parent ID (none!).

    /// Type of the ID with the specified level `L`.
    template <std::size_t L>
    using ID_t = details::AbsIDtype<L, ThisID_t>;

    /// Type of the ID `A` levels above this one.
    template <std::size_t A>
    using UpperID_t = details::RelIDtype<A, ThisID_t>;

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

    /// @{
    /// @name ID validity

    /// Returns true if the ID is valid
    explicit constexpr operator bool() const { return isValid; }

    /// Returns true if the ID is not valid
    constexpr bool operator!() const { return !isValid; }

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
    /// Return the parent ID of this one (void).
    constexpr ParentID_t parentID() const {}
    /// Return the parent ID of this one (void).
    ParentID_t parentID() {}
    /// Returns the index level `Index` of this type.
    template <std::size_t Index = 0U>
    constexpr auto getIndex() const;
    /// Returns the index level `Index` of this type.
    template <std::size_t Index = 0U>
    auto& writeIndex();
    /// Returns the index `Above` levels higher than `Level`.
    template <std::size_t Above>
    constexpr auto getRelIndex() const;

    /// Returns < 0 if this is smaller than other, 0 if equal, > 0 if larger
    constexpr int cmp(CryostatID const& other) const
    {
      return ThreeWayComparison(deepestIndex(), other.deepestIndex());
    }

    /// Conversion to CryostatID (for convenience of notation).
    constexpr CryostatID const& asCryostatID() const { return *this; }
    /// Conversion to CryostatID (for convenience of notation).
    CryostatID& asCryostatID() { return *this; }
    /// Conversion to CryostatID (for convenience of notation).
    constexpr CryostatID const& asConstCryostatID() { return *this; }

    /// Level of this element.
    static constexpr auto Level = geo::ElementLevel::Cryostat;

    /// Return the value of the invalid ID as a r-value
    static constexpr CryostatID_t getInvalidID() { return CryostatID::InvalidID; }

    /// Returns < 0 if a < b, 0 if a == b, > 0 if a > b
    template <typename T>
    static constexpr int ThreeWayComparison(T a, T b)
    {
      return (a == b) ? 0 : ((a < b) ? -1 : +1);
    }

  }; // struct CryostatID

  /// The data type to uniquely identify a optical detector.
  struct OpDetID : public CryostatID {
    using OpDetID_t = unsigned int; ///< Type for the ID number.

    using ThisID_t = OpDetID;      ///< Type of this ID.
    using ParentID_t = CryostatID; ///< Type of the parent ID.

    /// Type of the ID with the specified level `L`.
    template <std::size_t L>
    using ID_t = details::AbsIDtype<L, ThisID_t>;

    /// Type of the ID `A` levels above this one.
    template <std::size_t A>
    using UpperID_t = details::RelIDtype<A, ThisID_t>;

    // not constexpr because we would need an implementation file to define it
    /// Special code for an invalid ID.
    static constexpr OpDetID_t InvalidID = std::numeric_limits<OpDetID_t>::max();

    /// Index of the optical detector within its cryostat.
    OpDetID_t OpDet = InvalidID;

    /// Default constructor: an invalid optical detector ID.
    constexpr OpDetID() = default;

    /// Constructor: optical detector with index `o` in the cryostat identified
    /// by `cryoid`
    constexpr OpDetID(CryostatID const& cryoid, OpDetID_t o) : CryostatID(cryoid), OpDet(o) {}

    /// Constructor: opdtical detector with index `o` in the cryostat index `c`
    constexpr OpDetID(CryostatID_t c, OpDetID_t o) : CryostatID(c), OpDet(o) {}

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

    /// Conversion to OpDetID (for convenience of notation).
    constexpr OpDetID const& asOpDetID() const { return *this; }
    /// Conversion to OpDetID (for convenience of notation).
    OpDetID& asOpDetID() { return *this; }
    /// Conversion to OpDetID (for convenience of notation).
    constexpr OpDetID const& asConstOpDetID() { return *this; }

    /// Returns < 0 if this is smaller than other, 0 if equal, > 0 if larger
    constexpr int cmp(OpDetID const& other) const
    {
      int cmp_res = CryostatID::cmp(other);
      if (cmp_res == 0) // same cryostat: compare optical detectors
        return ThreeWayComparison(deepestIndex(), other.deepestIndex());
      else // return the order of cryostats
        return cmp_res;
    } // cmp()

    /// Level of this element.
    static constexpr auto Level = geo::ElementLevel::OpDet;

    /// Return the value of the invalid optical detector ID as a r-value
    static constexpr OpDetID_t getInvalidID() { return OpDetID::InvalidID; }

  }; // struct OpDetID

  /// The data type to uniquely identify a TPC.
  struct TPCID : public CryostatID {
    using TPCID_t = unsigned int; ///< Type for the ID number.

    using ThisID_t = TPCID;        ///< Type of this ID.
    using ParentID_t = CryostatID; ///< Type of the parent ID.

    /// Type of the ID with the specified level `L`.
    template <std::size_t L>
    using ID_t = details::AbsIDtype<L, ThisID_t>;

    /// Type of the ID `A` levels above this one.
    template <std::size_t A>
    using UpperID_t = details::RelIDtype<A, ThisID_t>;

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

    /// Conversion to TPCID (for convenience of notation).
    constexpr TPCID const& asTPCID() const { return *this; }
    /// Conversion to TPCID (for convenience of notation).
    TPCID& asTPCID() { return *this; }
    /// Conversion to TPCID (for convenience of notation).
    constexpr TPCID const& asConstTPCID() { return *this; }

    /// Returns < 0 if this is smaller than other, 0 if equal, > 0 if larger
    constexpr int cmp(TPCID const& other) const
    {
      int cmp_res = CryostatID::cmp(other);
      if (cmp_res == 0) // same cryostat: compare TPC
        return ThreeWayComparison(deepestIndex(), other.deepestIndex());
      else // return the order of cryostats
        return cmp_res;
    } // cmp()

    /// Level of this element.
    static constexpr auto Level = geo::ElementLevel::TPC;

    /// Return the value of the invalid TPC ID as a r-value
    static constexpr TPCID_t getInvalidID() { return TPCID::InvalidID; }

  }; // struct TPCID

  /// The data type to uniquely identify a Plane.
  struct PlaneID : public TPCID {
    using PlaneID_t = unsigned int; ///< Type for the ID number.

    using ThisID_t = PlaneID; ///< Type of this ID.
    using ParentID_t = TPCID; ///< Type of the parent ID.

    /// Type of the ID with the specified level `L`.
    template <std::size_t L>
    using ID_t = details::AbsIDtype<L, ThisID_t>;

    /// Type of the ID `A` levels above this one.
    template <std::size_t A>
    using UpperID_t = details::RelIDtype<A, ThisID_t>;

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
    /// Return the parent ID of this one (a TPC ID).
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

    /// Conversion to PlaneID (for convenience of notation).
    constexpr PlaneID const& asPlaneID() const { return *this; }
    /// Conversion to PlaneID (for convenience of notation).
    PlaneID& asPlaneID() { return *this; }
    /// Conversion to PlaneID (for convenience of notation).
    constexpr PlaneID const& asConstPlaneID() { return *this; }

    /// Returns < 0 if this is smaller than other, 0 if equal, > 0 if larger
    constexpr int cmp(PlaneID const& other) const
    {
      int cmp_res = TPCID::cmp(other);
      if (cmp_res == 0) // same TPC: compare plane
        return ThreeWayComparison(deepestIndex(), other.deepestIndex());
      else // return the order of planes
        return cmp_res;
    } // cmp()

    /// Level of this element.
    static constexpr auto Level = geo::ElementLevel::Plane;

    /// Return the value of the invalid plane ID as a r-value
    static constexpr PlaneID_t getInvalidID() { return PlaneID::InvalidID; }

  }; // struct PlaneID

  // The data type to uniquely identify a code wire segment.
  struct WireID : public PlaneID {
    using WireID_t = unsigned int; ///< Type for the ID number.

    using ThisID_t = WireID;    ///< Type of this ID.
    using ParentID_t = PlaneID; ///< Type of the parent ID.

    /// Type of the ID with the specified level `L`.
    template <std::size_t L>
    using ID_t = details::AbsIDtype<L, ThisID_t>;

    /// Type of the ID `A` levels above this one.
    template <std::size_t A>
    using UpperID_t = details::RelIDtype<A, ThisID_t>;

    // not constexpr because we would need an implementation file to define it
    /// Special code for an invalid ID.
    static constexpr WireID_t InvalidID = std::numeric_limits<WireID_t>::max();

    WireID_t Wire = InvalidID; ///< Index of the wire within its plane.

    /// Default constructor: an invalid TPC ID.
    constexpr WireID() = default;

    /// Constructor: wire with index w in the plane identified by planeid
    constexpr WireID(PlaneID const& planeid, WireID_t w) : PlaneID(planeid), Wire(w) {}

    /// Constructor: wire with index w in cryostat index c, TPC index t,
    /// plane index p
    constexpr WireID(CryostatID_t c, TPCID_t t, PlaneID_t p, WireID_t w) : PlaneID(c, t, p), Wire(w)
    {}

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
    /// Return the parent ID of this one (a plane ID).
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

    /// Conversion to WireID (for convenience of notation).
    constexpr WireID const& asWireID() const { return *this; }
    /// Conversion to WireID (for convenience of notation).
    WireID& asWireID() { return *this; }
    /// Conversion to WireID (for convenience of notation).
    constexpr WireID const& asConstWireID() { return *this; }

    /// Returns < 0 if this is smaller than tpcid, 0 if equal, > 0 if larger
    constexpr int cmp(WireID const& other) const
    {
      int cmp_res = PlaneID::cmp(other);
      if (cmp_res == 0) // same plane: compare wires
        return ThreeWayComparison(deepestIndex(), other.deepestIndex());
      else // return the order of wire
        return cmp_res;
    } // cmp()

    /// Backward compatibility; use the wire directly or a explicit cast instead
    /// @todo Remove the instances of geo::WireID::planeID() in the code
    constexpr PlaneID const& planeID() const { return *this; }

    /// Level of this element.
    static constexpr auto Level = geo::ElementLevel::Wire;

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
  } // operator<< (CryostatID)

  /// Generic output of OpDetID to stream.
  inline std::ostream& operator<<(std::ostream& out, OpDetID const& oid)
  {
    out << oid.asCryostatID() << " O:" << oid.OpDet;
    return out;
  } // operator<< (OpDetID)

  /// Generic output of TPCID to stream
  inline std::ostream& operator<<(std::ostream& out, TPCID const& tid)
  {
    out << ((CryostatID const&)tid) << " T:" << tid.TPC;
    return out;
  } // operator<< (TPCID)

  /// Generic output of PlaneID to stream
  inline std::ostream& operator<<(std::ostream& out, PlaneID const& pid)
  {
    out << ((TPCID const&)pid) << " P:" << pid.Plane;
    return out;
  } // operator<< (PlaneID)

  /// Generic output of WireID to stream
  inline std::ostream& operator<<(std::ostream& out, WireID const& wid)
  {
    out << ((PlaneID const&)wid) << " W:" << wid.Wire;
    return out;
  } // operator<< (WireID)

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
    return a.Cryostat != b.Cryostat;
  }

  /// Order cryostats with increasing ID
  inline constexpr bool operator<(CryostatID const& a, CryostatID const& b)
  {
    return a.Cryostat < b.Cryostat;
  }

  /// Comparison: the IDs point to same optical detector (validity is ignored)
  inline constexpr bool operator==(OpDetID const& a, OpDetID const& b)
  {
    return (a.asCryostatID() == b.asCryostatID()) && (a.OpDet == b.OpDet);
  }

  /// Comparison: IDs point to different optical detectors (validity is ignored)
  inline constexpr bool operator!=(OpDetID const& a, OpDetID const& b)
  {
    return (a.asCryostatID() != b.asCryostatID()) || (a.OpDet != b.OpDet);
  }

  /// Order OpDetID in increasing Cryo, then OpDet
  inline constexpr bool operator<(OpDetID const& a, OpDetID const& b)
  {
    int cmp_res = a.asCryostatID().cmp(b);
    if (cmp_res == 0) // same cryostat: compare optical detectors
      return a.OpDet < b.OpDet;
    else // return the order of cryostats
      return cmp_res < 0;
  } // operator< (OpDetID, OpDetID)

  /// Comparison: the IDs point to the same TPC (validity is ignored)
  inline constexpr bool operator==(TPCID const& a, TPCID const& b)
  {
    return (static_cast<CryostatID const&>(a) == static_cast<CryostatID const&>(b)) &&
           (a.TPC == b.TPC);
  } // operator== (TPCID, TPCID)

  /// Comparison: the IDs point to different TPCs (validity is ignored)
  inline constexpr bool operator!=(TPCID const& a, TPCID const& b)
  {
    return (static_cast<CryostatID const&>(a) != static_cast<CryostatID const&>(b)) ||
           (a.TPC != b.TPC);
  } // operator!= (TPCID, TPCID)

  /// Order TPCID in increasing Cryo, then TPC
  inline constexpr bool operator<(TPCID const& a, TPCID const& b)
  {
    int cmp_res = (static_cast<CryostatID const&>(a)).cmp(b);
    if (cmp_res == 0) // same cryostat: compare TPC
      return a.TPC < b.TPC;
    else // return the order of cryostats
      return cmp_res < 0;
  } // operator< (TPCID, TPCID)

  /// Comparison: the IDs point to the same plane (validity is ignored)
  inline constexpr bool operator==(PlaneID const& a, PlaneID const& b)
  {
    return (static_cast<TPCID const&>(a) == static_cast<TPCID const&>(b)) && (a.Plane == b.Plane);
  } // operator== (PlaneID, PlaneID)

  /// Comparison: the IDs point to different planes (validity is ignored)
  inline constexpr bool operator!=(PlaneID const& a, PlaneID const& b)
  {
    return (static_cast<TPCID const&>(a) != static_cast<TPCID const&>(b)) || (a.Plane != b.Plane);
  } // operator!= (PlaneID, PlaneID)

  /// Order PlaneID in increasing TPC, then plane
  inline constexpr bool operator<(PlaneID const& a, PlaneID const& b)
  {
    int cmp_res = (static_cast<TPCID const&>(a)).cmp(b);
    if (cmp_res == 0) // same TPC: compare plane
      return a.Plane < b.Plane;
    else // return the order of TPC
      return cmp_res < 0;
  } // operator< (PlaneID, PlaneID)

  /// Comparison: the IDs point to the same wire (validity is ignored)
  inline constexpr bool operator==(WireID const& a, WireID const& b)
  {
    return (static_cast<PlaneID const&>(a) == static_cast<PlaneID const&>(b)) && (a.Wire == b.Wire);
  } // operator== (WireID, WireID)

  /// Comparison: the IDs point to different wires (validity is ignored)
  inline constexpr bool operator!=(WireID const& a, WireID const& b)
  {
    return (static_cast<PlaneID const&>(a) != static_cast<PlaneID const&>(b)) || (a.Wire != b.Wire);
  } // operator!= (WireID, WireID)

  // Order WireID in increasing plane, then wire
  inline constexpr bool operator<(WireID const& a, WireID const& b)
  {
    int cmp_res = (static_cast<PlaneID const&>(a)).cmp(b);
    if (cmp_res == 0) // same plane: compare wire
      return a.Wire < b.Wire;
    else // return the order of planes
      return cmp_res < 0;
  } // operator< (WireID, WireID)

  /// @}

  //----------------------------------------------------------------------------
  struct WireIDIntersection {
    double y;         ///< y position of intersection
    double z;         ///< z position of intersection
    unsigned int TPC; ///< TPC of intersection

    // In APAs, we want this to increase in the direction wireID
    // index increases in: moving inward vertically towards y=0
    bool operator<(const WireIDIntersection& otherIntersect) const
    {
      return std::abs(y) > std::abs(otherIntersect.y);
    }
  };

  //----------------------------------------------------------------------------
  /// Returns the name of the specified signal type.
  std::string SignalTypeName(geo::SigType_t sigType);

  //----------------------------------------------------------------------------

} // namespace geo
/// @}
// END Geometry ----------------------------------------------------------------

namespace geo {
  namespace details {

    //--------------------------------------------------------------------------
    template <typename ID, std::size_t Index, typename /* = void */>
    struct AbsIDtypeStruct {
      static_assert(Index <= ID::Level, "Requested ID index is not available.");
      using type = typename AbsIDtypeStruct<typename ID::ParentID_t, Index>::type;
    }; // AbsIDtypeStruct<>

    template <typename ID, std::size_t Index>
    struct AbsIDtypeStruct<ID, Index, std::enable_if_t<(Index == ID::Level)>> {
      using type = ID;
    };

    //--------------------------------------------------------------------------
    template <typename ID, std::size_t UpIndex>
    struct RelIDtypeStruct {
      static_assert(UpIndex <= ID::Level, "Requested parent ID index is not available.");
      using type = typename RelIDtypeStruct<typename ID::ParentID_t, UpIndex - 1U>::type;
    }; // RelIDtypeStruct<>

    template <typename ID>
    struct RelIDtypeStruct<ID, 0U> {
      using type = ID;
    };

    //--------------------------------------------------------------------------
    template <typename T>
    inline std::string writeToString(T const& value)
    {
      std::ostringstream sstr;
      sstr << value;
      return sstr.str();
    } // writeToString()

    //--------------------------------------------------------------------------

  } // namespace details

} // namespace geo

//------------------------------------------------------------------------------
//--- template implementation
//------------------------------------------------------------------------------
template <std::size_t Index /* = 0U */>
constexpr auto geo::CryostatID::getIndex() const
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
} // geo::CryostatID::getIndex() const

template <std::size_t Index /* = 0U */>
auto& geo::CryostatID::writeIndex()
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
} // geo::CryostatID::writeIndex()

template <std::size_t Above>
constexpr auto geo::CryostatID::getRelIndex() const
{
  static_assert(Above <= Level, "This ID type does not have the requested Index level.");
  return getIndex<Level - Above>();
} // geo::CryostatID::getRelIndex()

//------------------------------------------------------------------------------
template <std::size_t Index /* = 0U */>
constexpr auto geo::OpDetID::getIndex() const
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
} // geo::OpDetID::getIndex() const

template <std::size_t Index /* = 0U */>
auto& geo::OpDetID::writeIndex()
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
} // geo::OpDetID::writeIndex()

template <std::size_t Above>
constexpr auto geo::OpDetID::getRelIndex() const
{
  static_assert(Above <= Level, "This ID type does not have the requested Index level.");
  return getIndex<Level - Above>();
} // geo::OpDetID::getRelIndex()

//------------------------------------------------------------------------------
template <std::size_t Index /* = 0U */>
constexpr auto geo::TPCID::getIndex() const
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
} // geo::TPCID::getIndex() const

template <std::size_t Index /* = 0U */>
auto& geo::TPCID::writeIndex()
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
} // geo::TPCID::writeIndex()

template <std::size_t Above>
constexpr auto geo::TPCID::getRelIndex() const
{
  static_assert(Above <= Level, "This ID type does not have the requested Index level.");
  return getIndex<Level - Above>();
} // geo::TPCID::getRelIndex()

//------------------------------------------------------------------------------
template <std::size_t Index /* = 0U */>
constexpr auto geo::PlaneID::getIndex() const
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
} // geo::PlaneID::getIndex() const

template <std::size_t Index /* = 0U */>
auto& geo::PlaneID::writeIndex()
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
} // geo::PlaneID::writeIndex()

template <std::size_t Above>
constexpr auto geo::PlaneID::getRelIndex() const
{
  static_assert(Above <= Level, "This ID type does not have the requested Index level.");
  return getIndex<Level - Above>();
} // geo::PlaneID::getRelIndex()

//------------------------------------------------------------------------------
template <std::size_t Index /* = 0U */>
constexpr auto geo::WireID::getIndex() const
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
} // geo::WireID::getIndex() const

template <std::size_t Index /* = 0U */>
auto& geo::WireID::writeIndex()
{
  static_assert(Index <= Level, "This ID type does not have the requested Index level.");
  return details::getAbsIDindex<Index>(*this);
} // geo::WireID::writeIndex()

template <std::size_t Above>
constexpr auto geo::WireID::getRelIndex() const
{
  static_assert(Above <= Level, "This ID type does not have the requested Index level.");
  return getIndex<Level - Above>();
} // geo::WireID::getRelIndex()

//------------------------------------------------------------------------------

#endif // LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_TYPES_H
