/**
 * @file   geo_types_fhicl_test.cc
 * @brief  Test of `larcoreobj/SimpleTypesAndConstants/geo_types_fhicl.h`.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   November 25, 2019
 */

// Boost libraries
#define BOOST_TEST_MODULE ( geo_types_fhicl_test )
#include <cetlib/quiet_unit_test.hpp> // BOOST_AUTO_TEST_CASE()
#include <boost/test/test_tools.hpp> // BOOST_CHECK(), BOOST_CHECK_EQUAL()

// LArSoft libraries
#include "larcoreobj/SimpleTypesAndConstants/geo_types_fhicl.h"
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

// support libraries
#include "fhiclcpp/types/Table.h"
#include "fhiclcpp/ParameterSet.h"


// C/C++ standard libraries
#include <iostream>
#include <string>
#include <array>
#include <type_traits> // std::is_same_v<>


//------------------------------------------------------------------------------
template <typename Config>
fhicl::Table<Config> validateConfig(std::string const& configStr) {
  fhicl::ParameterSet pset;
  pset = fhicl::ParameterSet::make(configStr);
  fhicl::Table<Config> validatedConfig { fhicl::Name("validatedConfig") };

  std::cout << std::string(80, '-') << std::endl;
  std::cout << "===> FHiCL configuration:";
  if (configStr.empty()) std::cout << " <empty>";
  else                   std::cout << "\n" << configStr;
  std::cout << std::endl;
  validatedConfig.print_allowed_configuration
    (std::cout << "===> Expected configuration: ");
  std::cout << std::endl;

  validatedConfig.validate_ParameterSet(pset);
  return validatedConfig;
} // validateConfig()


// --- BEGIN -- Cryostat ID tests ----------------------------------------------

void test_CryostatID_normal() {

  using ID_t = geo::CryostatID;
  struct Config { geo::fhicl::CryostatID Cryo { fhicl::Name("Cryo") }; };

  std::string const configStr { "Cryo: { C:2 }" };
  ID_t const expectedID { 2U };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.Cryo);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_CHECK_EQUAL(validatedConfig.Cryo().ID(), expectedID);
    BOOST_CHECK_EQUAL(id, expectedID);
  }
} // test_CryostatID_normal()


void test_CryostatID_invalid() {

  using ID_t = geo::CryostatID;
  struct Config { geo::fhicl::CryostatID Cryo { fhicl::Name("Cryo") }; };

  std::string const configStr { "Cryo: { isValid:false }" };
  ID_t const expectedID {};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.Cryo);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_CHECK_EQUAL(validatedConfig.Cryo().ID(), expectedID);
    BOOST_CHECK_EQUAL(id, expectedID);
  }
} // test_CryostatID_invalid()


void test_OptionalCryostatID_present() {

  using ID_t = geo::CryostatID;
  struct Config { geo::fhicl::OptionalCryostatID Cryo { fhicl::Name("Cryo") }; };

  std::string const configStr { "Cryo: { C:1 }" };
  auto const expectedID = std::make_optional<ID_t>(1U);

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id = readOptionalID(validatedConfig.Cryo);
  BOOST_CHECK_EQUAL(id.has_value(), expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_CHECK_EQUAL(id->isValid, expectedID->isValid);
    if (expectedID->isValid) BOOST_CHECK_EQUAL(id.value(), expectedID.value());
  }

} // test_OptionalCryostatID_present()


void test_OptionalCryostatID_omitted() {

  using ID_t = geo::CryostatID;
  struct Config { geo::fhicl::OptionalCryostatID Cryo { fhicl::Name("Cryo") }; };

  std::string const configStr { "" };

  std::optional<ID_t> const expectedID;

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id
    = geo::fhicl::readOptionalID(validatedConfig.Cryo);
  BOOST_CHECK_EQUAL(id.has_value(), expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_CHECK_EQUAL(id->isValid, expectedID->isValid);
    if (expectedID->isValid) BOOST_CHECK_EQUAL(id.value(), expectedID.value());
  }

} // test_OptionalCryostatID_omitted()


void test_CryostatIDsequence_normal() {

  using ID_t = geo::CryostatID;
  struct Config
    { geo::fhicl::CryostatIDsequence Cryos { fhicl::Name("Cryos") }; };

  std::string const configStr { "Cryos: [ { C:0 }, { C:2 } ]" };
  // BUG the double brace syntax is required to work around clang bug 21629
//  std::array<ID_t, 2U> const expectedIDs { ID_t{ 0U }, ID_t{ 2U } };
  std::array<ID_t, 2U> const expectedIDs {{ ID_t{ 0U }, ID_t{ 2U } }};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.Cryos);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_CHECK_EQUAL(ids.size(), expectedIDs.size());
  auto const n = std::min(ids.size(), expectedIDs.size());
  for (std::size_t i = 0; i < n; ++i) {
    auto const& id = ids[i];
    ID_t const& expectedID = expectedIDs[i];

    BOOST_TEST_CONTEXT("Item [" << i << "]") {
      BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
      if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
    } // BOOST_TEST_CONTEXT
  } // for

} // test_CryostatIDsequence_normal()


void test_CryostatIDsequence_empty() {

  using ID_t = geo::CryostatID;
  struct Config
    { geo::fhicl::CryostatIDsequence Cryos { fhicl::Name("Cryos") }; };

  std::string const configStr { "Cryos: []" };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.Cryos);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_CHECK_EQUAL(ids.size(), 0U);

} // test_CryostatIDsequence_empty()


