/**
 * @file   geo_types_test.cc
 * @brief  Test of geo_types.h types
 * @author Gianluca Petrillo (petrillo@fnal.gov)
 * @date   May 5th, 2015
 */

// Define the following non-zero to exclude include code that is required
// not to be compilable
#ifndef GEO_TYPES_TEST_SKIP_COMPILATION_ERRORS
#define GEO_TYPES_TEST_SKIP_COMPILATION_ERRORS 1
#endif // !GEO_TYPES_TEST_SKIP_COMPILATION_ERRORS

// Boost libraries
#define BOOST_TEST_MODULE (geo_types_test)
#include "boost/test/unit_test.hpp"

// LArSoft libraries
#include "larcoreobj/SimpleTypesAndConstants/geo_types.h"

// C/C++ standard libraries
#include <type_traits> // add_const<>

//------------------------------------------------------------------------------
// compile-time tests:
//
// consistency check between levels
static_assert(geo::details::isTopGeoElementID<geo::CryostatID>);
static_assert(!geo::details::isTopGeoElementID<geo::OpDetID>);
static_assert(!geo::details::isTopGeoElementID<geo::TPCID>);
static_assert(!geo::details::isTopGeoElementID<geo::PlaneID>);
static_assert(!geo::details::isTopGeoElementID<geo::WireID>);

static_assert(geo::CryostatID{0}.getIndex<0U>() == 0);
static_assert(geo::OpDetID{0, 1}.getIndex<0U>() == 0);
static_assert(geo::OpDetID{0, 1}.getIndex<1U>() == 1);
static_assert(geo::TPCID{0, 1}.getIndex<0U>() == 0);
static_assert(geo::TPCID{0, 1}.getIndex<1U>() == 1);
static_assert(geo::PlaneID{0, 1, 2}.getIndex<0U>() == 0);
static_assert(geo::PlaneID{0, 1, 2}.getIndex<1U>() == 1);
static_assert(geo::PlaneID{0, 1, 2}.getIndex<2U>() == 2);
static_assert(geo::WireID{0, 1, 2, 3}.getIndex<0U>() == 0);
static_assert(geo::WireID{0, 1, 2, 3}.getIndex<1U>() == 1);
static_assert(geo::WireID{0, 1, 2, 3}.getIndex<2U>() == 2);
static_assert(geo::WireID{0, 1, 2, 3}.getIndex<3U>() == 3);

// IDs must not be convertible to integers
static_assert(!std::is_convertible<geo::CryostatID, geo::CryostatID::CryostatID_t>::value,
              "geo::CryostatID can be implicitly converted to an integral type");
static_assert(!std::is_convertible<geo::OpDetID, geo::CryostatID::CryostatID_t>::value,
              "geo::OpDetID can be implicitly converted to an integral type");
static_assert(!std::is_convertible<geo::TPCID, geo::CryostatID::CryostatID_t>::value,
              "geo::TPCID can be implicitly converted to an integral type");
static_assert(!std::is_convertible<geo::PlaneID, geo::CryostatID::CryostatID_t>::value,
              "geo::PlaneID can be implicitly converted to an integral type");
static_assert(!std::is_convertible<geo::WireID, geo::CryostatID::CryostatID_t>::value,
              "geo::WireID can be implicitly converted to an integral type");

//------------------------------------------------------------------------------
// run-time tests:
//
void TestIDvalidity(geo::CryostatID const& id, bool answer)
{
  BOOST_TEST(id.isValid == answer);
  BOOST_TEST(!id == !answer);
  BOOST_TEST((bool)id == answer);
}

void TestSetIDvalidity(geo::CryostatID& id)
{
  bool const orig = bool(id);

  id.setValidity(true);
  BOOST_TEST(bool(id));
  id.setValidity(false);
  BOOST_TEST(!id);
  id.markValid();
  BOOST_TEST(bool(id));
  id.markInvalid();
  BOOST_TEST(!id);
  id.setValidity(orig);
  BOOST_TEST(bool(id) == orig);
}

/// Test comparison operators
template <typename TESTID, typename REFID = TESTID>
void TestCompareSmallerID(TESTID const& id, REFID const& smaller)
{
  BOOST_TEST(!(id < smaller));
  BOOST_TEST(!(id == smaller));
  BOOST_TEST(id != smaller);
  BOOST_TEST(smaller < id);
}

