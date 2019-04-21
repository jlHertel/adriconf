cmake_minimum_required(VERSION 3.8)
project(adriconf)

set(CMAKE_CXX_STANDARD 14)

option(ENABLE_XWAYLAND "Enable XWayland support" ON)
set(TOP_CMAKE_WAS_CALLED TRUE)

find_package(PkgConfig REQUIRED)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

# GTKMM
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

# BOOST
find_package(Boost 1.60 REQUIRED COMPONENTS locale filesystem)

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
        ${CMAKE_SOURCE_DIR}/adriconf/Device.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/Device.h
        ${CMAKE_SOURCE_DIR}/adriconf/DriverOption.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/DriverOption.h
        ${CMAKE_SOURCE_DIR}/adriconf/Section.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/Section.h
        ${CMAKE_SOURCE_DIR}/adriconf/Parser.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/Parser.h
        ${CMAKE_SOURCE_DIR}/adriconf/Application.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/Application.h
        ${CMAKE_SOURCE_DIR}/adriconf/ConfigurationResolver.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/ConfigurationResolver.h
        ${CMAKE_SOURCE_DIR}/adriconf/DRIQuery.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/DRIQuery.h
        ${CMAKE_SOURCE_DIR}/adriconf/HelpersWayland.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/HelpersWayland.h
        ${CMAKE_SOURCE_DIR}/adriconf/DriverConfiguration.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/DriverConfiguration.h
        ${CMAKE_SOURCE_DIR}/adriconf/Writer.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/Writer.h
        ${CMAKE_SOURCE_DIR}/adriconf/GUI.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/GUI.h
        ${CMAKE_SOURCE_DIR}/adriconf/ConfigurationLoader.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/ConfigurationLoader.h
        ${CMAKE_SOURCE_DIR}/adriconf/ApplicationOption.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/ApplicationOption.h
        ${CMAKE_SOURCE_DIR}/adriconf/GPUInfo.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/GPUInfo.h
        ${CMAKE_SOURCE_DIR}/adriconf/PCIDatabaseQuery.cpp
        ${CMAKE_SOURCE_DIR}/adriconf/PCIDatabaseQuery.h
        ${CMAKE_SOURCE_DIR}/adriconf/ComboBoxColumn.h
        ${CMAKE_SOURCE_DIR}/adriconf/DriverOptionType.h
        )

add_subdirectory(adriconf)
add_subdirectory(tests)