void test_OptionalCryostatIDsequence_normal() {

  using ID_t = geo::CryostatID;
  struct Config
    { geo::fhicl::OptionalCryostatIDsequence Cryos { fhicl::Name("Cryos") }; };

  std::string const configStr { "Cryos: [ { C:0 }, { C:2 } ]" };
  std::optional<std::vector<ID_t>> const expectedIDs
    (std::in_place, { ID_t{ 0U }, ID_t{ 2U } });

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.Cryos);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_CHECK_EQUAL(ids.has_value(), expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_CHECK_EQUAL(ids->size(), expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
        if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalCryostatIDsequence_normal()


void test_OptionalCryostatIDsequence_empty() {

  using ID_t = geo::CryostatID;
  struct Config
    { geo::fhicl::OptionalCryostatIDsequence Cryos { fhicl::Name("Cryos") }; };

  std::string const configStr { "Cryos: []" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::in_place);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.Cryos);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_CHECK_EQUAL(ids.has_value(), expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_CHECK_EQUAL(ids->size(), expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
        if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalCryostatIDsequence_empty()


void test_OptionalCryostatIDsequence_omitted() {

  using ID_t = geo::CryostatID;
  struct Config
    { geo::fhicl::OptionalCryostatIDsequence Cryos { fhicl::Name("Cryos") }; };

  std::string const configStr { "" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::nullopt);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.Cryos);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_CHECK_EQUAL(ids.has_value(), expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_CHECK_EQUAL(ids->size(), expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
        if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalCryostatIDsequence_omitted()


// --- END -- Cryostat ID tests ------------------------------------------------



// --- BEGIN -- Optical detector tests -----------------------------------------
void test_OpDetID_normal() {

  using ID_t = geo::OpDetID;
  struct Config { geo::fhicl::OpDetID OpDet { fhicl::Name("OpDet") }; };

  std::string const configStr { "OpDet: { C:2 O:3 }" };
  ID_t const expectedID { 2U, 3U };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.OpDet);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_CHECK_EQUAL(validatedConfig.OpDet().ID(), expectedID);
    BOOST_CHECK_EQUAL(id, expectedID);
  }
} // test_OpDetID_normal()


void test_OpDetID_invalid() {

  using ID_t = geo::OpDetID;
  struct Config { geo::fhicl::OpDetID OpDet { fhicl::Name("OpDet") }; };

  std::string const configStr { "OpDet: { isValid:false }" };
  ID_t const expectedID {};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.OpDet);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_CHECK_EQUAL(validatedConfig.OpDet().ID(), expectedID);
    BOOST_CHECK_EQUAL(id, expectedID);
  }
} // test_OpDetID_invalid()


void test_OptionalOpDetID_present() {

  using ID_t = geo::OpDetID;
  struct Config { geo::fhicl::OptionalOpDetID OpDet { fhicl::Name("OpDet") }; };

  std::string const configStr { "OpDet: { C:1 O:2 }" };
  std::optional<ID_t> const expectedID { std::in_place, 1U, 2U };

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id = readOptionalID(validatedConfig.OpDet);
  BOOST_CHECK_EQUAL(id.has_value(), expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_CHECK_EQUAL(id->isValid, expectedID->isValid);
    if (expectedID->isValid) BOOST_CHECK_EQUAL(id.value(), expectedID.value());
  }

} // test_OptionalOpDetID_present()


void test_OptionalOpDetID_omitted() {

  using ID_t = geo::OpDetID;
  struct Config { geo::fhicl::OptionalOpDetID OpDet { fhicl::Name("OpDet") }; };

  std::string const configStr { "" };

  std::optional<ID_t> const expectedID { std::nullopt };

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id
    = geo::fhicl::readOptionalID(validatedConfig.OpDet);
  BOOST_CHECK_EQUAL(id.has_value(), expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_CHECK_EQUAL(id->isValid, expectedID->isValid);
    if (expectedID->isValid) BOOST_CHECK_EQUAL(id.value(), expectedID.value());
  }

} // test_OptionalOpDetID_omitted()


void test_OpDetIDsequence_normal() {

  using ID_t = geo::OpDetID;
  struct Config
    { geo::fhicl::OpDetIDsequence OpDets { fhicl::Name("OpDets") }; };

  std::string const configStr { "OpDets: [ { C:0 O:1 }, { C:2 O:3 } ]" };
  // BUG the double brace syntax is required to work around clang bug 21629
//  std::array<ID_t, 2U> const expectedIDs { ID_t{ 0U, 1U }, ID_t{ 2U, 3U } };
  std::array<ID_t, 2U> const expectedIDs {{ ID_t{ 0U, 1U }, ID_t{ 2U, 3U } }};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.OpDets);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_CHECK_EQUAL(ids.size(), expectedIDs.size());
  auto const n = std::min(ids.size(), expectedIDs.size());
  for (std::size_t i = 0; i < n; ++i) {
    auto const& id = ids[i];
    ID_t const& expectedID = expectedIDs[i];

    BOOST_TEST_CONTEXT("Item [" << i << "]") {
      BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
      if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
    } // BOOST_TEST_CONTEXT
  } // for

} // test_OpDetIDsequence_normal()


void test_OpDetIDsequence_empty() {

  using ID_t = geo::OpDetID;
  struct Config
    { geo::fhicl::OpDetIDsequence OpDets { fhicl::Name("OpDets") }; };

  std::string const configStr { "OpDets: []" };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.OpDets);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_CHECK_EQUAL(ids.size(), 0U);

} // test_OpDetIDsequence_empty()


