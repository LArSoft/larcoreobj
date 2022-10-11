/**
 * @file   larcoreobj/SimpleTypesAndConstants/readout_types_fhicl.h
 * @brief  Utilities for using readout IDs in FHiCL validated configuration.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   November 26, 2019
 *
 * This library is header-only.
 */

#ifndef LARCOREOBJ_SIMPLETYPESANDCONSTANTS_READOUT_TYPES_FHICL_H
#define LARCOREOBJ_SIMPLETYPESANDCONSTANTS_READOUT_TYPES_FHICL_H

// LArSoft libraries
#include "larcoreobj/SimpleTypesAndConstants/geo_types_fhicl.h"
#include "larcoreobj/SimpleTypesAndConstants/readout_types.h"

// support libraries
#include "fhiclcpp/types/Atom.h"

namespace geo::fhicl {

  /// Configuration structure for validated `readout::TPCsetID` parameter.
  template <>
  struct IDConfig<readout::TPCsetID> : public IDConfig<readout::CryostatID> {
    using ID_t = readout::TPCsetID; ///< Type read by this configuration.

    ::fhicl::Atom<readout::TPCsetID::TPCsetID_t> S{
      ::fhicl::Name("S"),
      ::fhicl::Comment("TPC set number within the cryostat"),
      [this]() { return valid(); }};

    ID_t ID() const { return !valid() ? ID_t{} : ID_t{IDConfig<readout::CryostatID>::ID(), S()}; }
    operator ID_t() const { return ID(); }
  }; // struct IDConfig<readout::TPCsetID>

  /// Configuration structure for validated `readout::ROPID` parameter.
  template <>
  struct IDConfig<readout::ROPID> : public IDConfig<readout::TPCsetID> {
    using ID_t = readout::ROPID; ///< Type read by this configuration.

    ::fhicl::Atom<readout::ROPID::ROPID_t> R{
      ::fhicl::Name("R"),
      ::fhicl::Comment("Readout plane number within the TPC set"),
      [this]() { return valid(); }};

    ID_t ID() const { return !valid() ? ID_t{} : ID_t{IDConfig<readout::TPCsetID>::ID(), R()}; }
    operator ID_t() const { return ID(); }
  }; // struct IDConfig<readout::ROPID>

} // namespace geo::fhicl

/// FHiCL objects representing geometry classes as configuration parameters.
namespace readout::fhicl {

  // --- BEGIN -- Importing from geo::fhicl namespace --------------------------

  //
  // types
  //
  using geo::fhicl::IDConfig;
  using geo::fhicl::IDof;
  using geo::fhicl::IDparameter;
  using geo::fhicl::IDparameterTraits;
  using geo::fhicl::IDsequence;
  using geo::fhicl::OptionalID;
  using geo::fhicl::OptionalIDsequence;

  //
  // utilities
  //
  using geo::fhicl::readID;
  using geo::fhicl::readIDsequence;
  using geo::fhicl::readOptionalID;
  using geo::fhicl::readOptionalIDsequence;
  using geo::fhicl::readParameter;

  // --- END -- Importing from geo::fhicl namespace ----------------------------

  // --- BEGIN -- Validated configuration parameters for readout ID objects ----
  /**
   * @name Validated configuration parameters for readout ID objects
   *
   * These data types can be used in a class for validated FHiCL configuration.
   * They are implemented as configuration tables (`fhicl::Table`) of a
   * configuration structure containing one parameter (`fhicl::Atom`) per index
   * in the ID. They do _not_ support default values, but optional parameters
   * may be used as a workaround.
   *
   * These objects have the same type of implementation and follow the same
   * usage patterns as the ones for geometry ID objects defined and documented
   * in `geo::fhicl` namespace.
   *
   * This namespace offers:
   * * a set of `readout` aliases, e.g. `readout::fhicl::CryostatID`, for
   *   cryostat IDs;
   * * a set of classes for TPC set parameters, characterized by a configuration
   *   like `{ C:0 S:1 }`;
   * * a set of classes for readout plane parameters, characterized by a
   *   configuration like `{ C:0 S:1 R:2 }`;
   * * _no facility for `raw::ChannelID_t`_, which is a regular type and can be
   *   directly read with the standard `fhicl` objects.
   *
   */
  /// @{

  // --- BEGIN -- Cryostat ID --------------------------------------------------
  //
  // because `readout::CryostatID` is an alias of `geo::CryostatID`,
  // the following types are also aliases of the corresponding `geo::fhicl` ones
  //

  /// Member type of validated `readout::CryostatID` parameter.
  using CryostatID = IDparameter<readout::CryostatID>;

  /// Member type of optional validated `readout::CryostatID` parameter.
  using OptionalCryostatID = OptionalID<readout::CryostatID>;

  /// Member type of sequence of `readout::CryostatID` parameters.
  using CryostatIDsequence = IDsequence<readout::CryostatID>;

  /// Member type of optional sequence of `readout::CryostatID` parameters.
  using OptionalCryostatIDsequence = OptionalIDsequence<readout::CryostatID>;

  // --- END -- Cryostat ID ----------------------------------------------------

  // --- BEGIN -- TPC set ID ---------------------------------------------------

  /// Member type of validated `readout::TPCsetID` parameter.
  using TPCsetID = IDparameter<readout::TPCsetID>;

  /// Member type of optional validated `readout::TPCsetID` parameter.
  using OptionalTPCsetID = OptionalID<readout::TPCsetID>;

  /// Member type of sequence of `readout::TPCsetID` parameters.
  using TPCsetIDsequence = IDsequence<readout::TPCsetID>;

  /// Member type of optional sequence of `readout::TPCsetID` parameters.
  using OptionalTPCsetIDsequence = OptionalIDsequence<readout::TPCsetID>;

  // --- END -- TPC set ID -----------------------------------------------------

  // --- BEGIN -- Readout plane ID ---------------------------------------------

  /// Member type of validated `readout::ROPID` parameter.
  using ROPID = IDparameter<readout::ROPID>;

  /// Member type of optional validated `readout::ROPID` parameter.
  using OptionalROPID = OptionalID<readout::ROPID>;

  /// Member type of sequence of `readout::ROPID` parameters.
  using ROPIDsequence = IDsequence<readout::ROPID>;

  /// Member type of optional sequence of `readout::ROPID` parameters.
  using OptionalROPIDsequence = OptionalIDsequence<readout::ROPID>;

  // --- END -- Readout plane ID -----------------------------------------------

  /// @}
  // --- END -- Validated configuration parameters for readout ID objects ------

} // namespace readout::fhicl

// -----------------------------------------------------------------------------

#endif // LARCOREOBJ_SIMPLETYPESANDCONSTANTS_READOUT_TYPES_FHICL_H
