
find_package(ROOT COMPONENTS GenVector REQUIRED)

cet_make_library( SOURCE geo_types.cxx )
build_dictionary(DICTIONARY_LIBRARIES larcoreobj_SimpleTypesAndConstants)

install_headers()
install_source()

