# Implementing a separate build of qhull here, with a better build system
# Only the qhull C++ library is needed.

include(addlib)

option (BUILD_PLUGIN_QHULL "Build the qhull plugin" ON)

if (BUILD_PLUGIN_QHULL)

	# Define qhull_VERSION in CMakeLists.txt, Makefile, qhull-exports.def, qhull_p-exports.def, qhull_r-exports.def, qhull-warn.pri
	set(qhull_VERSION2 "2015.2 2016/01/18")  # not used, See global.c, global_r.c, rbox.c, rbox_r.c
	set(qhull_VERSION     "7.2.0")  # Advance every release

	# SOVERSION -- qhull 2003 = empty, 2009 = 5, 2010-2012 = 6, 2015 (reentrant) = 7
	set(qhull_SOVERSION 7) # For SOVERSION 

	# Non-reentrant Qhull
	set(libqhull_HEADERS
			deps/qhull/src/libqhull/libqhull.h
			deps/qhull/src/libqhull/geom.h
			deps/qhull/src/libqhull/io.h
			deps/qhull/src/libqhull/mem.h
			deps/qhull/src/libqhull/merge.h
			deps/qhull/src/libqhull/poly.h
			deps/qhull/src/libqhull/qhull_a.h
			deps/qhull/src/libqhull/qset.h
			deps/qhull/src/libqhull/random.h
			deps/qhull/src/libqhull/stat.h
			deps/qhull/src/libqhull/user.h
	)
	set(libqhull_SOURCES
			deps/qhull/src/libqhull/global.c
			deps/qhull/src/libqhull/stat.c
			deps/qhull/src/libqhull/geom2.c
			deps/qhull/src/libqhull/poly2.c
			deps/qhull/src/libqhull/merge.c
			deps/qhull/src/libqhull/libqhull.c
			deps/qhull/src/libqhull/geom.c
			deps/qhull/src/libqhull/poly.c
			deps/qhull/src/libqhull/qset.c
			deps/qhull/src/libqhull/mem.c
			deps/qhull/src/libqhull/random.c
			deps/qhull/src/libqhull/usermem.c
			deps/qhull/src/libqhull/userprintf.c
			deps/qhull/src/libqhull/io.c
			deps/qhull/src/libqhull/user.c
			deps/qhull/src/libqhull/rboxlib.c
			deps/qhull/src/libqhull/userprintf_rbox.c
			${libqhull_HEADERS}
	)

	# Reeentrant Qhull
	set(libqhullr_HEADERS
			deps/qhull/src/libqhull_r/libqhull_r.h
			deps/qhull/src/libqhull_r/geom_r.h
			deps/qhull/src/libqhull_r/io_r.h
			deps/qhull/src/libqhull_r/mem_r.h
			deps/qhull/src/libqhull_r/merge_r.h
			deps/qhull/src/libqhull_r/poly_r.h
			deps/qhull/src/libqhull_r/qhull_ra.h
			deps/qhull/src/libqhull_r/qset_r.h
			deps/qhull/src/libqhull_r/random_r.h
			deps/qhull/src/libqhull_r/stat_r.h
			deps/qhull/src/libqhull_r/user_r.h
	)
	set(libqhullr_SOURCES
			deps/qhull/src/libqhull_r/global_r.c
			deps/qhull/src/libqhull_r/stat_r.c
			deps/qhull/src/libqhull_r/geom2_r.c
			deps/qhull/src/libqhull_r/poly2_r.c
			deps/qhull/src/libqhull_r/merge_r.c
			deps/qhull/src/libqhull_r/libqhull_r.c
			deps/qhull/src/libqhull_r/geom_r.c
			deps/qhull/src/libqhull_r/poly_r.c
			deps/qhull/src/libqhull_r/qset_r.c
			deps/qhull/src/libqhull_r/mem_r.c
			deps/qhull/src/libqhull_r/random_r.c
			deps/qhull/src/libqhull_r/usermem_r.c
			deps/qhull/src/libqhull_r/userprintf_r.c
			deps/qhull/src/libqhull_r/io_r.c
			deps/qhull/src/libqhull_r/user_r.c
			deps/qhull/src/libqhull_r/rboxlib_r.c
			deps/qhull/src/libqhull_r/userprintf_rbox_r.c
			${libqhullr_HEADERS}
	)

	# C++ interface to reentrant Qhull
	set(libqhullcpp_HEADERS
			deps/qhull/src/libqhullcpp/Coordinates.h
			deps/qhull/src/libqhullcpp/functionObjects.h
			deps/qhull/src/libqhullcpp/PointCoordinates.h
			deps/qhull/src/libqhullcpp/Qhull.h
			deps/qhull/src/libqhullcpp/QhullError.h
			deps/qhull/src/libqhullcpp/QhullFacet.h
			deps/qhull/src/libqhullcpp/QhullFacetList.h
			deps/qhull/src/libqhullcpp/QhullFacetSet.h
			deps/qhull/src/libqhullcpp/QhullHyperplane.h
			deps/qhull/src/libqhullcpp/QhullIterator.h
			deps/qhull/src/libqhullcpp/QhullLinkedList.h
			deps/qhull/src/libqhullcpp/QhullPoint.h
			deps/qhull/src/libqhullcpp/QhullPoints.h
			deps/qhull/src/libqhullcpp/QhullPointSet.h
			deps/qhull/src/libqhullcpp/QhullQh.h
			deps/qhull/src/libqhullcpp/QhullRidge.h
			deps/qhull/src/libqhullcpp/QhullSet.h
			deps/qhull/src/libqhullcpp/QhullSets.h
			deps/qhull/src/libqhullcpp/QhullStat.h
			deps/qhull/src/libqhullcpp/QhullVertex.h
			deps/qhull/src/libqhullcpp/QhullVertexSet.h
			deps/qhull/src/libqhullcpp/RboxPoints.h
			deps/qhull/src/libqhullcpp/RoadError.h
			deps/qhull/src/libqhullcpp/RoadLogEvent.h
			deps/qhull/src/qhulltest/RoadTest.h
	)

	set(libqhullcpp_SOURCES
			deps/qhull/src/libqhullcpp/Coordinates.cpp
			deps/qhull/src/libqhullcpp/PointCoordinates.cpp
			deps/qhull/src/libqhullcpp/Qhull.cpp
			deps/qhull/src/libqhullcpp/QhullFacet.cpp
			deps/qhull/src/libqhullcpp/QhullFacetList.cpp
			deps/qhull/src/libqhullcpp/QhullFacetSet.cpp
			deps/qhull/src/libqhullcpp/QhullHyperplane.cpp
			deps/qhull/src/libqhullcpp/QhullPoint.cpp
			deps/qhull/src/libqhullcpp/QhullPointSet.cpp
			deps/qhull/src/libqhullcpp/QhullPoints.cpp
			deps/qhull/src/libqhullcpp/QhullQh.cpp
			deps/qhull/src/libqhullcpp/QhullRidge.cpp
			deps/qhull/src/libqhullcpp/QhullSet.cpp
			deps/qhull/src/libqhullcpp/QhullStat.cpp
			deps/qhull/src/libqhullcpp/QhullVertex.cpp
			deps/qhull/src/libqhullcpp/QhullVertexSet.cpp
			deps/qhull/src/libqhullcpp/RboxPoints.cpp
			deps/qhull/src/libqhullcpp/RoadError.cpp
			deps/qhull/src/libqhullcpp/RoadLogEvent.cpp
			${libqhullcpp_HEADERS}
	)


	# ---------------------------------------
	# Define shared library for reentrant qhull
	# ---------------------------------------

	add_library(qhull_r SHARED 
			${libqhullr_SOURCES}
			deps/qhull/src/libqhull_r/qhull_r-exports.def)
	set_target_properties(qhull_r PROPERTIES
		SOVERSION ${qhull_SOVERSION}
		VERSION ${qhull_VERSION})
	if(UNIX)
		target_link_libraries(qhull_r m)
	endif(UNIX)
	addlib(qhull_r SHARED)

	# ---------------------------------------
	# Define static libraries qhullstatic_r (reentrant)
	# ---------------------------------------

	add_library(qhullstatic_r STATIC ${libqhullr_SOURCES})
	set_target_properties(qhullstatic_r PROPERTIES VERSION ${qhull_VERSION})
	if(UNIX)
		target_link_libraries(qhullstatic_r m)
	endif(UNIX)
	addlib(qhullstatic_r SHARED)

	# ---------------------------------------
	# Define C++ static library qhullcpp
	#     Do not create libqhullcpp as a shared library.  Qhull C++ classes may change layout and size. 
	# ---------------------------------------

	add_library(qhullcpp STATIC ${libqhullcpp_SOURCES})
	set_target_properties(qhullcpp PROPERTIES VERSION ${qhull_VERSION})
	addlib(qhullcpp STATIC)
	target_include_directories(qhullcpp PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/qhull/src>)


	# Alternate quickhull library
	# https://github.com/akuukka/quickhull
	add_library(quickhull STATIC 
		deps/quickhull/ConvexHull.hpp
		deps/quickhull/HalfEdgeMesh.hpp
		deps/quickhull/MathUtils.hpp
		deps/quickhull/QuickHull.cpp
		deps/quickhull/QuickHull.hpp
		deps/quickhull/Types.hpp
		deps/quickhull/Structs/Mesh.hpp
		deps/quickhull/Structs/Plane.hpp
		deps/quickhull/Structs/Pool.hpp
		deps/quickhull/Structs/Ray.hpp
		deps/quickhull/Structs/Vector3.hpp
		deps/quickhull/Structs/VertexDataSource.hpp
		)
	addlib(quickhull STATIC)
	target_include_directories(quickhull PUBLIC 
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/deps/quickhull>)

	# The plugin
	add_library(qh SHARED README.md plugin-qhull.hpp plugin-qhull.cpp)
	addplugin(qh Algorithms icedb)
	target_link_libraries(qh icedb qhullcpp qhullstatic_r quickhull)
	#target_compile_definitions(io_adda PRIVATE _SILENCE_FPOS_SEEKPOS_DEPRECATION_WARNING=1)
endif()
