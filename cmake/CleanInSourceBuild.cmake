# Removes what the former in-source builds left in the source tree (the project used to be
# configured with 'cmake .' in the project directory). Run once, from anywhere:
#     cmake -P cmake/CleanInSourceBuild.cmake
# then build out of source: 'cmake --preset default && cmake --build build -j'.
#
# Nothing tracked by git is deleted: a tracked file that the in-source build overwrote (a submodule's
# own Makefile, for instance) is restored, and a directory holding tracked files is left alone.

# Products specific to this project, relative to the project directory
set(PROJECT_EXTRA_PATHS
)

get_filename_component(SRC "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
find_package(Git QUIET)
if(NOT GIT_FOUND)
    message(FATAL_ERROR "git is needed to tell build products from tracked files")
endif()

set(_removed 0)

# Removes <path>, unless git (repository <repo>) tracks it: a tracked file is restored instead,
# a directory holding tracked files is kept.
function(_clean repo path)
    if(NOT EXISTS "${path}" AND NOT IS_SYMLINK "${path}")
        return()
    endif()
    file(RELATIVE_PATH _rel "${SRC}" "${path}")
    file(RELATIVE_PATH _in_repo "${repo}" "${path}")
    execute_process(COMMAND ${GIT_EXECUTABLE} -C "${repo}" ls-files -- "${_in_repo}"
        OUTPUT_VARIABLE _tracked ERROR_QUIET)
    if(_tracked)
        if(IS_DIRECTORY "${path}")
            message(STATUS "kept      ${_rel} (holds files tracked by git)")
        else()
            # Restored only when the in-source build changed it
            execute_process(COMMAND ${GIT_EXECUTABLE} -C "${repo}" diff --quiet -- "${_in_repo}"
                RESULT_VARIABLE _modified OUTPUT_QUIET ERROR_QUIET)
            if(NOT _modified EQUAL 0)
                execute_process(COMMAND ${GIT_EXECUTABLE} -C "${repo}" checkout -- "${_in_repo}" OUTPUT_QUIET ERROR_QUIET)
                message(STATUS "restored  ${_rel}")
            endif()
        endif()
        return()
    endif()
    file(REMOVE_RECURSE "${path}")
    message(STATUS "removed   ${_rel}")
    math(EXPR _n "${_removed} + 1")
    set(_removed ${_n} PARENT_SCOPE)
endfunction()

#---------- Project directory
set(_paths
    CMakeCache.txt CMakeFiles Makefile cmake_install.cmake CTestTestfile.cmake CTestCostData.txt
    CMakeDoxyfile.in CMakeDoxygenDefaults.cmake CPackConfig.cmake CPackSourceConfig.cmake
    install_manifest.txt compile_commands.json Testing .qt bin lib exe doc
    build build-debug build-asan
    test/gTest_version.h
    ${PROJECT_EXTRA_PATHS})
# Sources generated from modules/*.cxx.in (the version files) used to be written into src/
file(GLOB _templates RELATIVE "${SRC}/modules" "${SRC}/modules/*.cxx.in")
foreach(t IN LISTS _templates)
    string(REGEX REPLACE "\\.in$" "" _generated "${t}")
    list(APPEND _paths "src/${_generated}")
endforeach()
foreach(p IN LISTS _paths)
    _clean("${SRC}" "${SRC}/${p}")
endforeach()
file(GLOB _junk LIST_DIRECTORIES true
    "${SRC}/gTest_*.cmake" "${SRC}/cmake_test_discovery_*.json" "${SRC}/*_autogen")
foreach(p IN LISTS _junk)
    _clean("${SRC}" "${p}")
endforeach()

#---------- Submodules: CMake files generated in their sources by the in-source build
set(_submodules "")
if(EXISTS "${SRC}/.gitmodules")
    execute_process(COMMAND ${GIT_EXECUTABLE} -C "${SRC}" config -f .gitmodules --get-regexp "\\.path$"
        OUTPUT_VARIABLE _lines ERROR_QUIET)
    string(REGEX MATCHALL "[^\n]+" _lines "${_lines}")
    foreach(l IN LISTS _lines)
        string(REGEX REPLACE "^[^ ]+ " "" _sub "${l}")
        list(APPEND _submodules "${_sub}")
    endforeach()
endif()
foreach(sub IN LISTS _submodules)
    set(_dir "${SRC}/${sub}")
    # An uninitialised submodule has no .git: nothing was built in it
    if(NOT EXISTS "${_dir}/.git")
        continue()
    endif()
    file(GLOB_RECURSE _files LIST_DIRECTORIES false
        "${_dir}/Makefile" "${_dir}/cmake_install.cmake" "${_dir}/CTestTestfile.cmake"
        "${_dir}/*Config.cmake" "${_dir}/*ConfigVersion.cmake" "${_dir}/*Targets.cmake")
    foreach(f IN LISTS _files)
        if(NOT f MATCHES "/(CMakeFiles|[^/]*_autogen)/")
            _clean("${_dir}" "${f}")
        endif()
    endforeach()
    file(GLOB_RECURSE _dirs LIST_DIRECTORIES true "${_dir}/*")
    foreach(d IN LISTS _dirs)
        if(IS_DIRECTORY "${d}" AND d MATCHES "/(CMakeFiles|[^/]*_autogen)$"
           AND NOT d MATCHES "/(CMakeFiles|[^/]*_autogen)/")
            _clean("${_dir}" "${d}")
        endif()
    endforeach()
    _clean("${_dir}" "${_dir}/googletest/generated")
endforeach()

message(STATUS "${_removed} in-source build product(s) removed from ${SRC}")
message(STATUS "Now: cmake --preset default && cmake --build build -j && cmake --build build --target install")