void test_OptionalOpDetIDsequence_normal() {

  using ID_t = geo::OpDetID;
  struct Config
    { geo::fhicl::OptionalOpDetIDsequence OpDets { fhicl::Name("OpDets") }; };

  std::string const configStr { "OpDets: [ { C:0 O:1 }, { C:2 O:3 } ]" };
  std::optional<std::vector<ID_t>> const expectedIDs
    (std::in_place, { ID_t{ 0U, 1U }, ID_t{ 2U, 3U } });

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.OpDets);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_CHECK_EQUAL(ids.has_value(), expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_CHECK_EQUAL(ids->size(), expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
        if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalOpDetIDsequence_normal()


void test_OptionalOpDetIDsequence_empty() {

  using ID_t = geo::OpDetID;
  struct Config
    { geo::fhicl::OptionalOpDetIDsequence OpDets { fhicl::Name("OpDets") }; };

  std::string const configStr { "OpDets: []" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::in_place);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.OpDets);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_CHECK_EQUAL(ids.has_value(), expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_CHECK_EQUAL(ids->size(), expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
        if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalOpDetIDsequence_empty()


void test_OptionalOpDetIDsequence_omitted() {

  using ID_t = geo::OpDetID;
  struct Config
    { geo::fhicl::OptionalOpDetIDsequence OpDets { fhicl::Name("OpDets") }; };

  std::string const configStr { "" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::nullopt);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.OpDets);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_CHECK_EQUAL(ids.has_value(), expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_CHECK_EQUAL(ids->size(), expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
        if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalOpDetIDsequence_omitted()

// --- END -- Optical detector ID tests ----------------------------------------



// --- BEGIN -- TPC ID tests ---------------------------------------------------
void test_TPCID_normal() {

  using ID_t = geo::TPCID;
  struct Config { geo::fhicl::TPCID TPC { fhicl::Name("TPC") }; };

  std::string const configStr { "TPC: { C:2 T:3 }" };
  ID_t const expectedID { 2U, 3U };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.TPC);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_CHECK_EQUAL(validatedConfig.TPC().ID(), expectedID);
    BOOST_CHECK_EQUAL(id, expectedID);
  }
} // test_TPCID_normal()


void test_TPCID_invalid() {

  using ID_t = geo::TPCID;
  struct Config { geo::fhicl::TPCID TPC { fhicl::Name("TPC") }; };

  std::string const configStr { "TPC: { isValid:false }" };
  ID_t const expectedID {};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.TPC);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_CHECK_EQUAL(validatedConfig.TPC().ID(), expectedID);
    BOOST_CHECK_EQUAL(id, expectedID);
  }
} // test_TPCID_invalid()


void test_OptionalTPCID_present() {

  using ID_t = geo::TPCID;
  struct Config { geo::fhicl::OptionalTPCID TPC { fhicl::Name("TPC") }; };

  std::string const configStr { "TPC: { C:1 T:2 }" };
  std::optional<ID_t> const expectedID { std::in_place, 1U, 2U };

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id = readOptionalID(validatedConfig.TPC);
  BOOST_CHECK_EQUAL(id.has_value(), expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_CHECK_EQUAL(id->isValid, expectedID->isValid);
    if (expectedID->isValid) BOOST_CHECK_EQUAL(id.value(), expectedID.value());
  }

} // test_OptionalTPCID_present()


void test_OptionalTPCID_omitted() {

  using ID_t = geo::TPCID;
  struct Config { geo::fhicl::OptionalTPCID TPC { fhicl::Name("TPC") }; };

  std::string const configStr { "" };

  std::optional<ID_t> const expectedID { std::nullopt };

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id
    = geo::fhicl::readOptionalID(validatedConfig.TPC);
  BOOST_CHECK_EQUAL(id.has_value(), expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_CHECK_EQUAL(id->isValid, expectedID->isValid);
    if (expectedID->isValid) BOOST_CHECK_EQUAL(id.value(), expectedID.value());
  }

} // test_OptionalTPCID_omitted()


void test_TPCIDsequence_normal() {

  using ID_t = geo::TPCID;
  struct Config
    { geo::fhicl::TPCIDsequence TPCs { fhicl::Name("TPCs") }; };

  std::string const configStr { "TPCs: [ { C:0 T:1 }, { C:2 T:3 } ]" };
  // BUG the double brace syntax is required to work around clang bug 21629
//  std::array<ID_t, 2U> const expectedIDs { ID_t{ 0U, 1U }, ID_t{ 2U, 3U } };
  std::array<ID_t, 2U> const expectedIDs {{ ID_t{ 0U, 1U }, ID_t{ 2U, 3U } }};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.TPCs);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_CHECK_EQUAL(ids.size(), expectedIDs.size());
  auto const n = std::min(ids.size(), expectedIDs.size());
  for (std::size_t i = 0; i < n; ++i) {
    auto const& id = ids[i];
    ID_t const& expectedID = expectedIDs[i];

    BOOST_TEST_CONTEXT("Item [" << i << "]") {
      BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
      if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
    } // BOOST_TEST_CONTEXT
  } // for

} // test_TPCIDsequence_normal()


void test_TPCIDsequence_empty() {

  using ID_t = geo::TPCID;
  struct Config
    { geo::fhicl::TPCIDsequence TPCs { fhicl::Name("TPCs") }; };

  std::string const configStr { "TPCs: []" };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.TPCs);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_CHECK_EQUAL(ids.size(), 0U);

} // test_TPCIDsequence_empty()


