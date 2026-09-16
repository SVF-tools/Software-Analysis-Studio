cmake_minimum_required(VERSION 3.23)

project(Teaching-Software-Analysis
        VERSION 1.0
        DESCRIPTION "Teaching-Software-Analysis is an open course for learning software analysis"
        HOMEPAGE_URL "https://github.com/SVF-tools/Teaching-Software-Analysis"
        LANGUAGES C CXX
)

# Check if the LLVM_DIR environment variable is defined and set it accordingly
if (DEFINED LLVM_DIR)
    set(ENV{LLVM_DIR} "${LLVM_DIR}")
elseif (DEFINED ENV{LLVM_DIR})
    set(LLVM_DIR $ENV{LLVM_DIR})
else()
    message(FATAL_ERROR "\
WARNING: The LLVM_DIR var was not set !\n\
Please set this to environment variable to point to the LLVM_DIR directory or set this variable to cmake configuration\n(e.g. on linux: export LLVM_DIR=/path/to/LLVM/dir) \n or \n \n(make the project via: cmake -DLLVM_DIR=your_path_to_LLVM) ")
endif()

# If the LLVM_DIR environment variable is set, configure CMake build flags and standards for C++ and C
if (DEFINED ENV{LLVM_DIR})
    # Set the C++ standard to C++17 and configure compiler flags based on the build type
    set(CMAKE_CXX_STANDARD 17)
    if(CMAKE_BUILD_TYPE MATCHES "Debug")
        set(CMAKE_CXX_FLAGS "-fPIC -std=gnu++17 -O0 -fno-rtti -Wno-deprecated -Werror")
    else()
        set(CMAKE_CXX_FLAGS "-fPIC -std=gnu++17 -O3 -fno-rtti -Wno-deprecated -Werror")
    endif()
    set(CMAKE_C_FLAGS "-fPIC")
    # Check if compiler is GNU and version is less than 9
    if (CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
        # Link filesystem library globally
        link_libraries(stdc++fs)
    endif()
endif()


# Locate and use the LLVM package for the project
find_package(LLVM REQUIRED CONFIG)
message(STATUS "LLVM STATUS:
    Version:                            ${LLVM_VERSION}
    Includes:                           ${LLVM_INCLUDE_DIRS}
    Libraries:                          ${LLVM_LIBRARY_DIRS}
    Build type:                         ${LLVM_BUILD_TYPE}
    RTTI enabled:                       ${LLVM_ENABLE_RTTI}
    Exceptions enabled:                 ${LLVM_ENABLE_EH}
    Dynamic lib:                        ${LLVM_LINK_LLVM_DYLIB}"
)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

# Add LLVM definitions to the compile options
add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

# Abort configuration if LLVM is not found
if(NOT "${LLVM_FOUND}")
    message(FATAL_ERROR "Failed to find supported LLVM version")
endif()

# Add the LLVM include and library directories for all subsequent targets
separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
include_directories(SYSTEM ${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

# Determine how to link with LLVM (dynamically with a single shared library or statically with multiple libraries)
if(LLVM_LINK_LLVM_DYLIB)
    message(STATUS "Linking to LLVM dynamic shared library object")
    set(llvm_libs LLVM)
else()
    message(STATUS "Linking to separate LLVM static libraries")
    llvm_map_components_to_libnames(llvm_libs bitwriter core ipo irreader instcombine instrumentation target linker analysis scalaropts support)
endif()

# Re-include AddLLVM module and configure LLVM/CMake settings
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

# Configure additional compile options for RTTI and exception handling based on LLVM settings
if(NOT LLVM_ENABLE_RTTI)
    add_compile_options("-fno-rtti")
endif()
if(NOT LLVM_ENABLE_EH)
    add_compile_options("-fno-exceptions")
endif()


# If SVF_DIR is not set while ENV{SVF_DIR} is set, sync.
if(NOT DEFINED SVF_DIR AND DEFINED ENV{SVF_DIR})
    set(SVF_DIR $ENV{SVF_DIR})
endif()
# Find the SVF CMake package (pass $SVF_DIR as a (prioritised) hint) Set
# $SVF_DIR to the installation prefix used to install SVF
find_package(SVF REQUIRED CONFIG HINTS ${SVF_DIR} ${SVF_DIR}/Debug-build ${SVF_DIR}/Release-build)

message(STATUS "SVF STATUS:
    Found:                              ${SVF_FOUND}
    Version:                            ${SVF_VERSION}
    Build mode:                         ${SVF_BUILD_TYPE}
    C++ standard:                       ${SVF_CXX_STANDARD}
    RTTI enabled:                       ${SVF_ENABLE_RTTI}
    Exceptions enabled:                 ${SVF_ENABLE_EXCEPTIONS}
    Install root directory:             ${SVF_INSTALL_ROOT}
    Install binary directory:           ${SVF_INSTALL_BIN_DIR}
    Install library directory:          ${SVF_INSTALL_LIB_DIR}
    Install include directory:          ${SVF_INSTALL_INCLUDE_DIR}
    Install 'extapi.bc' file path:      ${SVF_INSTALL_EXTAPI_FILE}")

# Set default build type to Release if not set
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Assert if CMAKE_BUILD_TYPE is Debug but SVF_BUILD_TYPE is Release
if(CMAKE_BUILD_TYPE STREQUAL "Debug" AND SVF_BUILD_TYPE STREQUAL "Release")
    message(FATAL_ERROR "CMAKE_BUILD_TYPE=Debug but SVF_BUILD_TYPE=Release is not allowed!")
endif()

# Check if SVF is found and handle importing with modern CMake methods or legacy methods
if("${SVF_FOUND}")
    message(STATUS "Found installed SVF instance; importing using modern CMake methods")

    # Ensure compatibility between SVF and LLVM in terms of RTTI and exception handling
    if(NOT (${SVF_ENABLE_RTTI} STREQUAL ${LLVM_ENABLE_RTTI}))
        message(FATAL_ERROR "SVF & LLVM RTTI support mismatch (SVF: ${SVF_ENABLE_RTTI}, LLVM: ${LLVM_ENABLE_RTTI})! This indicates that the version of LLVM used by your SVF dependency does not match the version of LLVM used by your current project. You need to check and ensure both are using the same LLVM version.")
    endif()
    if(NOT (${SVF_ENABLE_EXCEPTIONS} STREQUAL ${LLVM_ENABLE_EH}))
        message(WARNING "SVF & LLVM exceptions support mismatch (SVF: ${SVF_ENABLE_EXCEPTIONS}, LLVM: ${LLVM_ENABLE_EH}). You may not be able to catch exceptions across modules between SVF and LLVM.")
    endif()

    # Include SVF include directories and link the library directories
    include_directories(SYSTEM ${SVF_INSTALL_INCLUDE_DIR})
    link_directories(${SVF_INSTALL_LIB_DIR})
else()
    message(STATUS "Failed to find installed SVF instance; using legacy import method")
    message(FATAL_ERROR "SVF & LLVM RTTI support mismatch (SVF: ${SVF_ENABLE_RTTI}, LLVM: ${LLVM_ENABLE_RTTI})!")
endif()

# Set the SVF library components
set(SVF_LIB SvfLLVM SvfCore)

# Find and configure Z3 package, first trying the system Z3 with CMake, then fallback to SVF's Z3 instance
# Find Z3 and its include directory from the top-level include file
find_library(Z3_LIBRARIES REQUIRED NAMES z3 HINTS ${Z3_DIR} ENV Z3_DIR PATH_SUFFIXES bin lib)
find_path(Z3_INCLUDES REQUIRED NAMES z3++.h HINTS ${Z3_DIR} ENV Z3_DIR PATH_SUFFIXES include z3)
message(STATUS "Z3 STATUS:
    Z3 library file:                    ${Z3_LIBRARIES}
    Z3 include directory:               ${Z3_INCLUDES}"
  )

# Add the Z3 include directory and link the Z3 library to all targets of SVF
set(CMAKE_INSTALL_RPATH ${Z3_INCLUDES})
link_libraries(${Z3_LIBRARIES})
include_directories(SYSTEM ${Z3_INCLUDES})

# ==============================================================================
#                           SVF Ecosystem CMake Template
# ==============================================================================
#                 ↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑↑
#   Everything above this line is a general CMake template for SVF ecosystem projects.
#   Everything below this line is specific to this project (user/application code).
# ==============================================================================
enable_testing()

add_subdirectory(HelloWorld)
add_subdirectory(CodeGraph)
add_subdirectory(Assignment-1)
add_subdirectory(Assignment-2)
add_subdirectory(Assignment-3)
add_subdirectory(Lab-Z3)
add_subdirectory(Assignment-4)