/// Test comparison operators
template <typename TESTID, typename REFID = TESTID>
void TestCompareSameID(TESTID const& id, REFID const& same)
{
  BOOST_TEST(!(id < same));
  BOOST_TEST(id == same);
  BOOST_TEST(!(id != same));
  BOOST_TEST(!(same < id));
}

/// Test comparison operators
template <typename TESTID>
void TestCompareSelfID(TESTID const& id)
{
  return TestCompareSameID(id, id);
}

/// Test comparison operators
template <typename TESTID, typename REFID = TESTID>
void TestCompareLargerID(TESTID const& id, REFID const& larger)
{
  BOOST_TEST(id < larger);
  BOOST_TEST(id != larger);
  BOOST_TEST(!(id == larger));
  BOOST_TEST(!(larger < id));
}

/// Test comparison operators
template <typename TESTID, typename REFID = TESTID>
void TestIDcomparison(TESTID const& id,
                      REFID const& smaller,
                      REFID const& same,
                      REFID const& larger)
{
  TestCompareSmallerID(id, smaller);
  TestCompareSameID(id, same);
  TestCompareSelfID(id);
  TestCompareLargerID(id, larger);
}

// --- BEGIN CryostatID tests --------------------------------------------------
void test_CryostatID_defaultConstructor()
{
  BOOST_TEST_MESSAGE("Testing default-constructed cryostat ID");

  geo::CryostatID cid;

  // a default-constructed ID is invalid:
  TestIDvalidity(cid, false);
  TestSetIDvalidity(cid);

  BOOST_TEST(&cid.deepestIndex() == &cid.Cryostat);
  BOOST_TEST(&std::as_const(cid).deepestIndex() == &cid.Cryostat);
}

void test_CryostatID_directConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing cryostat ID constructed with an integer");

  geo::CryostatID cid(1);

  // an explicitly constructed ID is valid:
  TestIDvalidity(cid, true);
  TestSetIDvalidity(cid);

  // check the ID value
  BOOST_TEST(cid.Cryostat == geo::CryostatID::CryostatID_t(1));

  // test comparison operators
  // (exercise copy constructor too)
  geo::CryostatID smaller_cid(0), same_cid(cid), larger_cid(2);

  TestIDcomparison(cid, smaller_cid, same_cid, larger_cid);

  // make sure the ID with cryostat 0 is fine (it's not a bad ID!)
  BOOST_TEST_CHECKPOINT("Testing cryostat ID constructed with an integer 0");

  geo::CryostatID first_cid(0);
  TestIDvalidity(cid, true);
  TestSetIDvalidity(cid);

  // check the ID value
  BOOST_TEST(first_cid.Cryostat == geo::CryostatID::CryostatID_t(0));
}

// --- END CryostatID tests ----------------------------------------------------

// --- BEGIN OpDetID tests -----------------------------------------------------
void test_OpDetID_defaultConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing default-constructed optical detector ID");

  geo::OpDetID oid;

  // a default-constructed ID is invalid:
  TestIDvalidity(oid, false);
  TestSetIDvalidity(oid);

  BOOST_TEST(&oid.deepestIndex() == &oid.OpDet);
  BOOST_TEST(&std::as_const(oid).deepestIndex() == &oid.OpDet);
}

void test_OpDetID_integralConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing integral-constructed optical detector ID");

#if GEO_TYPES_TEST_SKIP_COMPILATION_ERRORS
  BOOST_TEST_MESSAGE("  (test skipped)");
#else
  geo::OpDetID oid(1);

  BOOST_TEST_MESSAGE("OpDetID(1) = " << std::string(oid));

  geo::OpDetID::OpDetID_t what = oid;

  BOOST_TEST_MESSAGE("int(OpDetID(1)) = " << what);

#endif // GEO_TYPES_TEST_SKIP_COMPILATION_ERRORS
}

