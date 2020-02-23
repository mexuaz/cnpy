CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)
if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(CNPY)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

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


add_executable(example1 example1.cpp)
target_link_libraries(example1 cnpy)

add_executable(boost_2_npz boost_2_npz.cpp)
target_link_libraries(boost_2_npz cnpy)
target_link_libraries(boost_2_npz ${Boost_LIBRARIES})
