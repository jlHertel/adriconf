cmake_minimum_required(VERSION 3.8)
project(adriconf)

set(CMAKE_CXX_STANDARD 14)

option(ENABLE_XWAYLAND "Enable XWayland support" ON)
option(ENABLE_UNIT_TESTS "Enables building Unit tests" ON)
set(TOP_CMAKE_WAS_CALLED TRUE)

find_package(PkgConfig REQUIRED)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

# GTKMM
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

# BOOST
find_package(Boost 1.60 REQUIRED COMPONENTS locale filesystem system)

# LIBXML
pkg_check_modules(LibXML++ libxml++-3.0)
if (NOT LibXML++_FOUND)
    pkg_check_modules(LibXML++2 REQUIRED libxml++-2.6)
    set(LibXML++_INCLUDE_DIRS ${LibXML++2_INCLUDE_DIRS})
    set(LibXML++_LIBRARIES ${LibXML++2_LIBRARIES})
endif ()

# X11
pkg_check_modules(X11 REQUIRED x11)

# LIBDRM
pkg_check_modules(DRM REQUIRED libdrm)

# LIBPCI
pkg_check_modules(PCILIB REQUIRED libpci)

# INTL
find_package(Intl REQUIRED)
find_package(Gettext REQUIRED)
include_directories(${INTL_INCLUDE_DIRS})
link_directories(${INTL_LIBRARY_DIRS})

#EGL
if (ENABLE_XWAYLAND)
    pkg_check_modules(EGL REQUIRED egl)
endif ()

#INTL INSTALL TRANSLATIONS
GETTEXT_CREATE_TRANSLATIONS(po/adriconf.pot ALL po/en.po po/hr.po po/pt_BR.po)

set(SHARED_SOURCE_FILES
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/Device.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/Device.h
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/DriverOption.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/DriverOption.h
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/Section.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/Section.h
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/DriverConfiguration.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/DriverConfiguration.h
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/GPUInfo.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/GPUInfo.h
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/Application.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/Application.h
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/ApplicationOption.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/ApplicationOption.h
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/ComboBoxColumn.h
        ${CMAKE_SOURCE_DIR}/adriconf/ValueObject/DriverOptionType.h

        ${CMAKE_SOURCE_DIR}/adriconf/Logging/LoggerInterface.h
        ${CMAKE_SOURCE_DIR}/adriconf/Logging/LoggerLevel.h

        ${CMAKE_SOURCE_DIR}/adriconf/Utils/ParserInterface.h
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/ConfigurationResolverInterface.h
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/DRIQuery.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/DRIQuery.h
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/HelpersWayland.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/HelpersWayland.h
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/WriterInterface.h
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/ConfigurationLoaderInterface.h
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/PCIDatabaseQuery.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/PCIDatabaseQuery.h
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/PCIDatabaseQueryInterface.h

        ${CMAKE_SOURCE_DIR}/adriconf/Utils/Parser.h
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/Parser.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/ConfigurationResolver.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/ConfigurationResolver.h
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/Writer.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/Writer.h
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/ConfigurationLoader.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/Utils/ConfigurationLoader.h

        ${CMAKE_SOURCE_DIR}/adriconf/GUI.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/GUI.h
        )

# GTest and GMock setup
if (ENABLE_UNIT_TESTS)
    configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
    execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
            WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download" )
    execute_process(COMMAND "${CMAKE_COMMAND}" --build .  WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download" )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    add_subdirectory("${CMAKE_BINARY_DIR}/googletest-src" "${CMAKE_BINARY_DIR}/googletest-build")
    add_subdirectory(tests)
endif()

add_subdirectory(adriconf)

