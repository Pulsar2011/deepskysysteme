#-------------------- Configuration du projet
project(DeepSkyProject CXX)

if(DEFINED ENV{DeepSkyROOT})
    set(DeepSkyROOT $ENV{DeepSkyROOT})
endif()

if(DEFINED ENV{Qt5})
    set(Qt5_ROOT $ENV{Qt5})
#    message("-- Expected CMAKE Qt5 module path "${Qt5_ROOT})
endif()

if(NOT WIN32)
    string(ASCII 27 Esc)
    set(ColourReset "${Esc}[m")
    set(ColourBold  "${Esc}[1m")
    set(Red         "${Esc}[31m")
    set(Green       "${Esc}[32m")
    set(Yellow      "${Esc}[33m")
    set(Blue        "${Esc}[34m")
    set(Magenta     "${Esc}[35m")
    set(Cyan        "${Esc}[36m")
    set(White       "${Esc}[37m")
    set(BoldRed     "${Esc}[1;31m")
    set(BoldGreen   "${Esc}[1;32m")
    set(BoldYellow  "${Esc}[1;33m")
    set(BoldBlue    "${Esc}[1;34m")
    set(BoldMagenta "${Esc}[1;35m")
    set(BoldCyan    "${Esc}[1;36m")
    set(BoldWhite   "${Esc}[1;37m")
endif()

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${DeepSkyROOT}" CACHE PATH "default install path" FORCE)
endif()

set(EXECUTABLE_OUTPUT_PATH ./build/exe/)
set(LIBRARY_OUTPUT_PATH ./build/lib/)

#-------------------- CMAKE Configuration
CMAKE_MINIMUM_REQUIRED(VERSION 2.8.3)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules")

message("${BoldYellow}-- CMAKE INSTALL PREFIX : ${ColourReset}"${CMAKE_INSTALL_PREFIX})

#-------------------- COMPILER
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

if(COMPILER_SUPPORTS_CXX17)
    message("-- USE CXX11 STANDARD")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif(COMPILER_SUPPORTS_CXX11)
    message("-- USE CXX11 STANDARD")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    message("-- USE CX0X STANDARD")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 nor C++11 nor C++0x support. Please use a different C++ compiler.")
endif()

set(BUILD_SHARED_LIBS "YES")

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v")

#-------------------- GCC specifics
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	# Linux specific code
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -pipe -fexceptions -fPIC  -m64 -Wno-unknown-pragmas")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

	set(OperatingSystem "Linux")
	ADD_DEFINITIONS(-DLinuxx86_64)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

#-------------------- DARWIN specifics
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	# Mac OS X specific code
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -m64 -fPIC")
	set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g")

	set(OperatingSystem "Mac OS X")
	ADD_DEFINITIONS(-DDarwinx86_64)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

