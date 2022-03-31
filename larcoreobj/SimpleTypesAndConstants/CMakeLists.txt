
find_package(ROOT COMPONENTS GenVector REQUIRED EXPORT)

cet_make_library(HEADERS_TARGET SOURCE geo_types.cxx )
cet_make_library(LIBRARY_NAME larcoreobj_geo_vectors INTERFACE SOURCE geo_vectors.h LIBRARIES INTERFACE ROOT::GenVector)
build_dictionary(DICTIONARY_LIBRARIES larcoreobj_SimpleTypesAndConstants)

install_headers()
install_source()

