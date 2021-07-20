/**
 * @file   readout_types_fhicl_test.cc
 * @brief  Test of `larcoreobj/SimpleTypesAndConstants/readout_types_fhicl.h`.
 * @author Gianluca Petrillo (petrillo@slac.stanford.edu)
 * @date   November 26, 2019
 */

// Boost libraries
#define BOOST_TEST_MODULE ( readout_types_fhicl_test )
#include "boost/test/unit_test.hpp"

// LArSoft libraries
#include "larcoreobj/SimpleTypesAndConstants/readout_types_fhicl.h"
#include "larcoreobj/SimpleTypesAndConstants/readout_types.h"

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

  using ID_t = readout::CryostatID;
  struct Config { readout::fhicl::CryostatID Cryo { fhicl::Name("Cryo") }; };

  std::string const configStr { "Cryo: { C:2 }" };
  ID_t const expectedID { 2U };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.Cryo);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_TEST(id.isValid == expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_TEST(validatedConfig.Cryo().ID() == expectedID);
    BOOST_TEST(id == expectedID);
  }
} // test_CryostatID_normal()


void test_CryostatID_invalid() {

  using ID_t = readout::CryostatID;
  struct Config { readout::fhicl::CryostatID Cryo { fhicl::Name("Cryo") }; };

  std::string const configStr { "Cryo: { isValid:false }" };
  ID_t const expectedID {};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.Cryo);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_TEST(id.isValid == expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_TEST(validatedConfig.Cryo().ID() == expectedID);
    BOOST_TEST(id == expectedID);
  }
} // test_CryostatID_invalid()


void test_OptionalCryostatID_present() {

  using ID_t = readout::CryostatID;
  struct Config
    { readout::fhicl::OptionalCryostatID Cryo { fhicl::Name("Cryo") }; };

  std::string const configStr { "Cryo: { C:1 }" };
  auto const expectedID = std::make_optional<ID_t>(1U);

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id = readOptionalID(validatedConfig.Cryo);
  BOOST_TEST(id.has_value() == expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_TEST(id->isValid == expectedID->isValid);
    if (expectedID->isValid) BOOST_TEST(id.value() == expectedID.value());
  }

} // test_OptionalCryostatID_present()


void test_OptionalCryostatID_omitted() {

  using ID_t = readout::CryostatID;
  struct Config
    { readout::fhicl::OptionalCryostatID Cryo { fhicl::Name("Cryo") }; };

  std::string const configStr { "" };

  std::optional<ID_t> const expectedID;

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id
    = geo::fhicl::readOptionalID(validatedConfig.Cryo);
  BOOST_TEST(id.has_value() == expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_TEST(id->isValid == expectedID->isValid);
    if (expectedID->isValid) BOOST_TEST(id.value() == expectedID.value());
  }

} // test_OptionalCryostatID_omitted()


void test_CryostatIDsequence_normal() {

  using ID_t = readout::CryostatID;
  struct Config
    { readout::fhicl::CryostatIDsequence Cryos { fhicl::Name("Cryos") }; };

  std::string const configStr { "Cryos: [ { C:0 }, { C:2 } ]" };
  // BUG the double brace syntax is required to work around clang bug 21629
//  std::array<ID_t, 2U> const expectedIDs { ID_t{ 0U }, ID_t{ 2U } };
  std::array<ID_t, 2U> const expectedIDs {{ ID_t{ 0U }, ID_t{ 2U } }};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.Cryos);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_TEST(ids.size() == expectedIDs.size());
  auto const n = std::min(ids.size(), expectedIDs.size());
  for (std::size_t i = 0; i < n; ++i) {
    auto const& id = ids[i];
    ID_t const& expectedID = expectedIDs[i];

    BOOST_TEST_CONTEXT("Item [" << i << "]") {
      BOOST_TEST(id.isValid == expectedID.isValid);
      if (expectedID.isValid) BOOST_TEST(id == expectedID);
    } // BOOST_TEST_CONTEXT
  } // for

} // test_CryostatIDsequence_normal()