option(WITH_DEBUG "Activate debug mode" OFF)
if(WITH_DEBUG)
	message("${Blue}-- DEBUG MODE ACTIVATED${ColourReset}")
	add_definitions(-D__DEBUG__)
	set(CMAKE_BUILD_TYPE DEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
else(WITH_DEBUG)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
endif(WITH_DEBUG)

message("${Green}-- CXX COMPILER:${ColourReset} ${CMAKE_CXX_COMPILER}")
message("${Green}-- DEFAULT CXX FLAGES:${ColourReset} ${CMAKE_CXX_FLAGS}")

#-------------------- APPEND GIT VERSION TAG
# Make a version file containing the current version from git.
find_package(Git)
include(GetGitRevisionDescription)
git_describe(VERSION --always --tags --long)

#parse the version information into pieces.
string(REGEX REPLACE "^v([0-9]+)\\..*" "\\1" VERSION_MAJOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${VERSION}")
string(REGEX REPLACE "^v[0-9]+\\.[0-9]+\\.[0-9]+.(-*)" "\\1" VERSION_SHA1 "${VERSION}")
set(VERSION_SHORT "v${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
message("-- PKG VERSION : "${VERSION})
message("-- PKG VERSION : "${VERSION_SHORT})
message("-- PKG MAJOR   : "${VERSION_MAJOR})
message("-- PKG MINOR   : "${VERSION_MINOR})
message("-- PKG PATCH   : "${VERSION_PATCH})
message("-- PKG SHA1    : "${VERSION_SHA1})

configure_file(${CMAKE_MODULE_PATH}/DSS_version.cxx.in ${CMAKE_CURRENT_BINARY_DIR}/src/DSS_version.cxx)
set(version_file "${CMAKE_CURRENT_BINARY_DIR}/src/DSS_version.cxx")

configure_file(${CMAKE_MODULE_PATH}/gTest_version.h.in ${CMAKE_CURRENT_BINARY_DIR}/test/gTest_version.h)
#-------------------- Inclusion des en-têtes publics

INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR}/. ${PROJECT_BINARY_DIR}/include ${PROJECT_BINARY_DIR}/include/DSTCore ${PROJECT_BINARY_DIR}/gest)
link_directories(${LIBRARY_OUTPUT_PATH} ${CMAKE_SOURCE_DIR}/gtest/lib)

message("-- PROJECT DIR: "${CMAKE_SOURCE_DIR})
message("-- PROJECT BINARY DIR: "${PROJECT_BINARY_DIR})
message("-- CURRENT BINARY DIR: "${CMAKE_CURRENT_BINARY_DIR})
message("-- CMAKE SOURCE DIR  : "${CMAKE_SOURCE_DIR})

#-------------------- Find GSL dependencies
set(CMAKE_INCLUDE_PATH "${GSL_INCLUDE_DIRS}" ${CMAKE_INCLUDE_PATH})
set(CMAKE_LIBRARY_PATH "${GSL_LIBRARY_DIRS}" ${CMAKE_LIBRARY_PATH})
set(CMAKE_LIBRARY_PATH "${PROJECT_BINARY_DIR}/gtest/lib" ${CMAKE_LIBRARY_PATH})

message("-- CMAKE INCLUDE DIR  : "${CMAKE_INCLUDE_PATH})
message("-- CMAKE LIBRARY DIR  : "${CMAKE_LIBRARY_PATH})

#-------------------- LIST SOURCE FILE

file(GLOB CORE_SRC "src/*.cxx")
file(GLOB CORE_HEADER "include/DSTCore/*.h")
file(GLOB TEST_SRC "test/*.cxx")

set(HEADER_FILES ${CORE_HEADER})

#-------------------- Configuration de la bibliothèque

add_library(
    DSTCore
    SHARED
#    STATIC
    ${CORE_SRC}
    ${CORE_HEADER}
    ${version_file}
)

#-------------------- Configuration des test
#add_subdirectory("${PROJECT_SOURCE_DIR}/extern/googletest" "extern/googletest")
enable_testing()

foreach( test_source_file ${TEST_SRC} )
	get_filename_component( tSRC ${test_source_file} NAME_WE)
	add_executable(
		${tSRC}
		${test_source_file}
	)

    target_link_libraries( ${tSRC} DSTCore gtest gtest_main)

endforeach( test_source_file ${TEST_SRC} )

include(GoogleTest)

foreach( test_source_file ${TEST_SRC} )
    get_filename_component( tSRC ${test_source_file} NAME_WE)
    gtest_discover_tests(${tSRC})
endforeach( test_source_file ${TEST_SRC} )

#-------------------- CONFIGURATION OF DOXYGEN DOC GENERATION

find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/DSL.Doxyfile ${CMAKE_CURRENT_BINARY_DIR}/DSL.Doxyfile @ONLY)
    add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/DSL.Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
)
endif(DOXYGEN_FOUND)



#-------------------- ADD DISTCLEAN CUSTOM TARGET

ADD_CUSTOM_TARGET (distclean @echo cleaning for source distribution)
set(DISTCLEANED
cmake.depends
cmake.check_depends
CMakeCache.txt
cmake.check_cache
cmake_install.cmake
CMakeTmp
${version_file}
Makefile
CMakeFiles
./build/lib
./build/exe
./build/test
./doc
)

ADD_CUSTOM_COMMAND(
DEPENDS clean
COMMENT "distribution clean"
COMMAND rm ARGS    -Rf ${DISTCLEANED}
COMMAND xargs ARGS rm -f < install_manifest.txt
COMMAND rm ARGS -f install_manifest.txt
TARGET  distclean
)


#-------------------- Installation proceedure
if(CMAKE_INSTALL_PREFIX)
    INSTALL(FILES ${CORE_HEADER} DESTINATION include/DSTCore)
    INSTALL(TARGETS DSTCore
		RUNTIME DESTINATION exe
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
)
endif(CMAKE_INSTALL_PREFIX)
