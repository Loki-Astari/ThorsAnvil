# ThorsAnvilConfig.cmake
# --------------------
# CMake package config file for ThorsAnvil
#
# This file is installed to <prefix>/lib/cmake/ThorsAnvil/ and enables:
#
#   find_package(ThorsAnvil REQUIRED)
#   target_link_libraries(myapp ThorsAnvil::ThorSerialize)
#
# Available imported targets:
#
#   ThorsAnvil::ThorsLogging      - Leveled logging macros
#   ThorsAnvil::ThorSerialize     - JSON/YAML/BSON serialization
#   ThorsAnvil::ThorsCrypto       - Cryptographic utilities (header-only)
#   ThorsAnvil::ThorsSocket       - Async IO as std::iostream
#   ThorsAnvil::ThorsStorage      - Columnar file storage
#   ThorsAnvil::ThorsMongo        - Type-safe MongoDB client
#   ThorsAnvil::Nisse             - Coroutine-based async HTTP server
#

include(CMakeFindDependencyMacro)

# External dependencies that consumers also need
find_dependency(OpenSSL)

# Compute the install prefix relative to this file's location:
#   <prefix>/lib/cmake/ThorsAnvil/ThorsAnvilConfig.cmake
get_filename_component(THORSANVIL_PREFIX "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

set(THORSANVIL_INCLUDE_DIR "${THORSANVIL_PREFIX}/include")
set(THORSANVIL_LIB_DIR     "${THORSANVIL_PREFIX}/lib")

# Guard against multiple inclusion
if(TARGET ThorsAnvil::ThorsLogging)
    return()
endif()

# ---------------------------------------------------------------------------
# ThorsLogging (no ThorsAnvil library dependencies)
# ---------------------------------------------------------------------------
add_library(ThorsAnvil::ThorsLogging SHARED IMPORTED)
set_target_properties(ThorsAnvil::ThorsLogging PROPERTIES
    IMPORTED_LOCATION "${THORSANVIL_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}ThorsLogging${CMAKE_SHARED_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${THORSANVIL_INCLUDE_DIR};${THORSANVIL_INCLUDE_DIR}/ThorsLogging"
)

# ---------------------------------------------------------------------------
# ThorSerialize (depends on ThorsLogging; links libyaml externally)
# ---------------------------------------------------------------------------
add_library(ThorsAnvil::ThorSerialize SHARED IMPORTED)
set_target_properties(ThorsAnvil::ThorSerialize PROPERTIES
    IMPORTED_LOCATION "${THORSANVIL_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}ThorSerialize${CMAKE_SHARED_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${THORSANVIL_INCLUDE_DIR};${THORSANVIL_INCLUDE_DIR}/ThorSerialize;${THORSANVIL_INCLUDE_DIR}/ThorsIOUtil;${THORSANVIL_INCLUDE_DIR}/fast_float"
    INTERFACE_LINK_LIBRARIES "ThorsAnvil::ThorsLogging"
)

# ---------------------------------------------------------------------------
# ThorsCrypto (header-only; depends on ThorsLogging; links OpenSSL)
# ---------------------------------------------------------------------------
add_library(ThorsAnvil::ThorsCrypto INTERFACE IMPORTED)
set_target_properties(ThorsAnvil::ThorsCrypto PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${THORSANVIL_INCLUDE_DIR};${THORSANVIL_INCLUDE_DIR}/ThorsCrypto"
    INTERFACE_LINK_LIBRARIES "ThorsAnvil::ThorsLogging;OpenSSL::Crypto"
)

# ---------------------------------------------------------------------------
# ThorsSocket (depends on ThorsLogging; links OpenSSL, zlib)
# ---------------------------------------------------------------------------
add_library(ThorsAnvil::ThorsSocket SHARED IMPORTED)
set_target_properties(ThorsAnvil::ThorsSocket PROPERTIES
    IMPORTED_LOCATION "${THORSANVIL_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}ThorsSocket${CMAKE_SHARED_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${THORSANVIL_INCLUDE_DIR};${THORSANVIL_INCLUDE_DIR}/ThorsSocket"
    INTERFACE_LINK_LIBRARIES "ThorsAnvil::ThorsLogging;OpenSSL::SSL;OpenSSL::Crypto"
)

# ---------------------------------------------------------------------------
# ThorsStorage (depends on ThorSerialize)
# ---------------------------------------------------------------------------
add_library(ThorsAnvil::ThorsStorage SHARED IMPORTED)
set_target_properties(ThorsAnvil::ThorsStorage PROPERTIES
    IMPORTED_LOCATION "${THORSANVIL_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}ThorsStorage${CMAKE_SHARED_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${THORSANVIL_INCLUDE_DIR};${THORSANVIL_INCLUDE_DIR}/ThorsStorage"
    INTERFACE_LINK_LIBRARIES "ThorsAnvil::ThorSerialize"
)

# ---------------------------------------------------------------------------
# ThorsMongo (depends on ThorSerialize, ThorsSocket, ThorsLogging; links snappy)
# ---------------------------------------------------------------------------
add_library(ThorsAnvil::ThorsMongo SHARED IMPORTED)
set_target_properties(ThorsAnvil::ThorsMongo PROPERTIES
    IMPORTED_LOCATION "${THORSANVIL_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}ThorsMongo${CMAKE_SHARED_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${THORSANVIL_INCLUDE_DIR};${THORSANVIL_INCLUDE_DIR}/ThorsMongo"
    INTERFACE_LINK_LIBRARIES "ThorsAnvil::ThorSerialize;ThorsAnvil::ThorsSocket;ThorsAnvil::ThorsLogging"
)

# ---------------------------------------------------------------------------
# Nisse (depends on ThorsSocket; links libevent, boost)
# ---------------------------------------------------------------------------
add_library(ThorsAnvil::Nisse SHARED IMPORTED)
set_target_properties(ThorsAnvil::Nisse PROPERTIES
    IMPORTED_LOCATION "${THORSANVIL_LIB_DIR}/${CMAKE_SHARED_LIBRARY_PREFIX}Nisse${CMAKE_SHARED_LIBRARY_SUFFIX}"
    INTERFACE_INCLUDE_DIRECTORIES "${THORSANVIL_INCLUDE_DIR};${THORSANVIL_INCLUDE_DIR}/NisseServer;${THORSANVIL_INCLUDE_DIR}/NisseHTTP"
    INTERFACE_LINK_LIBRARIES "ThorsAnvil::ThorsSocket;ThorsAnvil::ThorSerialize"
)

# ---------------------------------------------------------------------------
# Provide a convenience variable
# ---------------------------------------------------------------------------
set(ThorsAnvil_FOUND TRUE)
set(ThorsAnvil_INCLUDE_DIRS "${THORSANVIL_INCLUDE_DIR}")
set(ThorsAnvil_LIBRARIES
    ThorsAnvil::ThorsLogging
    ThorsAnvil::ThorSerialize
    ThorsAnvil::ThorsCrypto
    ThorsAnvil::ThorsSocket
    ThorsAnvil::ThorsStorage
    ThorsAnvil::ThorsMongo
    ThorsAnvil::Nisse
)