void test_OpDetID_nestedConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing ID-constructed optical detector ID");

  geo::CryostatID cid(1);
  geo::OpDetID oid(cid, 15);

  // an explicitly constructed ID is valid:
  TestIDvalidity(oid, true);
  TestSetIDvalidity(oid);

  // check the ID value
  BOOST_TEST(oid.Cryostat == geo::CryostatID::CryostatID_t(1));
  BOOST_TEST(oid.OpDet == geo::OpDetID::OpDetID_t(15));

  // test comparison operators (exercise copy constructor too)
  // - with optical detector ID
  BOOST_TEST_CHECKPOINT("Testing comparison with optical detector ID");
  geo::OpDetID smaller_oid(cid, oid.OpDet - 1), same_oid(oid), larger_oid(cid, oid.OpDet + 1);

  TestIDcomparison(oid, smaller_oid, same_oid, larger_oid);
}

void test_OpDetID_directConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing optical detector ID constructed with indices");

  geo::OpDetID oid(1, 15);

  // an explicitly constructed ID is valid:
  TestIDvalidity(oid, true);
  TestSetIDvalidity(oid);

  BOOST_TEST_CHECKPOINT("Testing comparison with same cryostat ID");

  geo::OpDetID smaller_oid(1, 14), same_oid(1, 15), larger_oid(1, 16);
  TestIDcomparison(oid, smaller_oid, same_oid, larger_oid);

  BOOST_TEST_CHECKPOINT("Testing comparison with different cryostat ID");
  geo::OpDetID smaller_cid(0, 16), larger_cid(2, 14);
  TestCompareSmallerID(oid, smaller_cid);
  TestCompareLargerID(oid, larger_cid);

  // make sure the ID with optical detector 0 is fine (it's not a bad ID!)
  BOOST_TEST_CHECKPOINT("Testing optical detector ID constructed with OpDet #0");

  geo::OpDetID first_oid(0, 0);
  TestIDvalidity(first_oid, true);
  TestSetIDvalidity(first_oid);

  // - check the ID value
  BOOST_TEST(first_oid.Cryostat == geo::CryostatID::CryostatID_t(0));
  BOOST_TEST(first_oid.OpDet == geo::OpDetID::OpDetID_t(0));
}

// --- END OpDetID tests -------------------------------------------------------

// --- BEGIN TPCID tests -------------------------------------------------------
void test_TPCID_defaultConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing default-constructed TPC ID");

  geo::TPCID tid;

  // a default-constructed ID is invalid:
  TestIDvalidity(tid, false);
  TestSetIDvalidity(tid);

  BOOST_TEST(&tid.deepestIndex() == &tid.TPC);
  BOOST_TEST(&std::as_const(tid).deepestIndex() == &tid.TPC);
}

void test_TPCID_integralConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing integral-constructed TPC ID");

#if GEO_TYPES_TEST_SKIP_COMPILATION_ERRORS
  BOOST_TEST_MESSAGE("  (test skipped)");
#else
  geo::TPCID tid(1);

  BOOST_TEST_MESSAGE("TPCID(1) = " << std::string(tid));

  geo::TPCID::TPCID_t what = tid;

  BOOST_TEST_MESSAGE("int(TPCID(1)) = " << what);

#endif // GEO_TYPES_TEST_SKIP_COMPILATION_ERRORS
}

void test_TPCID_nestedConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing ID-constructed TPC ID");

  geo::CryostatID cid(1);
  geo::TPCID tid(cid, 15);

  // an explicitly constructed ID is valid:
  TestIDvalidity(tid, true);
  TestSetIDvalidity(tid);

  // check the ID value
  BOOST_TEST(tid.Cryostat == geo::CryostatID::CryostatID_t(1));
  BOOST_TEST(tid.TPC == geo::TPCID::TPCID_t(15));

  // test comparison operators (exercise copy constructor too)
  // - with TPC ID
  BOOST_TEST_CHECKPOINT("Testing comparison with TPC ID");
  geo::TPCID smaller_tid(cid, tid.TPC - 1), same_tid(tid), larger_tid(cid, tid.TPC + 1);

  TestIDcomparison(tid, smaller_tid, same_tid, larger_tid);
} // test_TPCID_nestedConstructor()

