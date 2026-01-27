#-------------------- Configuration du projet
#-------------------- CMAKE Configuration
CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

project(DeepSkyProject CXX)
message("${Green}-- Project source directory :" ${CMAKE_SOURCE_DIR} "${ColourReset}")

set(CMAKE_BUILD_DIRECTORY ./build)

if(DEFINED ENV{DeepSkyROOT})
    set(DeepSkyROOT $ENV{DeepSkyROOT})
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
    set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/../. CACHE PATH "default install path" FORCE)
endif()

set(EXECUTABLE_OUTPUT_PATH ./build/exe/)
set(LIBRARY_OUTPUT_PATH ./build/lib/)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/modules")

message("${BoldYellow}-- CMAKE INSTALL PREFIX : ${ColourReset}" ${CMAKE_INSTALL_PREFIX})

#-------------------- COMPILER
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++23" COMPILER_SUPPORTS_CXX23)
CHECK_CXX_COMPILER_FLAG("-std=c++20" COMPILER_SUPPORTS_CXX20)
CHECK_CXX_COMPILER_FLAG("-std=c++17" COMPILER_SUPPORTS_CXX17)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)

if(COMPILER_SUPPORTS_CXX23)
    message("-- USE CXX23 STANDARD")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23")
    set(CMAKE_CXX_STANDARD "23")
elseif(COMPILER_SUPPORTS_CXX20)
    message("-- USE CXX20 STANDARD")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")
    set(CMAKE_CXX_STANDARD "20")
elseif(COMPILER_SUPPORTS_CXX17)
    message("-- USE CXX17 STANDARD")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
    set(CMAKE_CXX_STANDARD "17")
elseif(COMPILER_SUPPORTS_CXX11)
    message("-- USE CXX11 STANDARD")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    set(CMAKE_CXX_STANDARD "11")
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
    # Linux specific code (homogenized)
    cmake_policy(SET CMP0069 NEW)
    set(CMAKE_SYSTEM_PROCESSOR x86_64)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -pipe -pthread -fexceptions -fPIC -m64 -Wno-unknown-pragmas -Wmisleading-indentation -flto -mtune=native -ffp-contract=on -fomit-frame-pointer")

    # Enable Interprocedural Optimization (LTO) when supported on Linux
    include(CheckIPOSupported)
    check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
    if(ipo_supported)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    endif()

    set(OperatingSystem "Linux")
    ADD_DEFINITIONS(-DLinuxx86_64)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

#-------------------- DARWIN specifics
IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    # macOS specific code with architecture distinction
    if(CMAKE_SYSTEM_PROCESSOR MATCHES "arm64|aarch64")
        # Apple Silicon (ARM64)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -flto=thin -mcpu=native -mtune=native -ffp-contract=on -fomit-frame-pointer")
        set(OperatingSystem "Mac OS X")
        ADD_DEFINITIONS(-DDarwin -DDarwinarm64)
        set(CMAKE_OSX_ARCHITECTURES "arm64")
    elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|amd64")
        # Apple Intel (x86_64)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -m64 -fPIC -flto=thin -mcpu=native -mtune=native -ffp-contract=on -fomit-frame-pointer")
        set(OperatingSystem "Mac OS X")
        ADD_DEFINITIONS(-DDarwin -DDarwinx86_64)
        set(CMAKE_OSX_ARCHITECTURES "x86_64")
    else()
        # Fallback
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fPIC -flto=thin -mcpu=native -mtune=native -ffp-contract=on -fomit-frame-pointer")
        set(OperatingSystem "Mac OS X")
        ADD_DEFINITIONS(-DDarwin)
    endif()
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

option(WITH_DEBUG "Activate debug mode" OFF)
if(WITH_DEBUG)
	message("${Blue}-- DEBUG MODE ACTIVATED${ColourReset}")
	add_definitions(-D__DEBUG__)
	set(CMAKE_BUILD_TYPE DEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
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
message("-- PKG VERSION : " ${VERSION})
message("-- PKG VERSION : " ${VERSION_SHORT})
message("-- PKG MAJOR   : " ${VERSION_MAJOR})
message("-- PKG MINOR   : " ${VERSION_MINOR})
message("-- PKG PATCH   : " ${VERSION_PATCH})
message("-- PKG SHA1    : " ${VERSION_SHA1})

set(CMAKE_PROJECT_VERSION "${VERSION}")
set(CMAKE_PROJECT_VERSION_MAJOR "${VERSION_MAJOR}")
set(CMAKE_PROJECT_VERSION_MINOR "${VERSION_MINOR}")
set(CMAKE_PROJECT_VERSION_PATCH "${VERSION_PATCH}")

configure_file(${CMAKE_MODULE_PATH}/DSS_version.cxx.in ${CMAKE_CURRENT_BINARY_DIR}/src/DSS_version.cxx)
set(version_file "${CMAKE_CURRENT_BINARY_DIR}/src/DSS_version.cxx")

configure_file(${CMAKE_MODULE_PATH}/gTest_version.h.in ${CMAKE_CURRENT_BINARY_DIR}/test/gTest_version.h)

#-------------------- Inclusion des en-têtes publics
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR}/test ./include ./include/DSTCore)

