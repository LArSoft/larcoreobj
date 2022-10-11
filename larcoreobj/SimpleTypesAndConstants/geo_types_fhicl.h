/**
 * @file   larcoreobj/SimpleTypesAndConstants/geo_types_fhicl.h
 * @brief  Utilities for using geometry IDs in FHiCL validated configuration.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   November 25, 2019
 *
 * This library is header-only.
 */

#ifndef LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_TYPES_FHICL_H
#define LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_TYPES_FHICL_H

// LArSoft libraries
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

// support libraries
#include "fhiclcpp/types/Atom.h"
#include "fhiclcpp/types/OptionalAtom.h"
#include "fhiclcpp/types/OptionalSequence.h"
#include "fhiclcpp/types/OptionalTable.h"
#include "fhiclcpp/types/Sequence.h"
#include "fhiclcpp/types/Table.h"

// C/C++ standard libraries
#include <cstddef> // std::size_t
#include <optional>
#include <vector>

/// FHiCL objects representing geometry classes as configuration parameters.
namespace geo::fhicl {

  // --- BEGIN -- Validated configuration parameters for geometry ID objects ---
  /**
   * @name Validated configuration parameters for geometry ID objects
   *
   * These data types can be used in a class for validated FHiCL configuration.
   * They are implemented as configuration tables (`fhicl::Table`) of a
   * configuration structure containing one parameter (`fhicl::Atom`) per index
   * in the ID. They do _not_ support default values, but optional parameters
   * may be used as a workaround.
   * Unfortunately, they do not exactly follow the usage pattern of the standard
   * FHiCL objects, and helper functions are provided to facilitate their use:
   * * simple IDs _do_ support the standard usage pattern, except that they do
   *   not support default values; it is nevertheless safer to use
   *   `geo::fhicl::readID()`;
   * * optional IDs are better read with `geo::fhicl::readOptionalID()`, which
   *   returns a `std::optional` object;
   * * ID sequences are better read with `geo::fhicl::readIDsequence()`, which
   *   returns a STL vector of IDs;
   * * optional ID sequences are better read with
   *   `geo::fhicl::readOptionalIDsequence()`, which returns a `std::optional`
   *   object containing a STL vector of IDs.
   *
   * In addition, a _unified interface_ is provided and recommended (see below).
   *
   * An ID described data member can be specified as a table with the same
   * syntax as the standard printout of the IDs, e.g. `{ C:1 T:3 P:2 }`
   * for the plane `C:1 T:3 P:2`.
   * Example:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct Config {
   *
   *   geo::fhicl::PlaneIDsequence Planes {
   *     fhicl::Name("Planes"),
   *     fhicl::Comment("anode planes to process")
   *     };
   *
   *   geo::fhicl::OptionalPlaneID ReferencePlane {
   *     fhicl::Name("ReferencePlane"),
   *     fhicl::Comment("reference anode plane (first one by default)")
   *     };
   *
   * }; // struct Config
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * which can be configured as:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * Planes: [
   *   { C:0 T:1 P:0 },
   *   { C:0 T:1 P:1 },
   *   { C:0 T:1 P:2 }
   *   ]
   * ReferencePlane: { C:0 T:1 P:2 }
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * and read as:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * void readParams(art::EDProducer::Table<Config> const& config) {
   *
   *   std::vector<geo::PlaneID> const planes
   *     = geo::fhicl::readIDsequence(config().Planes);
   *   if (planes.empty()) {
   *     throw art::Exception(art::errors::Configuration)
   *       << "At least one plane is needed.\n";
   *   }
   *
   *   geo::PlaneID const refPlane
   *     = geo::fhicl::readOptionalID(config().ReferencePlane)
   *       .value_or(planes.front())
   *     ;
   *
   * } // readParams()
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   *
   * As mentioned above, currently default values are not supported.
   * The workaround is to use the "optional" version of the objects.
   * Utilities are provided to read them: the function
   * `geo::fhicl::readOptionalID()` reading a single optional ID was illustrated
   * in the previous example, while for optional sequences of IDs the following
   * usage pattern is suppported via `geo::fhicl::readOptionalIDsequence()`:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct Config {
   *
   *   geo::fhicl::OptionalPlaneIDsequence Planes {
   *     fhicl::Name("Planes"),
   *     fhicl::Comment("anode planes to process (omit or empty processes all)")
   *     };
   *
   * }; // struct Config
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * reading as:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * void readParams(art::EDProducer::Table<Config> const& config) {
   *
   *   std::vector<geo::PlaneID> planes
   *     = geo::fhicl::readOptionalIDsequence(config().Planes, {});
   *
   * } // readParams()
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * Note however that the default value will not show in the regular
   * `lar --print-description` output.
   *
   *
   * Unified interface
   * ==================
   *
   * A "unified" interface is provided to read all the ID parameters with the
   * same syntax. Given a parameter (`configPar`) to read, the syntax is:
   * * `readParameter(configPar)` to read the parameter
   *     * if the parameter is optional, a `std::optional` object wrapping the
   *       parameter value (if any) is returned
   *     * if the parameter is a sequence, the value is stored in a
   *       `std::vector` object (otherwise just in an object with the proper ID
   *       type)
   * * `readParameter(configPar, defValue)` to read the parameter (which must
   *   be optional) and return directly the value of the parameter, if
   *   specified, or a default value otherwise.
   *
   * A (long) example follows, showing how to read the wire ID parameters of
   * this configuration:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct Config {
   *
   *   geo::fhicl::WireID Wire { fhicl::Name("Wire") };
   *
   *   geo::fhicl::OptionalWireID MaybeWire { fhicl::Name("MaybeWire") };
   *
   *   geo::fhicl::WireIDsequence Wires { fhicl::Name("Wires") };
   *
   *   geo::fhicl::OptionalWireIDsequence MaybeWires { fhicl::Name("MaybeWires") };
   *
   * }; // struct Config
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * Here is how the unified interface reads them (the optional ones are read
   * twice to show reading with and without default values):
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * void readParameters(fhicl::Table<Config> const& config) {
   *
   *   ID_t const defaultID { 1U, 5U, 0U, 9U };
   *
   *   std::vector<ID_t> const defaultIDs
   *     ({ ID_t{ 1U, 5U, 0U, 4U }, ID_t{ 1U, 5U, 0U, 3U } });
   *
   *   // id11 is `geo::WireID`:
   *   auto id11 = geo::fhicl::readParameter(config().Wire);
   *
   *   // id21 is `std::optional<geo::WireID>`
   *   auto id21 = geo::fhicl::readParameter(config().MaybeWire);
   *
   *   // id22 is `geo::WireID`
   *   auto id22 = geo::fhicl::readParameter(config().MaybeWire, defaultID);
   *
   *   // id31 is `std::vector<geo::WireID>`
   *   auto id31 = geo::fhicl::readParameter(config().Wires);
   *
   *   // id41 is `std::optional<std::vector<geo::WireID>>`
   *   auto id41 = geo::fhicl::readParameter(config().MaybeWires);
   *
   *   // id42 is `std::vector<geo::WireID>`
   *   auto id42 = geo::fhicl::readParameter(config().MaybeWires, defaultIDs);
   *
   * }
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   *
   */
  /// @{