void test_TPCID_directConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing TPC ID constructed with indices");

  geo::TPCID tid(1, 15);

  // an explicitly constructed ID is valid:
  TestIDvalidity(tid, true);
  TestSetIDvalidity(tid);

  BOOST_TEST_CHECKPOINT("Testing comparison with same cryostat ID");

  geo::TPCID smaller_tid(1, 14), same_tid(1, 15), larger_tid(1, 16);
  TestIDcomparison(tid, smaller_tid, same_tid, larger_tid);

  BOOST_TEST_CHECKPOINT("Testing comparison with different cryostat ID");
  geo::TPCID smaller_cid(0, 16), larger_cid(2, 14);
  TestCompareSmallerID(tid, smaller_cid);
  TestCompareLargerID(tid, larger_cid);

  // make sure the ID with TPC 0 is fine (it's not a bad ID!)
  BOOST_TEST_CHECKPOINT("Testing TPC ID constructed with a TPC #0");

  geo::TPCID first_tid(0, 0);
  TestIDvalidity(first_tid, true);
  TestSetIDvalidity(first_tid);

  // - check the ID value
  BOOST_TEST(first_tid.Cryostat == geo::CryostatID::CryostatID_t(0));
  BOOST_TEST(first_tid.TPC == geo::TPCID::TPCID_t(0));
}

// --- END TPCID tests ---------------------------------------------------------

// --- BEGIN PlaneID tests -----------------------------------------------------
void test_PlaneID_defaultConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing default-constructed plane ID");

  geo::PlaneID pid;

  // a default-constructed ID is invalid:
  TestIDvalidity(pid, false);
  TestSetIDvalidity(pid);

  BOOST_TEST(&pid.deepestIndex() == &pid.Plane);
  BOOST_TEST(&std::as_const(pid).deepestIndex() == &pid.Plane);
}

void test_PlaneID_integralConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing integral-constructed plane ID");

#if GEO_TYPES_TEST_SKIP_COMPILATION_ERRORS
  BOOST_TEST_MESSAGE("  (test skipped)");
#else
  geo::PlaneID pid(1);

  BOOST_TEST_MESSAGE("PlaneID(1) = " << std::string(pid));

  geo::PlaneID::PlaneID_t what = pid;

  BOOST_TEST_MESSAGE("int(PlaneID(1)) = " << what);

#endif // GEO_TYPES_TEST_SKIP_COMPILATION_ERRORS
}

void test_PlaneID_nestedConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing ID-constructed plane ID");

  geo::TPCID tid(1, 15);
  geo::PlaneID pid(tid, 32);

  // an explicitly constructed ID is valid:
  TestIDvalidity(pid, true);
  TestSetIDvalidity(pid);

  // check the ID value
  BOOST_TEST(pid.Cryostat == geo::CryostatID::CryostatID_t(1));
  BOOST_TEST(pid.TPC == geo::TPCID::TPCID_t(15));
  BOOST_TEST(pid.Plane == geo::PlaneID::PlaneID_t(32));

  // test comparison operators (exercise copy constructor too)
  BOOST_TEST_CHECKPOINT("Testing comparison with plane ID");
  geo::PlaneID smaller_pid(tid, pid.Plane - 1), same_pid(pid), larger_pid(tid, pid.Plane + 1);

  TestIDcomparison(pid, smaller_pid, same_pid, larger_pid);
} // test_PlaneID_nestedConstructor()

