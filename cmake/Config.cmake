# Build release version if not specified otherwise.
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

include(${CMAKE_SOURCE_DIR}/cmake/defaults/CompilerWarnings.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/defaults/Conan.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/defaults/Errors.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/defaults/PVSStudio.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/defaults/Sanitizers.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/defaults/Windows.cmake)