  //@{
  /**
   * @brief Traits for ID parameter objects
   * @tparam IDparam the ID parameter object
   *
   * This set offers the following traits:
   * * `type`: type of parameter (same as `IDparam`)
   * * `ID_t`: type of ID read by the parameter
   * * `isOptional` (boolean): whether the parameter is optional
   * * `isAtom` (boolean): whether the parameter reads a single ID
   * * `isSequence` (boolean): whether the parameter reads a sequence of ID's
   *
   * The trait class is defined only for the supported parameter types.
   */
  template <typename IDparam>
  struct IDparameterTraits;
  //@}

  //@{
  /// Type of ID of the specified ID FHiCL parameter object/
  template <typename IDparam>
  using IDof = typename IDparameterTraits<IDparam>::ID_t;
  //@}

  // --- BEGIN -- Generic ID infrastructure ------------------------------------
  /// Helper class holding the ID validity flag.
  struct ValidIDConfig {

    ::fhicl::Atom<bool> isValid{::fhicl::Name("isValid"),
                                ::fhicl::Comment("whether the ID is valid"),
                                true};

    bool valid() const { return isValid(); }

  }; // struct ValidIDConfig

  /// Type of ID configuration structure (requires specialization)
  template <typename IDtype>
  struct IDConfig;

  /// Member type of validated ID parameter.
  template <typename IDtype>
  using IDparameter = ::fhicl::Table<IDConfig<IDtype>>;