void test_PlaneID_directConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing plane ID constructed with indices");

  geo::PlaneID pid(1, 15, 32);

  // an explicitly constructed ID is valid:
  TestIDvalidity(pid, true);
  TestSetIDvalidity(pid);

  // check the ID value
  BOOST_TEST(pid.Cryostat == geo::CryostatID::CryostatID_t(1));
  BOOST_TEST(pid.TPC == geo::TPCID::TPCID_t(15));
  BOOST_TEST(pid.Plane == geo::PlaneID::PlaneID_t(32));

  BOOST_TEST_CHECKPOINT("Testing comparison with same TPC ID");

  geo::PlaneID smaller_pid(1, 15, 31), same_pid(1, 15, 32), larger_pid(1, 15, 33);
  TestIDcomparison(pid, smaller_pid, same_pid, larger_pid);

  BOOST_TEST_CHECKPOINT("Testing comparison with different TPC ID (1)");
  geo::PlaneID smaller_tid1(1, 14, 33), larger_tid1(1, 16, 31);
  TestCompareSmallerID(pid, smaller_tid1);
  TestCompareLargerID(pid, larger_tid1);
  BOOST_TEST_CHECKPOINT("Testing comparison with different TPC ID (2)");
  geo::PlaneID smaller_tid2(1, 14, 32), larger_tid2(1, 16, 32);
  TestCompareSmallerID(pid, smaller_tid2);
  TestCompareLargerID(pid, larger_tid2);

  BOOST_TEST_CHECKPOINT("Testing comparison with different cryostat ID");
  geo::PlaneID smaller_cid1(0, 15, 33), larger_cid1(2, 15, 31);
  TestCompareSmallerID(pid, smaller_cid1);
  TestCompareLargerID(pid, larger_cid1);
  BOOST_TEST_CHECKPOINT("Testing comparison with different cryostat ID (2)");
  geo::PlaneID smaller_cid2(0, 15, 32), larger_cid2(2, 15, 32);
  TestCompareSmallerID(pid, smaller_cid2);
  TestCompareLargerID(pid, larger_cid2);

  // make sure the ID with plane 0 is fine (it's not a bad ID!)
  BOOST_TEST_CHECKPOINT("Testing plane ID constructed with a plane #0");

  geo::PlaneID first_pid(0, 0, 0);
  TestIDvalidity(first_pid, true);
  TestSetIDvalidity(first_pid);

  // - check the ID value
  BOOST_TEST(first_pid.Cryostat == geo::CryostatID::CryostatID_t(0));
  BOOST_TEST(first_pid.TPC == geo::TPCID::TPCID_t(0));
  BOOST_TEST(first_pid.Plane == geo::PlaneID::PlaneID_t(0));
}

// --- END PlaneID tests -------------------------------------------------------

// --- BEGIN WireID tests ------------------------------------------------------
void test_WireID_defaultConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing default-constructed wire ID");

  geo::WireID wid;

  // a default-constructed ID is invalid:
  TestIDvalidity(wid, false);
  TestSetIDvalidity(wid);

  BOOST_TEST(&wid.deepestIndex() == &wid.Wire);
  BOOST_TEST(&std::as_const(wid).deepestIndex() == &wid.Wire);
}

void test_WireID_integralConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing integral-constructed wire ID");

#if GEO_TYPES_TEST_SKIP_COMPILATION_ERRORS
  BOOST_TEST_MESSAGE("  (test skipped)");
#else
  geo::WireID wid(1);

  BOOST_TEST_MESSAGE("WireID(1) = " << std::string(wid));

  geo::WireID::WireID_t what = wid;

  BOOST_TEST_MESSAGE("int(WireID(1)) = " << what);

#endif // GEO_TYPES_TEST_SKIP_COMPILATION_ERRORS
}

void test_WireID_nestedConstructor()
{

  BOOST_TEST_CHECKPOINT("Testing ID-constructed wire ID");

  geo::PlaneID pid(1, 15, 32);
  geo::WireID wid(pid, 27);

  // an explicitly constructed ID is valid:
  TestIDvalidity(wid, true);
  TestSetIDvalidity(wid);

  // check the ID value
  BOOST_TEST(wid.Cryostat == geo::CryostatID::CryostatID_t(1));
  BOOST_TEST(wid.TPC == geo::TPCID::TPCID_t(15));
  BOOST_TEST(wid.Plane == geo::PlaneID::PlaneID_t(32));
  BOOST_TEST(wid.Wire == geo::WireID::WireID_t(27));

  // test comparison operators (exercise copy constructor too)
  // - with TPC ID
  BOOST_TEST_CHECKPOINT("Testing comparison with wire ID");
  geo::WireID smaller_wid(pid, wid.Wire - 1), same_wid(wid), larger_wid(pid, wid.Wire + 1);

  TestIDcomparison(wid, smaller_wid, same_wid, larger_wid);

} // test_WireID_nestedConstructor()

