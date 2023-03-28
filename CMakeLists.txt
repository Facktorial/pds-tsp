cmake_minimum_required(VERSION 3.22)

include(cmake/Presets.cmake)

project(PDS_Course CXX)

option(BUILD_DOC "Build documentation" OFF)
option(LIFT_DOCS "Lift docs to folder build_docs" ON)
option(TESTING "Enable testing" OFF)

project(${CMAKE_PROJECT_NAME} VERSION "0.0.1")
message("${CMAKE_PROJECT_NAME} VERSION: ${CMAKE_PROJECT_VERSION}")

project(${CMAKE_PROJECT_NAME} DESCRIPTION "Parallel and distributive course 2023.")
message("PROJECT_DESCRIPTION: ${CMAKE_PROJECT_DESCRIPTION}")

# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

include(cmake/Postsets.cmake)

#set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "cmake")

IF (BUILD_DOC)
    include(cmake/Doxygen.cmake)
    enable_doxygen()
ENDIF (BUILD_DOC)

find_package(MPI REQUIRED)
if(MPI_FOUND)
	message("OpenMPI version: ${MPI_VERSION}")
	message("include path: ${MPI_INCLUDE_PATH}")
	message("libs: ${MPI_LIBRARIES}")
else()
	message(FATAL_ERROR "OpenMPI not found")
endif()

include_directories(${MPI_INCLUDE_PATH})
set_property(GLOBAL PROPERTY ENVIRONMENT "LD_LIBRARY_PATH=${MPI_LIBRARIES}")

set(SRC
	#"${CMAKE_PROJECT_NAME}/lab01.cpp"
	"main.cpp"
)

set(HEADERS
)

include_directories("./EitherMonad/")
include_directories("./Logger/")

add_executable(${CMAKE_PROJECT_NAME} ${SRC} ${HEADERS})

target_link_libraries(${CMAKE_PROJECT_NAME} ${MPI_LIBRARIES})

#IF (${CMAKE_TESTING})
	#add_subdirectory (tests)
#ENDIF (${CMAKE_TESTING})

IF (TESTING)
	add_executable("tests" "tests/tests.cpp" ${HEADERS})
ENDIF (TESTING)