void test_OptionalTPCIDsequence_normal() {

  using ID_t = geo::TPCID;
  struct Config
    { geo::fhicl::OptionalTPCIDsequence TPCs { fhicl::Name("TPCs") }; };

  std::string const configStr { "TPCs: [ { C:0 T:1 }, { C:2 T:3 } ]" };
  std::optional<std::vector<ID_t>> const expectedIDs
    (std::in_place, { ID_t{ 0U, 1U }, ID_t{ 2U, 3U } });

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.TPCs);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_CHECK_EQUAL(ids.has_value(), expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_CHECK_EQUAL(ids->size(), expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
        if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalTPCIDsequence_normal()


void test_OptionalTPCIDsequence_empty() {

  using ID_t = geo::TPCID;
  struct Config
    { geo::fhicl::OptionalTPCIDsequence TPCs { fhicl::Name("TPCs") }; };

  std::string const configStr { "TPCs: []" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::in_place);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.TPCs);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_CHECK_EQUAL(ids.has_value(), expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_CHECK_EQUAL(ids->size(), expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
        if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalTPCIDsequence_empty()


void test_OptionalTPCIDsequence_omitted() {

  using ID_t = geo::TPCID;
  struct Config
    { geo::fhicl::OptionalTPCIDsequence TPCs { fhicl::Name("TPCs") }; };

  std::string const configStr { "" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::nullopt);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.TPCs);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_CHECK_EQUAL(ids.has_value(), expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_CHECK_EQUAL(ids->size(), expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
        if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalTPCIDsequence_omitted()

// --- END -- TPC ID tests -----------------------------------------------------



// --- BEGIN -- Plane ID tests -------------------------------------------------
void test_PlaneID_normal() {

  using ID_t = geo::PlaneID;
  struct Config { geo::fhicl::PlaneID Plane { fhicl::Name("Plane") }; };

  std::string const configStr { "Plane: { C:2 T:3 P:1 }" };
  ID_t const expectedID { 2U, 3U, 1U };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.Plane);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_CHECK_EQUAL(validatedConfig.Plane().ID(), expectedID);
    BOOST_CHECK_EQUAL(id, expectedID);
  }
} // test_PlaneID_normal()


void test_PlaneID_invalid() {

  using ID_t = geo::PlaneID;
  struct Config { geo::fhicl::PlaneID Plane { fhicl::Name("Plane") }; };

  std::string const configStr { "Plane: { isValid:false }" };
  ID_t const expectedID {};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.Plane);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_CHECK_EQUAL(validatedConfig.Plane().ID(), expectedID);
    BOOST_CHECK_EQUAL(id, expectedID);
  }
} // test_PlaneID_invalid()


void test_OptionalPlaneID_present() {

  using ID_t = geo::PlaneID;
  struct Config { geo::fhicl::OptionalPlaneID Plane { fhicl::Name("Plane") }; };

  std::string const configStr { "Plane: { C:1 T:2 P:1 }" };
  std::optional<ID_t> const expectedID { std::in_place, 1U, 2U, 1U };

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id = readOptionalID(validatedConfig.Plane);
  BOOST_CHECK_EQUAL(id.has_value(), expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_CHECK_EQUAL(id->isValid, expectedID->isValid);
    if (expectedID->isValid) BOOST_CHECK_EQUAL(id.value(), expectedID.value());
  }

} // test_OptionalPlaneID_present()


void test_OptionalPlaneID_omitted() {

  using ID_t = geo::PlaneID;
  struct Config { geo::fhicl::OptionalPlaneID Plane { fhicl::Name("Plane") }; };

  std::string const configStr { "" };

  std::optional<ID_t> const expectedID { std::nullopt };

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id
    = geo::fhicl::readOptionalID(validatedConfig.Plane);
  BOOST_CHECK_EQUAL(id.has_value(), expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_CHECK_EQUAL(id->isValid, expectedID->isValid);
    if (expectedID->isValid) BOOST_CHECK_EQUAL(id.value(), expectedID.value());
  }

} // test_OptionalPlaneID_omitted()


void test_PlaneIDsequence_normal() {

  using ID_t = geo::PlaneID;
  struct Config
    { geo::fhicl::PlaneIDsequence Planes { fhicl::Name("Planes") }; };

  std::string const configStr
    { "Planes: [ { C:0 T:1 P:1 }, { C:2 T:3 P:0 } ]" };
  std::array<ID_t, 2U> const expectedIDs
  // BUG the double brace syntax is required to work around clang bug 21629
//    { ID_t{ 0U, 1U, 1U }, ID_t{ 2U, 3U, 0U } };
    {{ ID_t{ 0U, 1U, 1U }, ID_t{ 2U, 3U, 0U } }};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.Planes);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_CHECK_EQUAL(ids.size(), expectedIDs.size());
  auto const n = std::min(ids.size(), expectedIDs.size());
  for (std::size_t i = 0; i < n; ++i) {
    auto const& id = ids[i];
    ID_t const& expectedID = expectedIDs[i];

    BOOST_TEST_CONTEXT("Item [" << i << "]") {
      BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
      if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
    } // BOOST_TEST_CONTEXT
  } // for

} // test_PlaneIDsequence_normal()


void test_PlaneIDsequence_empty() {

  using ID_t = geo::PlaneID;
  struct Config
    { geo::fhicl::PlaneIDsequence Planes { fhicl::Name("Planes") }; };

  std::string const configStr { "Planes: []" };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.Planes);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_CHECK_EQUAL(ids.size(), 0U);

} // test_PlaneIDsequence_empty()


