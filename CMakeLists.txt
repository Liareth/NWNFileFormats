cmake_minimum_required(VERSION 3.0.2)
project(NWNFileFormats)

# Platform and compiler defines

if(${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    set(CMP_MSVC 1)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Clang)
    set(CMP_CLANG 1)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL Intel)
    set(CMP_INTEL 1)
else() # If we can't detect a compiler, we presume GCC.
    set(CMP_GCC 1)
endif()

if(WIN32)
    set(OS_WINDOWS 1)
else() # If we can't detect an OS, we presume Linux.
    set(OS_LINUX 1)

    if(NOT UNIX)
        message("Attempting to build for an unknown platform. Presuming Linux.")
    endif()
endif()

if(CMP_MSVC)
    add_definitions(-DCMP_MSVC=1)
else()
    add_definitions(-DCMP_MSVC=0)
endif()

if(CMP_CLANG)
    add_definitions(-DCMP_CLANG=1)
else()
    add_definitions(-DCMP_CLANG=0)
endif()

if(CMP_INTEL)
    add_definitions(-DCMP_INTEL=1)
else()
    add_definitions(-DCMP_INTEL=0)
endif()

if(CMP_GCC)
    add_definitions(-DCMP_GCC=1)
else()
    add_definitions(-DCMP_GCC=0)
endif()

if(OS_WINDOWS)
    add_definitions(-DOS_WINDOWS=1)
else()
    add_definitions(-DOS_WINDOWS=0)
endif()

if(OS_LINUX)
    add_definitions(-DOS_LINUX=1)
else()
    add_definitions(-DOS_LINUX=0)
endif()

# Compiler switches

if(CMP_MSVC)
    # C++17 support
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++latest")

    # Multithreaded compilation
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

    # Disable warnings for usage of compliant functions.
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)

    # Release final gets a normal PDB.
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
    set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /DEBUG:FULL")

    # RelWithDebInfo gets Edit-And-Continue support.
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /Zi")
    set(CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO "${CMAKE_EXE_LINKER_FLAGS_RELWITHDEBINFO} /DEBUG:FASTLINK")

    # Debug gets Edit-And-Continue support.
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /ZI")
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG:FASTLINK")
endif()

if(CMP_GCC OR CMP_CLANG)
    # C++17 support
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y")

    # Various warnings ...
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-format-security")
endif()

# Define targets
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -DTAR_RELEASE_FINAL=1 -DTAR_RELEASE=1 -DTAR_DEBUG=0")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -DTAR_RELEASE_FINAL=0 -DTAR_RELEASE=1 -DTAR_DEBUG=1")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DTAR_RELEASE_FINAL=0 -DTAR_RELEASE=0 -DTAR_DEBUG=1")

set(ARTIFACTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Build-Artifacts")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ARTIFACTS_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ARTIFACTS_DIR}/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Utility)
add_subdirectory(FileFormats)
add_subdirectory(Examples)
