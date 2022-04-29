# ======================================================================
#
# Testing
#
# ======================================================================
find_package(Boost COMPONENTS unit_test_framework REQUIRED)

cet_test(geo_types_test USE_BOOST_UNIT
  LIBRARIES PRIVATE
  larcoreobj::SimpleTypesAndConstants
)

cet_test(readout_types_test USE_BOOST_UNIT
  LIBRARIES PRIVATE
  larcoreobj::SimpleTypesAndConstants
)

cet_test(geo_types_fhicl_test USE_BOOST_UNIT
  LIBRARIES PRIVATE
  larcoreobj::geo_types_fhicl
  fhiclcpp::types
  fhiclcpp::fhiclcpp
)
cet_test(readout_types_fhicl_test USE_BOOST_UNIT
  LIBRARIES PRIVATE
  larcoreobj::readout_types_fhicl
  fhiclcpp::types
  fhiclcpp::fhiclcpp
)
cet_test(testPhysicalConstants)