void test_OptionalPlaneIDsequence_normal() {

  using ID_t = geo::PlaneID;
  struct Config
    { geo::fhicl::OptionalPlaneIDsequence Planes { fhicl::Name("Planes") }; };

  std::string const configStr
    { "Planes: [ { C:0 T:1 P:1 }, { C:2 T:3 P:0 } ]" };
  std::optional<std::vector<ID_t>> const expectedIDs
    (std::in_place, { ID_t{ 0U, 1U, 1U }, ID_t{ 2U, 3U, 0U } });

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.Planes);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_CHECK_EQUAL(ids.has_value(), expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_CHECK_EQUAL(ids->size(), expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
        if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalPlaneIDsequence_normal()


void test_OptionalPlaneIDsequence_empty() {

  using ID_t = geo::PlaneID;
  struct Config
    { geo::fhicl::OptionalPlaneIDsequence Planes { fhicl::Name("Planes") }; };

  std::string const configStr { "Planes: []" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::in_place);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.Planes);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_CHECK_EQUAL(ids.has_value(), expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_CHECK_EQUAL(ids->size(), expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
        if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalPlaneIDsequence_empty()


void test_OptionalPlaneIDsequence_omitted() {

  using ID_t = geo::PlaneID;
  struct Config
    { geo::fhicl::OptionalPlaneIDsequence Planes { fhicl::Name("Planes") }; };

  std::string const configStr { "" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::nullopt);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.Planes);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_CHECK_EQUAL(ids.has_value(), expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_CHECK_EQUAL(ids->size(), expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
        if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalPlaneIDsequence_omitted()

// --- END -- Plane ID tests ---------------------------------------------------



// --- BEGIN -- Wire ID tests -------------------------------------------------
void test_WireID_normal() {

  using ID_t = geo::WireID;
  struct Config { geo::fhicl::WireID Wire { fhicl::Name("Wire") }; };

  std::string const configStr { "Wire: { C:2 T:3 P:1 W:9 }" };
  ID_t const expectedID { 2U, 3U, 1U, 9U };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.Wire);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_CHECK_EQUAL(validatedConfig.Wire().ID(), expectedID);
    BOOST_CHECK_EQUAL(id, expectedID);
  }
} // test_WireID_normal()


void test_WireID_invalid() {

  using ID_t = geo::WireID;
  struct Config { geo::fhicl::WireID Wire { fhicl::Name("Wire") }; };

  std::string const configStr { "Wire: { isValid:false }" };
  ID_t const expectedID {};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.Wire);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_CHECK_EQUAL(validatedConfig.Wire().ID(), expectedID);
    BOOST_CHECK_EQUAL(id, expectedID);
  }
} // test_WireID_invalid()


void test_OptionalWireID_present() {

  using ID_t = geo::WireID;
  struct Config { geo::fhicl::OptionalWireID Wire { fhicl::Name("Wire") }; };

  std::string const configStr { "Wire: { C:1 T:2 P:1 W:9 }" };
  std::optional<ID_t> const expectedID { std::in_place, 1U, 2U, 1U, 9U };

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id = readOptionalID(validatedConfig.Wire);
  BOOST_CHECK_EQUAL(id.has_value(), expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_CHECK_EQUAL(id->isValid, expectedID->isValid);
    if (expectedID->isValid) BOOST_CHECK_EQUAL(id.value(), expectedID.value());
  }

} // test_OptionalWireID_present()


void test_OptionalWireID_omitted() {

  using ID_t = geo::WireID;
  struct Config { geo::fhicl::OptionalWireID Wire { fhicl::Name("Wire") }; };

  std::string const configStr { "" };

  std::optional<ID_t> const expectedID { std::nullopt };

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id
    = geo::fhicl::readOptionalID(validatedConfig.Wire);
  BOOST_CHECK_EQUAL(id.has_value(), expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_CHECK_EQUAL(id->isValid, expectedID->isValid);
    if (expectedID->isValid) BOOST_CHECK_EQUAL(id.value(), expectedID.value());
  }

} // test_OptionalWireID_omitted()


void test_WireIDsequence_normal() {

  using ID_t = geo::WireID;
  struct Config
    { geo::fhicl::WireIDsequence Wires { fhicl::Name("Wires") }; };

  std::string const configStr
    { "Wires: [ { C:0 T:1 P:1 W:9 }, { C:2 T:3 P:0 W:7 } ]" };
  std::array<ID_t, 2U> const expectedIDs
  // BUG the double brace syntax is required to work around clang bug 21629
//    { ID_t{ 0U, 1U, 1U, 9U }, ID_t{ 2U, 3U, 0U, 7U } };
    {{ ID_t{ 0U, 1U, 1U, 9U }, ID_t{ 2U, 3U, 0U, 7U } }};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.Wires);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_CHECK_EQUAL(ids.size(), expectedIDs.size());
  auto const n = std::min(ids.size(), expectedIDs.size());
  for (std::size_t i = 0; i < n; ++i) {
    auto const& id = ids[i];
    ID_t const& expectedID = expectedIDs[i];

    BOOST_TEST_CONTEXT("Item [" << i << "]") {
      BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
      if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
    } // BOOST_TEST_CONTEXT
  } // for

} // test_WireIDsequence_normal()


void test_WireIDsequence_empty() {

  using ID_t = geo::WireID;
  struct Config
    { geo::fhicl::WireIDsequence Wires { fhicl::Name("Wires") }; };

  std::string const configStr { "Wires: []" };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.Wires);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_CHECK_EQUAL(ids.size(), 0U);

} // test_WireIDsequence_empty()


