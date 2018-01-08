set(srcs ddscat_boost_parsers.cpp
	ddscat_boost_parsers.hpp
	ddscat_fml_sca_avg.cpp
	ddscat_fml_sca_avg.hpp
	ddscat_import.cpp
	ddscat_import.hpp
	ddscat_matrices.cpp
	ddscat_matrices.hpp
	ddscat_par.cpp
	ddscat_par.hpp
	ddscat_par_keys.cpp
	ddscat_run.cpp
	ddscat_run.hpp
	ddscat_shape.cpp
	ddscat_shape.hpp
	)
set(srcs_rtmath
	from_rtmath/ddOriData.cpp
	from_rtmath/ddOriData.h
	from_rtmath/ddOriDataParsers.cpp
	from_rtmath/ddOriDataParsers.h
	from_rtmath/ddOutput.cpp
	from_rtmath/ddOutput.h
	from_rtmath/ddOutput_query.cpp
	from_rtmath/ddScattMatrix.cpp
	from_rtmath/ddScattMatrix.h
	from_rtmath/shapefile.cpp
	from_rtmath/shapefile.h
	from_rtmath/shapefile_convolute.cpp
	from_rtmath/shapefile_parsers.cpp
	from_rtmath/shapefile_query.cpp
	from_rtmath/shapefile_supplemental.h
	)
add_library(io_ddscat STATIC ${srcs} ${srcs_rtmath})

set_target_properties(io_ddscat PROPERTIES CXX_STANDARD 17)
target_include_directories(io_ddscat SYSTEM PRIVATE ${Boost_INCLUDE_DIRS})
target_include_directories(io_ddscat SYSTEM PRIVATE ${CMAKE_SOURCE_DIR})
#target_link_libraries(io_ddscat libicedb ${Boost_PROGRAM_OPTIONS_LIBRARY}) # ${HDF5_LIBRARIES})

addlib(io_ddscat STATIC)