  /// Member type of optional validated ID parameter.
  template <typename IDtype>
  using OptionalID = ::fhicl::OptionalTable<IDConfig<IDtype>>;

  /// Member type of sequence of ID parameters.
  template <typename IDtype>
  using IDsequence = ::fhicl::Sequence<IDparameter<IDtype>>;

  /// Member type of optional sequence of ID parameters.
  template <typename IDtype>
  using OptionalIDsequence = ::fhicl::OptionalSequence<IDparameter<IDtype>>;

  // --- END -- Generic ID infrastructure --------------------------------------

  // --- BEGIN -- Cryostat ID --------------------------------------------------
  /// Configuration structure for validated `geo::CryostatID` parameter.
  template <>
  struct IDConfig<geo::CryostatID> : public ValidIDConfig {
    using ID_t = geo::CryostatID; ///< Type read by this configuration.

    ::fhicl::Atom<geo::CryostatID::CryostatID_t> C{::fhicl::Name("C"),
                                                   ::fhicl::Comment("cryostat number"),
                                                   [this]() { return valid(); }};

    ID_t ID() const { return !valid() ? ID_t{} : ID_t{C()}; }
    operator ID_t() const { return ID(); }

  }; // struct IDConfig<geo::CryostatID>

  /// Member type of validated `geo::CryostatID` parameter.
  using CryostatID = IDparameter<geo::CryostatID>;

  /// Member type of optional validated `geo::CryostatID` parameter.
  using OptionalCryostatID = OptionalID<geo::CryostatID>;

  /// Member type of sequence of `geo::CryostatID` parameters.
  using CryostatIDsequence = IDsequence<geo::CryostatID>;

  /// Member type of optional sequence of `geo::CryostatID` parameters.
  using OptionalCryostatIDsequence = OptionalIDsequence<geo::CryostatID>;

  // --- END -- Cryostat ID ----------------------------------------------------

  // --- BEGIN -- TPC ID -------------------------------------------------------
  /// Configuration structure for validated `geo::TPCID` parameter.
  template <>
  struct IDConfig<geo::TPCID> : public IDConfig<geo::CryostatID> {
    using ID_t = geo::TPCID; ///< Type read by this configuration.

    ::fhicl::Atom<geo::TPCID::TPCID_t> T{::fhicl::Name("T"),
                                         ::fhicl::Comment("TPC number within the cryostat"),
                                         [this]() { return valid(); }};

    ID_t ID() const { return !valid() ? ID_t{} : ID_t{IDConfig<geo::CryostatID>::ID(), T()}; }
    operator ID_t() const { return ID(); }
  }; // struct IDConfig<geo::TPCID>

  /// Member type of validated `geo::TPCID` parameter.
  using TPCID = IDparameter<geo::TPCID>;

  /// Member type of optional validated `geo::TPCID` parameter.
  using OptionalTPCID = OptionalID<geo::TPCID>;

  /// Member type of sequence of `geo::TPCID` parameters.
  using TPCIDsequence = IDsequence<geo::TPCID>;

  /// Member type of optional sequence of `geo::TPCID` parameters.
  using OptionalTPCIDsequence = OptionalIDsequence<geo::TPCID>;

  // --- END -- TPC ID ---------------------------------------------------------

  // --- BEGIN -- Optical detector ID ------------------------------------------
  /// Configuration structure for validated `geo::OpDetID` parameter.
  template <>
  struct IDConfig<geo::OpDetID> : public IDConfig<geo::CryostatID> {
    using ID_t = geo::OpDetID; ///< Type read by this configuration.

    ::fhicl::Atom<geo::OpDetID::OpDetID_t> O{
      ::fhicl::Name("O"),
      ::fhicl::Comment("Optical detector number within the cryostat"),
      [this]() { return valid(); }};

