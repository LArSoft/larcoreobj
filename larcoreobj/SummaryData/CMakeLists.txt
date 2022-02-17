cet_make_library( SOURCE GeometryConfigurationInfo.cxx  POTSummary.cxx  RunData.cxx )

build_dictionary(DICTIONARY_LIBRARIES larcoreobj_SummaryData canvas::canvas)

add_subdirectory(SampledDicts)

install_headers()
install_source()