void test_OptionalWireIDsequence_normal() {

  using ID_t = geo::WireID;
  struct Config
    { geo::fhicl::OptionalWireIDsequence Wires { fhicl::Name("Wires") }; };

  std::string const configStr
    { "Wires: [ { C:0 T:1 P:1 W:9 }, { C:2 T:3 P:0 W:7 } ]" };
  std::optional<std::vector<ID_t>> const expectedIDs
    (std::in_place, { ID_t{ 0U, 1U, 1U, 9U }, ID_t{ 2U, 3U, 0U, 7U } });

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.Wires);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_CHECK_EQUAL(ids.has_value(), expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_CHECK_EQUAL(ids->size(), expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
        if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalWireIDsequence_normal()


void test_OptionalWireIDsequence_empty() {

  using ID_t = geo::WireID;
  struct Config
    { geo::fhicl::OptionalWireIDsequence Wires { fhicl::Name("Wires") }; };

  std::string const configStr { "Wires: []" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::in_place);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.Wires);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_CHECK_EQUAL(ids.has_value(), expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_CHECK_EQUAL(ids->size(), expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
        if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalWireIDsequence_empty()


void test_OptionalWireIDsequence_omitted() {

  using ID_t = geo::WireID;
  struct Config
    { geo::fhicl::OptionalWireIDsequence Wires { fhicl::Name("Wires") }; };

  std::string const configStr { "" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::nullopt);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.Wires);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_CHECK_EQUAL(ids.has_value(), expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_CHECK_EQUAL(ids->size(), expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_CHECK_EQUAL(id.isValid, expectedID.isValid);
        if (expectedID.isValid) BOOST_CHECK_EQUAL(id, expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalWireIDsequence_omitted()


void test_WireUnifiedInterface() {

  using ID_t = geo::WireID;
  struct Config {

    geo::fhicl::WireID Wire { fhicl::Name("Wire") };

    geo::fhicl::OptionalWireID MaybeWire { fhicl::Name("MaybeWire") };

    geo::fhicl::OptionalWireID NoWire { fhicl::Name("NoWire") };

    geo::fhicl::WireIDsequence Wires { fhicl::Name("Wires") };

    geo::fhicl::OptionalWireIDsequence MaybeWires { fhicl::Name("MaybeWires") };

    geo::fhicl::OptionalWireIDsequence NoWires { fhicl::Name("NoWires") };

  }; // struct Config

  std::string const configStr {
      "Wire:       { C:1 T:5 P:2 W:9 }"
    "\nMaybeWire:  { C:1 T:5 P:2 W:8 }"
    "\n# NoWire:     @nil"
    "\n"
    "\nWires:      [ { C:1 T:5 P:2 W:7 }, { C:1 T:5 P:2 W:6 } ] "
    "\nMaybeWires: [ { C:1 T:5 P:2 W:5 } ] "
    "\n# NoWires:    @nil"
    "\n"
    };

  auto const& config = validateConfig<Config>(configStr);

  ID_t const expected1                { 1U, 5U, 2U, 9U };
  ID_t const expected2                { 1U, 5U, 2U, 8U };
  std::array<ID_t, 2U> const expected4
  // BUG the double brace syntax is required to work around clang bug 21629
//    { ID_t{ 1U, 5U, 2U, 7U }, ID_t{ 1U, 5U, 2U, 6U } };
    {{ ID_t{ 1U, 5U, 2U, 7U }, ID_t{ 1U, 5U, 2U, 6U } }};
  std::array<ID_t, 1U> const expected5
  // BUG the double brace syntax is required to work around clang bug 21629
//     { ID_t{ 1U, 5U, 2U, 5U } };
    {{ ID_t{ 1U, 5U, 2U, 5U } }};

  ID_t const default3 { 1U, 5U, 0U, 9U };
  auto default3value = default3;

  std::vector<ID_t> const default6({
    ID_t{ 1U, 5U, 0U, 4U }, ID_t{ 1U, 5U, 0U, 3U }, ID_t{ 1U, 5U, 0U, 2U }
    });
  auto default6value = default6;

  //
  // read simple atom
  //
  auto id11 = geo::fhicl::readParameter(config().Wire);
  static_assert(std::is_same_v<decltype(id11), ID_t>);
  BOOST_CHECK_EQUAL(id11, expected1);

  //
  // read optional atom
  //

  // this one is present (default values should be ignored):
  auto id21 = geo::fhicl::readParameter(config().MaybeWire);
  static_assert(std::is_same_v<decltype(id21), std::optional<ID_t>>);
  BOOST_CHECK(id21.has_value());
  if (id21.has_value()) BOOST_CHECK_EQUAL(id21.value(), expected2);

  default3value = default3;
  auto id22 = geo::fhicl::readParameter(config().MaybeWire, default3);
  static_assert(std::is_same_v<decltype(id22), ID_t>);
  BOOST_CHECK_EQUAL(id22, expected2);
  BOOST_CHECK_EQUAL(default3value, default3);

  default3value = default3;
  auto id23
    = geo::fhicl::readParameter(config().MaybeWire, std::move(default3));
  static_assert(std::is_same_v<decltype(id23), ID_t>);
  BOOST_CHECK_EQUAL(id23, expected2);
  BOOST_CHECK_EQUAL(default3value, default3);

  // this one is omitted:
  auto id31 = geo::fhicl::readParameter(config().NoWire);
  static_assert(std::is_same_v<decltype(id31), std::optional<ID_t>>);
  BOOST_CHECK(!id31.has_value());

  default3value = default3;
  auto id32 = geo::fhicl::readParameter(config().NoWire, default3);
  static_assert(std::is_same_v<decltype(id32), ID_t>);
  BOOST_CHECK_EQUAL(id32, default3);
  BOOST_CHECK_EQUAL(default3value, default3);

  default3value = default3;
  auto id33 = geo::fhicl::readParameter(config().NoWire, std::move(default3));
  static_assert(std::is_same_v<decltype(id33), ID_t>);
  BOOST_CHECK_EQUAL(id33, default3);

  // test for compilation
  auto id34 = geo::fhicl::readParameter(config().NoWire, { 1U, 3U, 6U, 9U });
  BOOST_CHECK_EQUAL(id34, (geo::WireID{ 1U, 3U, 6U, 9U }));


  //
  // read sequence
  //
  auto id41 = geo::fhicl::readParameter(config().Wires);
  static_assert(std::is_same_v<decltype(id41), std::vector<ID_t>>);
  BOOST_CHECK_EQUAL(id41.size(), expected4.size());
  std::size_t max41 = std::max(id41.size(), expected4.size());
  for (std::size_t i = 0U; i < max41; ++i) BOOST_TEST_CONTEXT("element " << i) {
    BOOST_CHECK_EQUAL(id41[i], expected4[i]);
  }

  //
  // read optional sequence
  //
  // this one is present (default values should be ignored):
  auto id51 = geo::fhicl::readParameter(config().MaybeWires);
  static_assert
    (std::is_same_v<decltype(id51), std::optional<std::vector<ID_t>>>);
  BOOST_CHECK(id51.has_value());
  if (id51.has_value()) {
    BOOST_CHECK_EQUAL_COLLECTIONS
      (id51->begin(), id51->end(), expected5.begin(), expected5.end());
  }

  default6value = default6;
  auto id52 = geo::fhicl::readParameter(config().MaybeWires, default6value);
  static_assert(std::is_same_v<decltype(id52), std::vector<ID_t>>);
  BOOST_CHECK_EQUAL_COLLECTIONS
    (id52.begin(), id52.end(), expected5.begin(), expected5.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(
    default6value.begin(), default6value.end(),
    default6.begin(), default6.end()
    );

  default6value = default6;
  auto id53
    = geo::fhicl::readParameter(config().MaybeWires, std::move(default6value));
  static_assert(std::is_same_v<decltype(id53), std::vector<ID_t>>);
  BOOST_CHECK_EQUAL_COLLECTIONS
    (id53.begin(), id53.end(), expected5.begin(), expected5.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(
    default6value.begin(), default6value.end(),
    default6.begin(), default6.end()
    );

  // this one is omitted:
  auto id61 = geo::fhicl::readParameter(config().NoWires);
  static_assert
    (std::is_same_v<decltype(id61), std::optional<std::vector<ID_t>>>);
  BOOST_CHECK(!id61.has_value());

  default6value = default6;
  auto id62 = geo::fhicl::readParameter(config().NoWires, default6value);
  static_assert(std::is_same_v<decltype(id62), std::vector<ID_t>>);
  BOOST_CHECK_EQUAL_COLLECTIONS
    (id62.begin(), id62.end(), default6.begin(), default6.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(
    default6value.begin(), default6value.end(),
    default6.begin(), default6.end()
    );

  default6value = default6;
  auto id63
    = geo::fhicl::readParameter(config().NoWires, std::move(default6value));
  static_assert(std::is_same_v<decltype(id63), std::vector<ID_t>>);
  BOOST_CHECK_EQUAL_COLLECTIONS
    (id63.begin(), id63.end(), default6.begin(), default6.end());
  // this is a bit out of standard, since after moving an object is guaranteed
  // only to be good for destruction; yet usually implementations of std::vector
  // leave it as a fully valid empty vector:
  BOOST_CHECK(default6value.empty());

} // test_WireUnifiedInterface()


// --- END -- Wire ID tests ----------------------------------------------------



// --- BEGIN -- Documentation tests --------------------------------------------
void test_groupDocumentation_example1() {
  /*
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
   *   std::vector<geo::PlaneID> planes
   *     = geo::fhicl::readIDsequence(config().Planes);
   *   if (planes.empty()) {
   *     throw art::Exception(art::errors::Configuration)
   *       << "At least one plane is needed.\n";
   *   }
   *
   *   geo::PlaneID refPlane; // invalid by default
   *   if (!config().ReferencePlane(refPlane)) refPlane = planes.front();
   *
   * } // readParams()
   *
   */

  //
  // code (adapted to non-art context)
  //
  struct Config {

    geo::fhicl::PlaneIDsequence Planes {
      fhicl::Name("Planes"),
      fhicl::Comment("anode planes to process")
      };

    geo::fhicl::OptionalPlaneID ReferencePlane {
      fhicl::Name("ReferencePlane"),
      fhicl::Comment("reference anode plane (first one by default)")
      };

  }; // struct Config

  std::string const configStr = {
      "Planes: ["
    "\n  { C:0 T:1 P:0 },"
    "\n  { C:0 T:1 P:1 },"
    "\n  { C:0 T:1 P:2 }"
    "\n  ]"
    "\nReferencePlane: { C:0 T:1 P:2 }"
    "\n"
    };


  fhicl::Table<Config> config = validateConfig<Config>(configStr);

  std::vector<geo::PlaneID> const planes
    = geo::fhicl::readIDsequence(config().Planes);
  if (planes.empty()) {
    throw std::runtime_error("At least one plane is needed.");
  }

  geo::PlaneID const refPlane
    = geo::fhicl::readOptionalID(config().ReferencePlane)
      .value_or(planes.front())
    ;

  //
  // test
  //
  BOOST_CHECK_EQUAL(planes.size(), 3U);
  BOOST_CHECK_EQUAL(planes[0U], geo::PlaneID(0U, 1U, 0U));
  BOOST_CHECK_EQUAL(planes[1U], geo::PlaneID(0U, 1U, 1U));
  BOOST_CHECK_EQUAL(planes[2U], geo::PlaneID(0U, 1U, 2U));

  BOOST_CHECK_EQUAL(refPlane, geo::PlaneID(0U, 1U, 2U));

} // test_groupDocumentation_example1()


// -----------------------------------------------------------------------------
void test_groupDocumentation_example2() {
  /*
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
   *
   */

  //
  // code (adapted to non-art context)
  //
  struct Config {

    geo::fhicl::OptionalPlaneIDsequence Planes {
      fhicl::Name("Planes"),
      fhicl::Comment("anode planes to process (omit or empty processes all)")
      };

  }; // struct Config

  //
  // test A
  //
  std::string const configStrA = {
      "Planes: ["
    "\n  { C:0 T:1 P:0 },"
    "\n  { C:0 T:1 P:1 },"
    "\n  { C:0 T:1 P:2 }"
    "\n  ]"
    "\n"
    };

  fhicl::Table<Config> configA = validateConfig<Config>(configStrA);

  std::vector<geo::PlaneID> planes
    = geo::fhicl::readOptionalIDsequence(configA().Planes, {});

  BOOST_CHECK_EQUAL(planes.size(), 3U);
  BOOST_CHECK_EQUAL(planes[0U], geo::PlaneID(0U, 1U, 0U));
  BOOST_CHECK_EQUAL(planes[1U], geo::PlaneID(0U, 1U, 1U));
  BOOST_CHECK_EQUAL(planes[2U], geo::PlaneID(0U, 1U, 2U));

  //
  // test B
  //
  std::string const configStrB = { "" };

  fhicl::Table<Config> configB = validateConfig<Config>(configStrB);

  planes = geo::fhicl::readOptionalIDsequence(configB().Planes, {});

  BOOST_CHECK(planes.empty());


} // test_groupDocumentation_example2()


// --- END -- Documentation tests ----------------------------------------------


//------------------------------------------------------------------------------
//
// CryostatID test
//
BOOST_AUTO_TEST_CASE(CryostatID_testcase) {

  test_CryostatID_normal();
  test_CryostatID_invalid();

  test_OptionalCryostatID_present();
  test_OptionalCryostatID_omitted();

  test_CryostatIDsequence_normal();
  test_CryostatIDsequence_empty();

  test_OptionalCryostatIDsequence_normal();
  test_OptionalCryostatIDsequence_empty();
  test_OptionalCryostatIDsequence_omitted();

} // BOOST_AUTO_TEST_CASE(CryostatID_testcase)


//------------------------------------------------------------------------------
//
// OpDetID test
//
BOOST_AUTO_TEST_CASE(OpDetID_testcase) {

  test_OpDetID_normal();
  test_OpDetID_invalid();

  test_OptionalOpDetID_present();
  test_OptionalOpDetID_omitted();

  test_OpDetIDsequence_normal();
  test_OpDetIDsequence_empty();

  test_OptionalOpDetIDsequence_normal();
  test_OptionalOpDetIDsequence_empty();
  test_OptionalOpDetIDsequence_omitted();

} // BOOST_AUTO_TEST_CASE(OpDetID_testcase)


//------------------------------------------------------------------------------
//
// TPCID test
//
BOOST_AUTO_TEST_CASE(TPCID_testcase) {

  test_TPCID_normal();
  test_TPCID_invalid();

  test_OptionalTPCID_present();
  test_OptionalTPCID_omitted();

  test_TPCIDsequence_normal();
  test_TPCIDsequence_empty();

  test_OptionalTPCIDsequence_normal();
  test_OptionalTPCIDsequence_empty();
  test_OptionalTPCIDsequence_omitted();

} // BOOST_AUTO_TEST_CASE(TPCID_testcase)


//------------------------------------------------------------------------------
//
// PlaneID test
//
BOOST_AUTO_TEST_CASE(PlaneID_testcase) {

  test_PlaneID_normal();
  test_PlaneID_invalid();

  test_OptionalPlaneID_present();
  test_OptionalPlaneID_omitted();

  test_PlaneIDsequence_normal();
  test_PlaneIDsequence_empty();

  test_OptionalPlaneIDsequence_normal();
  test_OptionalPlaneIDsequence_empty();
  test_OptionalPlaneIDsequence_omitted();

} // BOOST_AUTO_TEST_CASE(PlaneID_testcase)


//------------------------------------------------------------------------------
//
// WireID test
//
BOOST_AUTO_TEST_CASE(WireID_testcase) {

  test_WireID_normal();
  test_WireID_invalid();

  test_OptionalWireID_present();
  test_OptionalWireID_omitted();

  test_WireIDsequence_normal();
  test_WireIDsequence_empty();

  test_OptionalWireIDsequence_normal();
  test_OptionalWireIDsequence_empty();
  test_OptionalWireIDsequence_omitted();

  test_WireUnifiedInterface();

} // BOOST_AUTO_TEST_CASE(WireID_testcase)


//------------------------------------------------------------------------------
//
// documentation test
//
BOOST_AUTO_TEST_CASE(documentation_testcase) {

  test_groupDocumentation_example1();
  test_groupDocumentation_example2();

} // BOOST_AUTO_TEST_CASE(documentation_testcase)


//------------------------------------------------------------------------------
