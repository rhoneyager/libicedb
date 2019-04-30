# Alternate quickhull library
# https://github.com/akuukka/quickhull

include(addlib)

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
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>)
add_library(
    icedb::quickhull ALIAS quickhull )
