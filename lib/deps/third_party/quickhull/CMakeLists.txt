# Alternate quickhull library
# https://github.com/akuukka/quickhull

include(addlib)

add_library(quickhull STATIC 
	quickhull/ConvexHull.hpp
	quickhull/HalfEdgeMesh.hpp
	quickhull/MathUtils.hpp
	quickhull/QuickHull.cpp
	quickhull/QuickHull.hpp
	quickhull/Types.hpp
	quickhull/Structs/Mesh.hpp
	quickhull/Structs/Plane.hpp
	quickhull/Structs/Pool.hpp
	quickhull/Structs/Ray.hpp
	quickhull/Structs/Vector3.hpp
	quickhull/Structs/VertexDataSource.hpp
	)
addlib(quickhull STATIC)
storebin(quickhull)
target_include_directories(quickhull PUBLIC 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
add_library(
    icedb::quickhull ALIAS quickhull )
#list(APPEND ICEDB_TARGETS quickhull)