void test_CryostatIDsequence_empty() {

  using ID_t = readout::CryostatID;
  struct Config
    { readout::fhicl::CryostatIDsequence Cryos { fhicl::Name("Cryos") }; };

  std::string const configStr { "Cryos: []" };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.Cryos);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_TEST(ids.size() == 0U);

} // test_CryostatIDsequence_empty()


void test_OptionalCryostatIDsequence_normal() {

  using ID_t = readout::CryostatID;
  struct Config
    { readout::fhicl::OptionalCryostatIDsequence Cryos { fhicl::Name("Cryos") }; };

  std::string const configStr { "Cryos: [ { C:0 }, { C:2 } ]" };
  std::optional<std::vector<ID_t>> const expectedIDs
    (std::in_place, { ID_t{ 0U }, ID_t{ 2U } });

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.Cryos);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_TEST(ids.has_value() == expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_TEST(ids->size() == expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_TEST(id.isValid == expectedID.isValid);
        if (expectedID.isValid) BOOST_TEST(id == expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalCryostatIDsequence_normal()


void test_OptionalCryostatIDsequence_empty() {

  using ID_t = readout::CryostatID;
  struct Config
    { readout::fhicl::OptionalCryostatIDsequence Cryos { fhicl::Name("Cryos") }; };

  std::string const configStr { "Cryos: []" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::in_place);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.Cryos);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_TEST(ids.has_value() == expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_TEST(ids->size() == expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_TEST(id.isValid == expectedID.isValid);
        if (expectedID.isValid) BOOST_TEST(id == expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalCryostatIDsequence_empty()


void test_OptionalCryostatIDsequence_omitted() {

  using ID_t = readout::CryostatID;
  struct Config
    { readout::fhicl::OptionalCryostatIDsequence Cryos { fhicl::Name("Cryos") }; };

  std::string const configStr { "" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::nullopt);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.Cryos);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_TEST(ids.has_value() == expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_TEST(ids->size() == expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_TEST(id.isValid == expectedID.isValid);
        if (expectedID.isValid) BOOST_TEST(id == expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalCryostatIDsequence_omitted()


// --- END -- Cryostat ID tests ------------------------------------------------



// --- BEGIN -- TPC set ID tests -----------------------------------------------
void test_TPCsetID_normal() {

  using ID_t = readout::TPCsetID;
  struct Config { readout::fhicl::TPCsetID TPCset { fhicl::Name("TPCset") }; };

  std::string const configStr { "TPCset: { C:2 S:3 }" };
  ID_t const expectedID { 2U, 3U };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.TPCset);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_TEST(id.isValid == expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_TEST(validatedConfig.TPCset().ID() == expectedID);
    BOOST_TEST(id == expectedID);
  }
} // test_TPCsetID_normal()


void test_TPCsetID_invalid() {

  using ID_t = readout::TPCsetID;
  struct Config { readout::fhicl::TPCsetID TPCset { fhicl::Name("TPCset") }; };

  std::string const configStr { "TPCset: { isValid:false }" };
  ID_t const expectedID {};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.TPCset);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_TEST(id.isValid == expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_TEST(validatedConfig.TPCset().ID() == expectedID);
    BOOST_TEST(id == expectedID);
  }
} // test_TPCsetID_invalid()


void test_OptionalTPCsetID_present() {

  using ID_t = readout::TPCsetID;
  struct Config
    { readout::fhicl::OptionalTPCsetID TPC { fhicl::Name("TPCset") }; };

  std::string const configStr { "TPCset: { C:1 S:2 }" };
  std::optional<ID_t> const expectedID { std::in_place, 1U, 2U };

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id = readOptionalID(validatedConfig.TPC);
  BOOST_TEST(id.has_value() == expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_TEST(id->isValid == expectedID->isValid);
    if (expectedID->isValid) BOOST_TEST(id.value() == expectedID.value());
  }

} // test_OptionalTPCsetID_present()


void test_OptionalTPCsetID_omitted() {

  using ID_t = readout::TPCsetID;
  struct Config
    { readout::fhicl::OptionalTPCsetID TPCset { fhicl::Name("TPCset") }; };

  std::string const configStr { "" };

  std::optional<ID_t> const expectedID { std::nullopt };

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id
    = geo::fhicl::readOptionalID(validatedConfig.TPCset);
  BOOST_TEST(id.has_value() == expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_TEST(id->isValid == expectedID->isValid);
    if (expectedID->isValid) BOOST_TEST(id.value() == expectedID.value());
  }

} // test_OptionalTPCsetID_omitted()


void test_TPCsetIDsequence_normal() {

  using ID_t = readout::TPCsetID;
  struct Config
    { readout::fhicl::TPCsetIDsequence TPCsets { fhicl::Name("TPCsets") }; };

  std::string const configStr { "TPCsets: [ { C:0 S:1 }, { C:2 S:3 } ]" };
  // BUG the double brace syntax is required to work around clang bug 21629
//  std::array<ID_t, 2U> const expectedIDs { ID_t{ 0U, 1U }, ID_t{ 2U, 3U } };
  std::array<ID_t, 2U> const expectedIDs {{ ID_t{ 0U, 1U }, ID_t{ 2U, 3U } }};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.TPCsets);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_TEST(ids.size() == expectedIDs.size());
  auto const n = std::min(ids.size(), expectedIDs.size());
  for (std::size_t i = 0; i < n; ++i) {
    auto const& id = ids[i];
    ID_t const& expectedID = expectedIDs[i];

    BOOST_TEST_CONTEXT("Item [" << i << "]") {
      BOOST_TEST(id.isValid == expectedID.isValid);
      if (expectedID.isValid) BOOST_TEST(id == expectedID);
    } // BOOST_TEST_CONTEXT
  } // for

} // test_TPCsetIDsequence_normal()


void test_TPCsetIDsequence_empty() {

  using ID_t = readout::TPCsetID;
  struct Config
    { readout::fhicl::TPCsetIDsequence TPCsets { fhicl::Name("TPCsets") }; };

  std::string const configStr { "TPCsets: []" };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.TPCsets);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_TEST(ids.size() == 0U);

} // test_TPCsetIDsequence_empty()


void test_OptionalTPCsetIDsequence_normal() {

  using ID_t = readout::TPCsetID;
  struct Config {
    readout::fhicl::OptionalTPCsetIDsequence TPCsets { fhicl::Name("TPCsets") };
  };

  std::string const configStr { "TPCsets: [ { C:0 S:1 }, { C:2 S:3 } ]" };
  std::optional<std::vector<ID_t>> const expectedIDs
    (std::in_place, { ID_t{ 0U, 1U }, ID_t{ 2U, 3U } });

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.TPCsets);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_TEST(ids.has_value() == expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_TEST(ids->size() == expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_TEST(id.isValid == expectedID.isValid);
        if (expectedID.isValid) BOOST_TEST(id == expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalTPCsetIDsequence_normal()


void test_OptionalTPCsetIDsequence_empty() {

  using ID_t = readout::TPCsetID;
  struct Config {
    readout::fhicl::OptionalTPCsetIDsequence TPCsets { fhicl::Name("TPCsets") };
  };

  std::string const configStr { "TPCsets: []" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::in_place);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.TPCsets);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_TEST(ids.has_value() == expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_TEST(ids->size() == expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_TEST(id.isValid == expectedID.isValid);
        if (expectedID.isValid) BOOST_TEST(id == expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalTPCsetIDsequence_empty()


void test_OptionalTPCsetIDsequence_omitted() {

  using ID_t = readout::TPCsetID;
  struct Config {
    readout::fhicl::OptionalTPCsetIDsequence TPCsets { fhicl::Name("TPCsets") };
  };

  std::string const configStr { "" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::nullopt);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.TPCsets);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_TEST(ids.has_value() == expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_TEST(ids->size() == expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_TEST(id.isValid == expectedID.isValid);
        if (expectedID.isValid) BOOST_TEST(id == expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalTPCsetIDsequence_omitted()

// --- END -- TPC set ID tests -------------------------------------------------



// --- BEGIN -- Readout plane ID tests -----------------------------------------
void test_ROPID_normal() {

  using ID_t = readout::ROPID;
  struct Config { readout::fhicl::ROPID ROP { fhicl::Name("ROP") }; };

  std::string const configStr { "ROP: { C:2 S:3 R:1 }" };
  ID_t const expectedID { 2U, 3U, 1U };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.ROP);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_TEST(id.isValid == expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_TEST(validatedConfig.ROP().ID() == expectedID);
    BOOST_TEST(id == expectedID);
  }
} // test_ROPID_normal()


void test_ROPID_invalid() {

  using ID_t = readout::ROPID;
  struct Config { readout::fhicl::ROPID ROP { fhicl::Name("ROP") }; };

  std::string const configStr { "ROP: { isValid:false }" };
  ID_t const expectedID {};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto const id = geo::fhicl::readID(validatedConfig.ROP);
  static_assert(std::is_same_v<decltype(id), ID_t const>);
  BOOST_TEST(id.isValid == expectedID.isValid);
  if (expectedID.isValid) {
    BOOST_TEST(validatedConfig.ROP().ID() == expectedID);
    BOOST_TEST(id == expectedID);
  }
} // test_ROPID_invalid()


void test_OptionalROPID_present() {

  using ID_t = readout::ROPID;
  struct Config { readout::fhicl::OptionalROPID ROP { fhicl::Name("ROP") }; };

  std::string const configStr { "ROP: { C:1 S:2 R:1 }" };
  std::optional<ID_t> const expectedID { std::in_place, 1U, 2U, 1U };

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id = readOptionalID(validatedConfig.ROP);
  BOOST_TEST(id.has_value() == expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_TEST(id->isValid == expectedID->isValid);
    if (expectedID->isValid) BOOST_TEST(id.value() == expectedID.value());
  }

} // test_OptionalROPID_present()


void test_OptionalROPID_omitted() {

  using ID_t = readout::ROPID;
  struct Config { readout::fhicl::OptionalROPID ROP { fhicl::Name("ROP") }; };

  std::string const configStr { "" };

  std::optional<ID_t> const expectedID { std::nullopt };

  auto validatedConfig = validateConfig<Config>(configStr)();

  std::optional<ID_t> const id
    = geo::fhicl::readOptionalID(validatedConfig.ROP);
  BOOST_TEST(id.has_value() == expectedID.has_value());
  if (expectedID.has_value()) {
    BOOST_TEST(id->isValid == expectedID->isValid);
    if (expectedID->isValid) BOOST_TEST(id.value() == expectedID.value());
  }

} // test_OptionalROPID_omitted()


void test_ROPIDsequence_normal() {

  using ID_t = readout::ROPID;
  struct Config
    { readout::fhicl::ROPIDsequence ROPs { fhicl::Name("ROPs") }; };

  std::string const configStr
    { "ROPs: [ { C:0 S:1 R:1 }, { C:2 S:3 R:0 } ]" };
  std::array<ID_t, 2U> const expectedIDs
  // BUG the double brace syntax is required to work around clang bug 21629
//    { ID_t{ 0U, 1U, 1U }, ID_t{ 2U, 3U, 0U } };
    {{ ID_t{ 0U, 1U, 1U }, ID_t{ 2U, 3U, 0U } }};

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.ROPs);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_TEST(ids.size() == expectedIDs.size());
  auto const n = std::min(ids.size(), expectedIDs.size());
  for (std::size_t i = 0; i < n; ++i) {
    auto const& id = ids[i];
    ID_t const& expectedID = expectedIDs[i];

    BOOST_TEST_CONTEXT("Item [" << i << "]") {
      BOOST_TEST(id.isValid == expectedID.isValid);
      if (expectedID.isValid) BOOST_TEST(id == expectedID);
    } // BOOST_TEST_CONTEXT
  } // for

} // test_ROPIDsequence_normal()


void test_ROPIDsequence_empty() {

  using ID_t = readout::ROPID;
  struct Config { readout::fhicl::ROPIDsequence ROPs { fhicl::Name("ROPs") }; };

  std::string const configStr { "ROPs: []" };

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readIDsequence(validatedConfig.ROPs);
  static_assert(std::is_same_v<decltype(ids), std::vector<ID_t>>);

  BOOST_TEST(ids.size() == 0U);

} // test_ROPIDsequence_empty()


void test_OptionalROPIDsequence_normal() {

  using ID_t = readout::ROPID;
  struct Config
    { readout::fhicl::OptionalROPIDsequence ROPs { fhicl::Name("ROPs") }; };

  std::string const configStr
    { "ROPs: [ { C:0 S:1 R:1 }, { C:2 S:3 R:0 } ]" };
  std::optional<std::vector<ID_t>> const expectedIDs
    (std::in_place, { ID_t{ 0U, 1U, 1U }, ID_t{ 2U, 3U, 0U } });

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.ROPs);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_TEST(ids.has_value() == expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_TEST(ids->size() == expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_TEST(id.isValid == expectedID.isValid);
        if (expectedID.isValid) BOOST_TEST(id == expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalROPIDsequence_normal()


void test_OptionalROPIDsequence_empty() {

  using ID_t = readout::ROPID;
  struct Config
    { readout::fhicl::OptionalROPIDsequence ROPs { fhicl::Name("ROPs") }; };

  std::string const configStr { "ROPs: []" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::in_place);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.ROPs);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_TEST(ids.has_value() == expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_TEST(ids->size() == expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_TEST(id.isValid == expectedID.isValid);
        if (expectedID.isValid) BOOST_TEST(id == expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalROPIDsequence_empty()


void test_OptionalROPIDsequence_omitted() {

  using ID_t = readout::ROPID;
  struct Config
    { readout::fhicl::OptionalROPIDsequence ROPs { fhicl::Name("ROPs") }; };

  std::string const configStr { "" };
  std::optional<std::vector<ID_t>> const expectedIDs(std::nullopt);

  auto validatedConfig = validateConfig<Config>(configStr)();

  auto ids = geo::fhicl::readOptionalIDsequence(validatedConfig.ROPs);
  static_assert
    (std::is_same_v<decltype(ids), std::optional<std::vector<ID_t>>>);

  BOOST_TEST(ids.has_value() == expectedIDs.has_value());
  if (expectedIDs.has_value()) {
    BOOST_TEST(ids->size() == expectedIDs->size());
    auto const n = std::min(ids->size(), expectedIDs->size());
    for (std::size_t i = 0; i < n; ++i) {
      auto const& id = ids.value()[i];
      ID_t const& expectedID = expectedIDs.value()[i];

      BOOST_TEST_CONTEXT("Item [" << i << "]") {
        BOOST_TEST(id.isValid == expectedID.isValid);
        if (expectedID.isValid) BOOST_TEST(id == expectedID);
      } // BOOST_TEST_CONTEXT
    } // for
  }

} // test_OptionalROPIDsequence_omitted()


void test_ROPUnifiedInterface() {

  using ID_t = readout::ROPID;
  struct Config {

    readout::fhicl::ROPID ROP { fhicl::Name("ROP") };

    readout::fhicl::OptionalROPID MaybeROP { fhicl::Name("MaybeROP") };

    readout::fhicl::OptionalROPID NoROP { fhicl::Name("NoROP") };

    readout::fhicl::ROPIDsequence ROPs { fhicl::Name("ROPs") };

    readout::fhicl::OptionalROPIDsequence MaybeROPs { fhicl::Name("MaybeROPs") };

    readout::fhicl::OptionalROPIDsequence NoROPs { fhicl::Name("NoROPs") };

  }; // struct Config

  std::string const configStr {
      "ROP:       { C:1 S:5 R:0 }"
    "\nMaybeROP:  { C:1 S:5 R:1 }"
    "\n# NoROP:     @nil"
    "\n"
    "\nROPs:      [ { C:1 S:5 R:2 }, { C:1 S:5 R:3 } ] "
    "\nMaybeROPs: [ { C:1 S:5 R:4 } ] "
    "\n# NoROPs:    @nil"
    "\n"
    };

  auto const& config = validateConfig<Config>(configStr);

  ID_t const expected1                { 1U, 5U, 0U };
  ID_t const expected2                { 1U, 5U, 1U };
  std::array<ID_t, 2U> const expected4
  // BUG the double brace syntax is required to work around clang bug 21629
//    { ID_t{ 1U, 5U, 2U }, ID_t{ 1U, 5U, 3U } };
    {{ ID_t{ 1U, 5U, 2U }, ID_t{ 1U, 5U, 3U } }};
  std::array<ID_t, 1U> const expected5
  // BUG the double brace syntax is required to work around clang bug 21629
//     { ID_t{ 1U, 5U, 4U } };
    {{ ID_t{ 1U, 5U, 4U } }};

  ID_t const default3 { 1U, 5U, 5U };
  auto default3value = default3;

  std::vector<ID_t> const default6({
    ID_t{ 1U, 5U, 6U }, ID_t{ 1U, 5U, 7U }, ID_t{ 1U, 5U, 8U }
    });
  auto default6value = default6;

  //
  // read simple atom
  //
  auto id11 = geo::fhicl::readParameter(config().ROP);
  static_assert(std::is_same_v<decltype(id11), ID_t>);
  BOOST_TEST(id11 == expected1);

  //
  // read optional atom
  //

  // this one is present (default values should be ignored):
  auto id21 = geo::fhicl::readParameter(config().MaybeROP);
  static_assert(std::is_same_v<decltype(id21), std::optional<ID_t>>);
  BOOST_TEST(id21.has_value());
  if (id21.has_value()) BOOST_TEST(id21.value() == expected2);

  default3value = default3;
  auto id22 = geo::fhicl::readParameter(config().MaybeROP, default3);
  static_assert(std::is_same_v<decltype(id22), ID_t>);
  BOOST_TEST(id22 == expected2);
  BOOST_TEST(default3value == default3);

  default3value = default3;
  auto id23
    = geo::fhicl::readParameter(config().MaybeROP, std::move(default3));
  static_assert(std::is_same_v<decltype(id23), ID_t>);
  BOOST_TEST(id23 == expected2);
  BOOST_TEST(default3value == default3);

  // this one is omitted:
  auto id31 = geo::fhicl::readParameter(config().NoROP);
  static_assert(std::is_same_v<decltype(id31), std::optional<ID_t>>);
  BOOST_TEST(!id31.has_value());

  default3value = default3;
  auto id32 = geo::fhicl::readParameter(config().NoROP, default3);
  static_assert(std::is_same_v<decltype(id32), ID_t>);
  BOOST_TEST(id32 == default3);
  BOOST_TEST(default3value == default3);

  default3value = default3;
  auto id33 = geo::fhicl::readParameter(config().NoROP, std::move(default3));
  static_assert(std::is_same_v<decltype(id33), ID_t>);
  BOOST_TEST(id33 == default3);

  // test for compilation
  auto id34 = geo::fhicl::readParameter(config().NoROP, { 1U, 3U, 6U });
  BOOST_TEST(id34 == (readout::ROPID{ 1U, 3U, 6U }));


  //
  // read sequence
  //
  auto id41 = geo::fhicl::readParameter(config().ROPs);
  static_assert(std::is_same_v<decltype(id41), std::vector<ID_t>>);
  BOOST_TEST(id41.size() == expected4.size());
  std::size_t max41 = std::max(id41.size(), expected4.size());
  for (std::size_t i = 0U; i < max41; ++i) BOOST_TEST_CONTEXT("element " << i) {
    BOOST_TEST(id41[i] == expected4[i]);
  }

  //
  // read optional sequence
  //
  // this one is present (default values should be ignored):
  auto id51 = geo::fhicl::readParameter(config().MaybeROPs);
  static_assert
    (std::is_same_v<decltype(id51), std::optional<std::vector<ID_t>>>);
  BOOST_TEST(id51.has_value());
  if (id51.has_value()) {
    BOOST_CHECK_EQUAL_COLLECTIONS
      (id51->begin(), id51->end(), expected5.begin(), expected5.end());
  }

  default6value = default6;
  auto id52 = geo::fhicl::readParameter(config().MaybeROPs, default6value);
  static_assert(std::is_same_v<decltype(id52), std::vector<ID_t>>);
  BOOST_CHECK_EQUAL_COLLECTIONS
    (id52.begin(), id52.end(), expected5.begin(), expected5.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(
    default6value.begin(), default6value.end(),
    default6.begin(), default6.end()
    );

  default6value = default6;
  auto id53
    = geo::fhicl::readParameter(config().MaybeROPs, std::move(default6value));
  static_assert(std::is_same_v<decltype(id53), std::vector<ID_t>>);
  BOOST_CHECK_EQUAL_COLLECTIONS
    (id53.begin(), id53.end(), expected5.begin(), expected5.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(
    default6value.begin(), default6value.end(),
    default6.begin(), default6.end()
    );

  // this one is omitted:
  auto id61 = geo::fhicl::readParameter(config().NoROPs);
  static_assert
    (std::is_same_v<decltype(id61), std::optional<std::vector<ID_t>>>);
  BOOST_TEST(!id61.has_value());

  default6value = default6;
  auto id62 = geo::fhicl::readParameter(config().NoROPs, default6value);
  static_assert(std::is_same_v<decltype(id62), std::vector<ID_t>>);
  BOOST_CHECK_EQUAL_COLLECTIONS
    (id62.begin(), id62.end(), default6.begin(), default6.end());
  BOOST_CHECK_EQUAL_COLLECTIONS(
    default6value.begin(), default6value.end(),
    default6.begin(), default6.end()
    );

  default6value = default6;
  auto id63
    = geo::fhicl::readParameter(config().NoROPs, std::move(default6value));
  static_assert(std::is_same_v<decltype(id63), std::vector<ID_t>>);
  BOOST_CHECK_EQUAL_COLLECTIONS
    (id63.begin(), id63.end(), default6.begin(), default6.end());
  // this is a bit out of standard, since after moving an object is guaranteed
  // only to be good for destruction; yet usually implementations of std::vector
  // leave it as a fully valid empty vector:
  BOOST_TEST(default6value.empty());

} // test_ROPUnifiedInterface()


// --- END -- Readout plane ID tests -------------------------------------------



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
// TPCsetID test
//
BOOST_AUTO_TEST_CASE(TPCsetID_testcase) {

  test_TPCsetID_normal();
  test_TPCsetID_invalid();

  test_OptionalTPCsetID_present();
  test_OptionalTPCsetID_omitted();

  test_TPCsetIDsequence_normal();
  test_TPCsetIDsequence_empty();

  test_OptionalTPCsetIDsequence_normal();
  test_OptionalTPCsetIDsequence_empty();
  test_OptionalTPCsetIDsequence_omitted();

} // BOOST_AUTO_TEST_CASE(TPCsetID_testcase)


//------------------------------------------------------------------------------
//
// ROPID test
//
BOOST_AUTO_TEST_CASE(ROPID_testcase) {

  test_ROPID_normal();
  test_ROPID_invalid();

  test_OptionalROPID_present();
  test_OptionalROPID_omitted();

  test_ROPIDsequence_normal();
  test_ROPIDsequence_empty();

  test_OptionalROPIDsequence_normal();
  test_OptionalROPIDsequence_empty();
  test_OptionalROPIDsequence_omitted();
  test_ROPUnifiedInterface();

} // BOOST_AUTO_TEST_CASE(ROPID_testcase)


//------------------------------------------------------------------------------