void test_WireID_directConstructor()
{
  BOOST_TEST_CHECKPOINT("Testing wire ID constructed with indices");

  geo::WireID wid(1, 15, 32, 27);

  // an explicitly constructed ID is valid:
  TestIDvalidity(wid, true);
  TestSetIDvalidity(wid);

  // check the ID value
  BOOST_TEST(wid.Cryostat == geo::CryostatID::CryostatID_t(1));
  BOOST_TEST(wid.TPC == geo::TPCID::TPCID_t(15));
  BOOST_TEST(wid.Plane == geo::PlaneID::PlaneID_t(32));
  BOOST_TEST(wid.Wire == geo::WireID::WireID_t(27));

  BOOST_TEST_CHECKPOINT("Testing comparison with same TPC ID");

  geo::WireID smaller_wid(1, 15, 32, 26), same_wid(1, 15, 32, 27), larger_wid(1, 15, 32, 28);
  TestIDcomparison(wid, smaller_wid, same_wid, larger_wid);

  BOOST_TEST_CHECKPOINT("Testing comparison with different plane ID (1)");
  geo::WireID smaller_pid1(1, 15, 31, 28), larger_pid1(1, 15, 33, 26);
  TestCompareSmallerID(wid, smaller_pid1);
  TestCompareLargerID(wid, larger_pid1);
  BOOST_TEST_CHECKPOINT("Testing comparison with different plane ID (2)");
  geo::WireID smaller_pid2(1, 15, 31, 27), larger_pid2(1, 15, 33, 27);
  TestCompareSmallerID(wid, smaller_pid2);
  TestCompareLargerID(wid, larger_pid2);

  BOOST_TEST_CHECKPOINT("Testing comparison with different TPC ID (1)");
  geo::WireID smaller_tid1(1, 14, 32, 28), larger_tid1(1, 16, 32, 26);
  TestCompareSmallerID(wid, smaller_tid1);
  TestCompareLargerID(wid, larger_tid1);
  BOOST_TEST_CHECKPOINT("Testing comparison with different TPC ID (2)");
  geo::WireID smaller_tid2(1, 14, 32, 27), larger_tid2(1, 16, 32, 27);
  TestCompareSmallerID(wid, smaller_tid2);
  TestCompareLargerID(wid, larger_tid2);

  BOOST_TEST_CHECKPOINT("Testing comparison with different cryostat ID");
  geo::WireID smaller_cid1(0, 15, 32, 28), larger_cid1(2, 15, 32, 26);
  TestCompareSmallerID(wid, smaller_cid1);
  TestCompareLargerID(wid, larger_cid1);
  BOOST_TEST_CHECKPOINT("Testing comparison with different cryostat ID (2)");
  geo::WireID smaller_cid2(0, 15, 32, 27), larger_cid2(2, 15, 32, 27);
  TestCompareSmallerID(wid, smaller_cid2);
  TestCompareLargerID(wid, larger_cid2);

  // make sure the ID with wire 0 is fine (it's not a bad ID!)
  BOOST_TEST_CHECKPOINT("Testing wire ID constructed with a wire #0");

  geo::WireID first_wid(0, 0, 0, 0);
  TestIDvalidity(first_wid, true);
  TestSetIDvalidity(first_wid);

  // - check the ID value
  BOOST_TEST(first_wid.Cryostat == geo::CryostatID::CryostatID_t(0));
  BOOST_TEST(first_wid.TPC == geo::TPCID::TPCID_t(0));
  BOOST_TEST(first_wid.Plane == geo::PlaneID::PlaneID_t(0));
  BOOST_TEST(first_wid.Wire == geo::WireID::WireID_t(0));
}

// --- END WireID tests -------------------------------------------------------

BOOST_AUTO_TEST_CASE(CryostatIDtest)
{
  test_CryostatID_defaultConstructor();
  test_CryostatID_directConstructor();
}

BOOST_AUTO_TEST_CASE(OpDetIDtest)
{
  test_OpDetID_defaultConstructor();
  test_OpDetID_nestedConstructor();
  test_OpDetID_directConstructor();
  test_OpDetID_integralConstructor();
}

BOOST_AUTO_TEST_CASE(TPCIDtest)
{
  test_TPCID_defaultConstructor();
  test_TPCID_nestedConstructor();
  test_TPCID_directConstructor();
  test_TPCID_integralConstructor();
}

BOOST_AUTO_TEST_CASE(PlaneIDtest)
{
  test_PlaneID_defaultConstructor();
  test_PlaneID_nestedConstructor();
  test_PlaneID_directConstructor();
  test_PlaneID_integralConstructor();
}

BOOST_AUTO_TEST_CASE(WireIDtest)
{
  test_WireID_defaultConstructor();
  test_WireID_nestedConstructor();
  test_WireID_directConstructor();
  test_WireID_integralConstructor();
}