    ID_t ID() const { return !valid() ? ID_t{} : ID_t{IDConfig<geo::CryostatID>::ID(), O()}; }
    operator ID_t() const { return ID(); }
  }; // struct IDConfig<geo::OpDetID>

  /// Member type of validated `geo::OpDetID` parameter.
  using OpDetID = IDparameter<geo::OpDetID>;

  /// Member type of optional validated `geo::OpDetID` parameter.
  using OptionalOpDetID = OptionalID<geo::OpDetID>;

  /// Member type of sequence of `geo::OpDetID` parameters.
  using OpDetIDsequence = IDsequence<geo::OpDetID>;

  /// Member type of optional sequence of `geo::OpDetID` parameters.
  using OptionalOpDetIDsequence = OptionalIDsequence<geo::OpDetID>;

  // --- END -- Optical detector ID --------------------------------------------

  // --- BEGIN -- Plane ID -----------------------------------------------------
  /// Configuration structure for validated `geo::PlaneID` parameter.
  template <>
  struct IDConfig<geo::PlaneID> : public IDConfig<geo::TPCID> {
    using ID_t = geo::PlaneID; ///< Type read by this configuration.

    ::fhicl::Atom<geo::PlaneID::PlaneID_t> P{::fhicl::Name("P"),
                                             ::fhicl::Comment("Plane number within the TPC"),
                                             [this]() { return valid(); }};

    ID_t ID() const { return !valid() ? ID_t{} : ID_t{IDConfig<geo::TPCID>::ID(), P()}; }
    operator ID_t() const { return ID(); }
  }; // struct IDConfig<geo::PlaneID>

  /// Member type of validated `geo::PlaneID` parameter.
  using PlaneID = IDparameter<geo::PlaneID>;

  /// Member type of optional validated `geo::PlaneID` parameter.
  using OptionalPlaneID = OptionalID<geo::PlaneID>;

  /// Member type of sequence of `geo::PlaneID` parameters.
  using PlaneIDsequence = IDsequence<geo::PlaneID>;

  /// Member type of optional sequence of `geo::PlaneID` parameters.
  using OptionalPlaneIDsequence = OptionalIDsequence<geo::PlaneID>;

  // --- END -- Plane ID -------------------------------------------------------

  // --- BEGIN -- Wire ID ------------------------------------------------------
  /// Configuration structure for validated `geo::PlaneID` parameter.
  template <>
  struct IDConfig<geo::WireID> : public IDConfig<geo::PlaneID> {
    using ID_t = geo::WireID; ///< Type read by this configuration.

    ::fhicl::Atom<geo::WireID::WireID_t> W{::fhicl::Name("W"),
                                           ::fhicl::Comment("Wire number within the plane"),
                                           [this]() { return valid(); }};

    ID_t ID() const { return !valid() ? ID_t{} : ID_t{IDConfig<geo::PlaneID>::ID(), W()}; }
    operator ID_t() const { return ID(); }
  }; // struct IDConfig<geo::WireID>

  /// Member type of validated `geo::WireID` parameter.
  using WireID = IDparameter<geo::WireID>;

  /// Member type of optional validated `geo::WireID` parameter.
  using OptionalWireID = OptionalID<geo::WireID>;

  /// Member type of sequence of `geo::WireID` parameters.
  using WireIDsequence = IDsequence<geo::WireID>;

  /// Member type of optional sequence of `geo::WireID` parameters.
  using OptionalWireIDsequence = OptionalIDsequence<geo::WireID>;

  // --- END -- Wire ID --------------------------------------------------------

  // --- BEGIN -- ID parsing ---------------------------------------------------

