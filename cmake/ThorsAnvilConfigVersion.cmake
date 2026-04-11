# ThorsAnvilConfigVersion.cmake
# ----------------------------
# Version file for find_package(ThorsAnvil ...)
#
# Supports: find_package(ThorsAnvil 9.1 REQUIRED)

set(PACKAGE_VERSION "9.1.11")

if(PACKAGE_VERSION VERSION_LESS PACKAGE_FIND_VERSION)
    set(PACKAGE_VERSION_COMPATIBLE FALSE)
else()
    set(PACKAGE_VERSION_COMPATIBLE TRUE)
    if(PACKAGE_VERSION STREQUAL PACKAGE_FIND_VERSION)
        set(PACKAGE_VERSION_EXACT TRUE)
    endif()
endif()
