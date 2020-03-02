CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)


project(CNPY)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


message( "C++ compiler version: " ${CMAKE_CXX_COMPILER_VERSION} )
message( "C++ standard version: " ${CMAKE_CXX_STANDARD} )


option(ENABLE_STATIC "Build static (.a) library" ON)

find_package(ZLIB REQUIRED)
include_directories(${ZLIB_INCLUDE_DIRS})

add_library(cnpy SHARED "cnpy.cpp")
target_link_libraries(cnpy ${ZLIB_LIBRARIES})
install(TARGETS "cnpy" LIBRARY DESTINATION lib PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)

if(ENABLE_STATIC)
    add_library(cnpy-static STATIC "cnpy.cpp")
    set_target_properties(cnpy-static PROPERTIES OUkTPUT_NAME "cnpy")
    install(TARGETS "cnpy-static" ARCHIVE DESTINATION lib)
endif(ENABLE_STATIC)

install(FILES "cnpy.h" DESTINATION include)
install(FILES "mat2npz" "npy2mat" "npz2mat" DESTINATION bin PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)



# local boost build directory
cmake_host_system_information(RESULT _host_name QUERY HOSTNAME)
IF(${_host_name} MATCHES "amdebhp")
        set(CMAKE_PREFIX_PATH "/home/am/builds/boost_1_72_0")
ENDIF()
find_package( Boost 1.68.0 COMPONENTS system serialization REQUIRED )

include_directories( ${Boost_INCLUDE_DIR} )


# add hdf5 library
find_package(HDF5 REQUIRED COMPONENTS C CXX)
include_directories(${HDF5_INCLUDE_DIR})


add_executable(example1 example1.cpp)
target_link_libraries(example1 cnpy)

add_executable(boost_2_npz boost_2_npz.cpp)
target_link_libraries(boost_2_npz cnpy)
target_link_libraries(boost_2_npz ${Boost_LIBRARIES})

add_executable(boost_2_hdf5 boost_2_hdf5.cpp)
target_link_libraries(boost_2_hdf5 ${Boost_LIBRARIES})
target_link_libraries(boost_2_hdf5 ${HDF5_CXX_LIBRARIES} ${HDF5_LIBRARIES})

add_executable(graph_multi_cvt graph_multi_cvt.cpp)
#target_link_libraries(graph_multi_cvt cnpy)
target_link_libraries(graph_multi_cvt ${Boost_LIBRARIES})
target_link_libraries(graph_multi_cvt ${HDF5_CXX_LIBRARIES} ${HDF5_LIBRARIES})