  //@{
  /**
   * @brief Returns an ID extracted from the specified ID atom.
   * @tparam SrcID type of the ID read by the FHiCL parameter
   * @tparam ID type of the returned ID (default: same as `SrcID`)
   * @param atom the ID atom parameter to convert
   * @return an `ID` object converted from `atom`
   *
   * This function is trivial and provided mostly for uniformity of interface.
   * In the standard usage is equivalent to using the `atom` directly:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct Config {
   *
   *   geo::fhicl::TPCID TPC { fhicl::Name("TPC"), fhicl::Comment("TPC") };
   *
   * };
   *
   * void newInterface(art::EDProducer::Table<Config> const& config) {
   *
   *   geo::TPCID const tpcid = geo::fhicl::readID(config().TPC);
   *
   *   // ...
   * }
   *
   * void classicInterface(art::EDProducer::Table<Config> const& config) {
   *
   *   geo::TPCID const tpcid = config().TPC();
   *
   *   // ...
   * }
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * has `newInterface()` and `classicInterface()` completely equivalent.
   * Note however that because of inplementation details, `config().TPC()`
   * (classic interface) _does not_ return a `geo::TPCID` object, but a
   * different, implementation dependent object that is converted upon
   * assignment. This implies that the following variables may have different
   * types, and the new interface is the one guaranteeing the right type:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * auto tpcidNew     = geo::fhicl::readID(config().TPC); // type is geo::TPCID
   * auto tpcidClassic = config().TPC();     // type is implementation dependent
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   *
   * @note The additional template parameter `ID` is provided as an added bonus
   *       to choose which type to convert the configuration parameters into,
   *       and it's not enforced to be a ID type at all.
   */
  template <typename SrcID, typename ID = SrcID>
  ID readID(IDparameter<SrcID> const& atom);

  template <typename SrcID, typename ID = SrcID>
  ID readParameter(IDparameter<SrcID> const& atom)
  {
    return readID<SrcID, ID>(atom);
  }
  //@}

  //@{
  /**
   * @brief Returns an ID extracted from the specified optional ID atom.
   * @tparam SrcID type of the ID read by the FHiCL parameter
   * @tparam ID type of the returned ID (default: same as `SrcID`)
   * @param atom the optional ID atom parameter to convert
   * @return an `ID` object converted from `atom`, or no value if omitted
   *
   * The return type of this function is of type `std::optional`.
   * This function returns the value of the specified FHiCL optional atom
   * (`fhicl::OptionalAtom`). If the parameter was omitted, no value is
   * returned.
   *
   * Example of usage: the configuration object `Config` and the data member to
   * store the configuration parameter value are defined in a class as:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct Config {
   *
   *   geo::fhicl::OptionalTPCID refTPC
   *     { fhicl::Name("refTPC"), fhicl::Comment("reference TPC") };
   *
   * };
   *
   * std::optional<geo::TPCID> fRefTPC;
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * The constructor of that class should have an entry in the initializer list
   * like:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   fRefTPC(geo::fhicl::readOptionalID(config().RefTPC))
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * (note that the argument is just `config().RefTPC`, not
   * `config().RefTPC()`).
   *
   * @note The additional template parameter `ID` is provided as an added bonus
   *       to choose which type to convert the configuration parameters into,
   *       and it's not enforced to be a ID type at all.
   */
  template <typename SrcID, typename ID = SrcID>
  std::optional<ID> readOptionalID(OptionalID<SrcID> const& atom);

  template <typename SrcID, typename ID = SrcID>
  std::optional<ID> readParameter(OptionalID<SrcID> const& atom)
  {
    return readOptionalID<SrcID, ID>(atom);
  }
  //@}

  //@{
  /**
   * @brief Returns an ID extracted from the specified optional ID atom.
   * @tparam SrcID type of the ID read by the FHiCL parameter
   * @tparam ID type of the returned ID (default: same as `SrcID`)
   * @param atom the optional ID atom parameter to convert
   * @param defValue a value to use if the parameter was omitted
   * @return an `ID` object converted from `atom`, or `defValue` if omitted
   *
   * This function returns the value of the specified FHiCL optional atom
   * (`fhicl::OptionalAtom`). If the parameter was omitted, the `defValue` is
   * copied or moved depending on the function.
   *
   * Example of usage: the configuration object `Config` and the data member to
   * store the configuration parameter value are defined in a class as:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct Config {
   *
   *   geo::fhicl::OptionalTPCID refTPC
   *     { fhicl::Name("refTPC"), fhicl::Comment("reference TPC") };
   *
   * };
   *
   * std::optional<geo::TPCID> fRefTPC;
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * The constructor of that class should have an entry in the initializer list
   * like:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   fRefTPC(geo::fhicl::readOptionalID(config().RefTPC, { 0U, 0U }))
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * will set `fRefTPC` to `C:0 T:0` TPC ID if `refTPC` was omitted from the
   * configuration (note that the argument is just `config().RefTPC`, not
   * `config().RefTPC()`).
   *
   * @note The additional template parameter `ID` is provided as an added bonus
   *       to choose which type to convert the configuration parameters into,
   *       and it's not enforced to be a ID type at all.
   */
  template <typename SrcID, typename ID = SrcID>
  ID readOptionalID(OptionalID<SrcID> const& atom, ID const& defValue);

