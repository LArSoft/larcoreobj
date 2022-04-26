cet_make_library(SOURCE readout_types.h geo_types.cxx)

cet_make_library(LIBRARY_NAME geo_vectors INTERFACE
  SOURCE geo_vectors.h geo_optical_vectors.h
  LIBRARIES INTERFACE ROOT::GenVector
)

cet_make_library(LIBRARY_NAME geo_types_fhicl INTERFACE
  SOURCE geo_types_fhicl.h
  LIBRARIES INTERFACE
  larcoreobj::SimpleTypesAndConstants
  fhiclcpp::types
)

cet_make_library(LIBRARY_NAME readout_types_fhicl INTERFACE
  SOURCE readout_types_fhicl.h
  LIBRARIES INTERFACE
  larcoreobj::SimpleTypesAndConstants
  fhiclcpp::types
)

build_dictionary(DICTIONARY_LIBRARIES larcoreobj::geo_vectors)

install_headers()
install_source()