message("-- PROJECT DIR: " ${CMAKE_SOURCE_DIR})
message("-- PROJECT BINARY DIR: " ${PROJECT_BINARY_DIR})
message("-- CURRENT BINARY DIR: " ${CMAKE_CURRENT_BINARY_DIR})
message("-- CMAKE SOURCE DIR  : " ${CMAKE_SOURCE_DIR})


#-------------------- Find Boost dependencies
if(!${COMPILER_SUPPORTS_CXX17})
    message("${Red}----------- SEARCH BOOST Framework -----------${ColourReset}")
    find_package(Boost REQUIRED COMPONENTS filesystem chrono)
    if(${Boost_FOUND})
        message("${Blue}-- Boost INC DIR ${ColourReset} ${Boost_INCLUDE_DIRS}")
        message("${Blue}-- Boost LIBRARY DIR ${ColourReset} ${Boost_LIBRARY_DIRS}")
        message("${Blue}-- Boost LIB ${ColourReset} ${Boost_LIBRARIES}")
    endif(${Boost_FOUND})
    set(Boost_USE_MULTITHREADED ON)
endif(!${COMPILER_SUPPORTS_CXX17})

#-------------------- LIST SOURCE FILE

file(GLOB CORE_SRC "src/*.cxx")
file(GLOB GEN_SRC "${CMAKE_CURRENT_BINARY_DIR}/src/*.cxx")
file(GLOB CORE_HEADER "include/DSTCore/*.h")
file(GLOB GEN_HEADER "${CMAKE_CURRENT_BINARY_DIR}/test/*.h")
file(GLOB TEST_SRC "test/*.cxx")

set(HEADER_FILES ${CORE_HEADER})

#-------------------- Configuration de la bibliothèque

add_library(
    DSTCore
    SHARED
    ${CORE_SRC}
    ${CORE_HEADER}
    ${version_file}
)

#-------------------- Configuration des test
add_subdirectory(googletest   EXCLUDE_FROM_ALL)
include_directories(${PARENT_PATH_OF_GTEST}/googltest/googltest/include)
enable_testing()

foreach( test_source_file ${TEST_SRC} )
	get_filename_component( tSRC ${test_source_file} NAME_WE)
	add_executable(
		${tSRC}
		${test_source_file}
	)

    if(COMPILER_SUPPORTS_CXX17)
        target_link_libraries( ${tSRC} DSTCore gtest gtest_main)
    else(COMPILER_SUPPORTS_CXX17)
        target_link_libraries( ${tSRC} DSTCore ${Boost_LIBRARIES} gtest gtest_main)
    endif(COMPILER_SUPPORTS_CXX17)


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

add_custom_target(distclean)
set(DISTCLEANED
cmake.depends
cmake.check_depends
CMakeCache.txt
cmake.check_cache
CMakeTmp
${version_file}
Makefile
CMakeFiles
*.cmake
./build/lib
./build/exe
./build/test
./doc
./googletest/CMakeFiles
./googletest/Makefile
./googletest/*.cmake
./lib
./bin
)

add_custom_target(
    distclean_run
    COMMENT "distribution clean"
    COMMAND ${CMAKE_COMMAND} -E echo "distribution clean"
    COMMAND rm -Rf ${DISTCLEANED}
    COMMAND /usr/bin/env bash -c "test -e install_manifest.txt && xargs rm -f < install_manifest.txt || true"
    COMMAND /usr/bin/env bash -c "test -e install_manifest.txt && rm -f install_manifest.txt || true"
    VERBATIM
)
add_dependencies(distclean distclean_run)


#-------------------- Installation proceedure
if(CMAKE_INSTALL_PREFIX)
    INSTALL(FILES ${CORE_HEADER} DESTINATION include/DSTCore)
    INSTALL(TARGETS DSTCore
		RUNTIME DESTINATION exe
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib
)
endif(CMAKE_INSTALL_PREFIX)