  template <typename SrcID, typename ID = SrcID>
  ID readOptionalID(OptionalID<SrcID> const& atom, ID&& defValue);

  template <typename SrcID, typename ID = SrcID>
  ID readParameter(OptionalID<SrcID> const& atom, ID const& defValue)
  {
    return readOptionalID<SrcID, ID>(atom, defValue);
  }

  template <typename SrcID, typename ID = SrcID>
  ID readParameter(OptionalID<SrcID> const& atom, ID&& defValue)
  {
    return readOptionalID<SrcID, ID>(atom, std::move(defValue));
  }
  //@}

  //@{
  /**
   * @brief Returns a vector of IDs extracted from the specified ID sequence.
   * @tparam SrcID type of the ID read by the FHiCL parameter
   * @tparam ID type of the returned ID (default: same as `SrcID`)
   * @param seq the sequence of ID parameters to convert
   * @return a STL vector of `ID` objects converted from `seq` parameter values
   *
   * This function returns the value of the specified FHiCL sequence object
   * (`fhicl::Sequence`). It supports both fixed and variable size sequences,
   * but it always returns a STL vector as a result.
   *
   * Example of usage: the configuration object `Config` and the data member to
   * store the configuration parameter value are defined in a class as:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct Config {
   *
   *   geo::fhicl::TPCIDsequence TPCs
   *     { fhicl::Name("TPCs"), fhicl::Comment("selected TPCs") };
   *
   * };
   *
   * std::vector<geo::TPCID> fTPCs;
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * The constructor of that class should have an entry in the initializer list
   * like:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   fTPCs(geo::fhicl::readIDsequence(config().TPCs))
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * (note that the argument is just `config().TPCs`, not `config().TPCs()`).
   *
   * @note The additional template parameter `ID` is provided as an added bonus
   *       to choose which type to convert the configuration parameters into,
   *       and it's not enforced to be a ID type at all.
   */
  template <typename SrcID, typename ID = SrcID>
  std::vector<ID> readIDsequence(IDsequence<SrcID> const& seq);

  template <typename SrcID, typename ID = SrcID>
  std::vector<ID> readParameter(IDsequence<SrcID> const& seq)
  {
    return readIDsequence<SrcID, ID>(seq);
  }
  //@}

  //@{
  /**
   * @brief Returns a vector of IDs extracted from the specified optional ID
   *        sequence.
   * @tparam SrcID type of the ID read by the FHiCL parameter
   * @tparam ID type of the returned ID (default: same as `SrcID`)
   * @param seq the optional sequence of ID parameters to convert
   * @return an optional collection containing a STL vector of `ID` objects
   *         converted from `seq` parameter values, or no value if the parameter
   *         was omitted
   *
   * This function returns the value of the specified FHiCL optional sequence
   * object (`fhicl::OptionalSequence`). It supports both fixed and variable
   * size optional sequences, but it always returns an optional STL vector as a
   * result.
   *
   * Example of usage: the configuration object `Config` and the data member to
   * store the configuration parameter value are defined in a class as:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * struct Config {
   *
   *   geo::fhicl::OptionalTPCIDsequence TPCs
   *     { fhicl::Name("TPCs"), fhicl::Comment("selected TPCs") };
   *
   * };
   *
   * std::optional<std::vector<geo::TPCID>> fTPCs;
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * The constructor of that class should have an entry in the initializer list
   * like:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   fTPCs(geo::fhicl::readIDsequence(config().TPCs))
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * (note that the argument is just `config().TPCs`, not `config().TPCs()`).
   * If instead a "default value" needs to be provided, the data member is
   * simply:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   * std::vector<geo::TPCID> fTPCs;
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * and the value can be assigned via the standard `std::optional` interface:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   fTPCs(geo::fhicl::readIDsequence(config().TPCs).value_or(std::vector<geo::TPCID>{}))
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   * (in this case the default value is an empty collection of TPC IDs) or using
   * a different overload of `readOptionalIDsequence()`:
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.cpp}
   *   fTPCs(geo::fhicl::readIDsequence(config().TPCs, {}))
   * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   *
   *
   * @note The additional template parameter `ID` is provided as an added bonus
   *       to choose which type to convert the configuration parameters into,
   *       and it's not enforced to be a ID type at all.
   */
  template <typename SrcID, typename ID = SrcID>
  std::optional<std::vector<ID>> readOptionalIDsequence(OptionalIDsequence<SrcID> const& seq);

  template <typename SrcID, typename ID = SrcID>
  std::optional<std::vector<ID>> readParameter(OptionalIDsequence<SrcID> const& seq)
  {
    return readOptionalIDsequence<SrcID, ID>(seq);
  }
  //@}

  //@{
  /**
   * @brief Returns a vector of IDs extracted from the specified optional ID
   *        sequence, or a default value.
   * @tparam SrcID type of the ID read by the FHiCL parameter
   * @tparam ID type of the returned ID (default: same as `SrcID`)
   * @param seq the optional sequence of ID parameters to convert
   * @param defValue value to be returned if the optional parameter was omitted
   * @return a collection containing a STL vector of `ID` objects
   *         converted either from `seq` parameter values or from `defValue`
   *
   * This function is based on `readOptionalIDsequence(IDsequence const&)`.
   * The operating mode is the same, but if the value is not available from
   * the parameters, a copy of `defValue` is returned, or `defValue` content
   * is moved into the returned value.
   */
  template <typename SrcID, typename ID = SrcID>
  std::vector<ID> readOptionalIDsequence(OptionalIDsequence<SrcID> const& seq,
                                         std::vector<ID> const& defValue);

  template <typename SrcID, typename ID = SrcID>
  std::vector<ID> readOptionalIDsequence(OptionalIDsequence<SrcID> const& seq,
                                         std::vector<ID>&& defValue);

  template <typename SrcID, typename ID = SrcID>
  std::vector<ID> readParameter(OptionalIDsequence<SrcID> const& seq,
                                std::vector<ID> const& defValue)
  {
    return readOptionalIDsequence<SrcID, ID>(seq, defValue);
  }

  template <typename SrcID, typename ID = SrcID>
  std::vector<ID> readParameter(OptionalIDsequence<SrcID> const& seq, std::vector<ID>&& defValue)
  {
    return readOptionalIDsequence<SrcID, ID>(seq, std::move(defValue));
  }

  //@}

  // --- END -- ID parsing -----------------------------------------------------

  /// @}
  // --- END -- Validated configuration parameters for geometry ID objects -----

} // namespace geo::fhicl

// -----------------------------------------------------------------------------
// ---  template implementation
// -----------------------------------------------------------------------------
// --- definition of the traits
// ---
template <typename ID>
struct geo::fhicl::IDparameterTraits<geo::fhicl::IDparameter<ID>> {

  using type = IDparameter<ID>; ///< Type of parameter.

  using ID_t = typename type::ID_t; ///< Type of ID read.

  static constexpr bool isOptional = false; ///< Whether it is optional.

  static constexpr bool isAtom = true; ///< Whether it is a single value.

  static constexpr bool isSequence = false; ///< Whether it is a sequence.

}; // geo::fhicl::IDparameterTraits<geo::fhicl::IDparameter<ID>>

template <typename ID>
struct geo::fhicl::IDparameterTraits<geo::fhicl::OptionalID<ID>>
  : geo::fhicl::IDparameterTraits<geo::fhicl::IDparameter<ID>> {
  // override:
  static constexpr bool isOptional = true; ///< Whether it is optional.
}; // geo::fhicl::IDparameterTraits<geo::fhicl::OptionalID<ID>>

template <typename ID>
struct geo::fhicl::IDparameterTraits<geo::fhicl::IDsequence<ID>>
  : geo::fhicl::IDparameterTraits<geo::fhicl::IDparameter<ID>> {
  // override:
  static constexpr bool isAtom = false;    ///< Whether it is a single value.
  static constexpr bool isSequence = true; ///< Whether it is a sequence.
}; // geo::fhicl::IDparameterTraits<geo::fhicl::IDsequence<ID>>

template <typename ID>
struct geo::fhicl::IDparameterTraits<geo::fhicl::OptionalIDsequence<ID>>
  : geo::fhicl::IDparameterTraits<geo::fhicl::IDsequence<ID>> {
  // override:
  static constexpr bool isOptional = true; ///< Whether it is optional.
}; // geo::fhicl::IDparameterTraits<geo::fhicl::OptionalIDsequence<ID>>

// -----------------------------------------------------------------------------
template <typename SrcID, typename ID /* = SrcID */>
ID geo::fhicl::readID(IDparameter<SrcID> const& atom)
{
  return atom(); // conversions happen as needed (if possible)
} // geo::fhicl::readID()

// -----------------------------------------------------------------------------
template <typename SrcID, typename ID /* = SrcID */>
std::optional<ID> geo::fhicl::readOptionalID(OptionalID<SrcID> const& atom)
{
  using ID_t = ID;

  typename OptionalID<SrcID>::value_type cs;
  return atom(cs) ? std::make_optional<ID_t>(cs.ID()) : std::nullopt;
} // geo::fhicl::readOptionalID()

// -----------------------------------------------------------------------------
template <typename SrcID, typename ID /* = SrcID */>
ID geo::fhicl::readOptionalID(OptionalID<SrcID> const& atom, ID const& defValue)
{
  return readOptionalID(atom).value_or(defValue);
}

// -----------------------------------------------------------------------------
template <typename SrcID, typename ID /* = SrcID */>
ID geo::fhicl::readOptionalID(OptionalID<SrcID> const& atom, ID&& defValue)
{
  return readOptionalID(atom).value_or(std::move(defValue));
}

// -----------------------------------------------------------------------------
template <typename SrcID, typename ID /* = SrcID */>
std::vector<ID> geo::fhicl::readIDsequence(IDsequence<SrcID> const& seq)
{
  using ID_t = ID;

  std::vector<ID_t> IDs;
  std::size_t const n = seq.size();
  IDs.reserve(n);
  for (std::size_t i = 0; i < n; ++i)
    IDs.push_back(seq(i)); // seq(i) is TPCIDConfig
  return IDs;
} // geo::fhicl::readIDsequence()

// -----------------------------------------------------------------------------
template <typename SrcID, typename ID /* = SrcID */>
std::optional<std::vector<ID>> geo::fhicl::readOptionalIDsequence(
  OptionalIDsequence<SrcID> const& seq)
{
  using values_t = std::vector<ID>;

  typename OptionalIDsequence<SrcID>::value_type values;
  if (!seq(values)) return std::nullopt;

  values_t IDs;
  IDs.reserve(values.size());
  std::copy(values.begin(), values.end(), std::back_inserter(IDs));
  return {std::move(IDs)};

} // geo::fhicl::readOptionalIDsequence()

// -----------------------------------------------------------------------------
template <typename SrcID, typename ID /* = SrcID */>
std::vector<ID> geo::fhicl::readOptionalIDsequence(OptionalIDsequence<SrcID> const& seq,
                                                   std::vector<ID> const& defValue)
{
  // making sure `paramValue` is not a r-value; not sure whether it is necessary
  auto paramValue = readOptionalIDsequence(seq);
  return paramValue.value_or(defValue);
} // geo::fhicl::readOptionalIDsequence(std::vector const&)

// -----------------------------------------------------------------------------
template <typename SrcID, typename ID /* = SrcID */>
std::vector<ID> geo::fhicl::readOptionalIDsequence(OptionalIDsequence<SrcID> const& seq,
                                                   std::vector<ID>&& defValue)
{
  return readOptionalIDsequence(seq).value_or(std::move(defValue));
} // geo::fhicl::readOptionalIDsequence(std::vector const&)

// -----------------------------------------------------------------------------

#endif // LARCOREOBJ_SIMPLETYPESANDCONSTANTS_GEO_TYPES_FHICL_H
