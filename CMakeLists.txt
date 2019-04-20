cmake_minimum_required(VERSION 3.8)
project(adriconf)

set(CMAKE_CXX_STANDARD 14)

option(ENABLE_XWAYLAND "Enable XWayland support" ON)

set(SOURCE_FILES Device.cpp Device.h
        DriverOption.cpp DriverOption.h
        Section.cpp Section.h
        Parser.cpp Parser.h
        Application.cpp Application.h
        ConfigurationResolver.cpp ConfigurationResolver.h
        DRIQuery.cpp DRIQuery.h
        HelpersWayland.cpp HelpersWayland.h
        DriverConfiguration.cpp DriverConfiguration.h
        Writer.cpp Writer.h GUI.cpp GUI.h ConfigurationLoader.cpp ConfigurationLoader.h ApplicationOption.cpp ApplicationOption.h
        resources.c GPUInfo.cpp GPUInfo.h PCIDatabaseQuery.cpp PCIDatabaseQuery.h ComboBoxColumn.h DriverOptionType.h)

set(SOURCE_TESTS tests/ConfigurationResolverTest.cpp
                 tests/DeviceTest.cpp
                 tests/ApplicationTest.cpp
                 tests/ApplicationOptionTest.cpp
                 tests/DriverConfigurationTest.cpp
                 tests/DriverOptionTest.cpp
                 tests/SectionTest.cpp
                 tests/GPUInfoTest.cpp
)

find_package(PkgConfig REQUIRED)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

# GTKMM
pkg_check_modules(GTKMM REQUIRED gtkmm-3.0)

# BOOST
find_package(Boost 1.60 REQUIRED COMPONENTS locale)

# LIBXML
pkg_check_modules(LibXML++ libxml++-3.0)
if(NOT LibXML++_FOUND)
    pkg_check_modules(LibXML++2 REQUIRED libxml++-2.6)
    set(LibXML++_INCLUDE_DIRS ${LibXML++2_INCLUDE_DIRS})
    set(LibXML++_LIBRARIES ${LibXML++2_LIBRARIES})
endif()

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
#Use these libs if running on Wayland
if(ENABLE_XWAYLAND)
    pkg_check_modules(EGL REQUIRED egl)
endif()

#INTL INSTALL TRANSLATIONS
GETTEXT_CREATE_TRANSLATIONS(po/adriconf.pot ALL po/en.po po/hr.po po/pt_BR.po)

# Define the executable itself
add_executable(adriconf main.cpp ${SOURCE_FILES})

target_link_libraries(adriconf ${GTKMM_LIBRARIES})
target_include_directories(adriconf PUBLIC ${GTKMM_INCLUDE_DIRS})

target_link_libraries(adriconf ${Boost_LOCALE_LIBRARIES})
target_include_directories(adriconf PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(adriconf ${LibXML++_LIBRARIES})
target_include_directories(adriconf PUBLIC ${LibXML++_INCLUDE_DIRS})

target_link_libraries(adriconf ${X11_LIBRARIES})
target_include_directories(adriconf PUBLIC ${X11_INCLUDE_DIRS})

target_link_libraries(adriconf ${DRM_LIBRARIES})
target_include_directories(adriconf PUBLIC ${DRM_INCLUDE_DIRS})

target_link_libraries(adriconf ${PCILIB_LIBRARIES})
target_include_directories(adriconf PUBLIC ${PCILIB_INCLUDE_DIRS})

target_link_libraries(adriconf ${OPENGL_gl_LIBRARY})
if(OpenGL_GLX_FOUND)
    target_link_libraries(adriconf ${OPENGL_glx_LIBRARY})
endif()

if (ENABLE_XWAYLAND)
    target_link_libraries(adriconf ${EGL_LIBRARIES})
    target_include_directories(adriconf PUBLIC ${EGL_INCLUDE_DIRS})
    target_compile_definitions(adriconf PRIVATE ENABLE_XWAYLAND)
endif()

add_custom_command(OUTPUT ${CMAKE_SOURCE_DIR}/resources.c
    COMMAND glib-compile-resources adriconf.gresource.xml --target=resources.c --generate-source
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    DEPENDS  ${CMAKE_SOURCE_DIR}/DriConf.glade
)
set_source_files_properties(${CMAKE_SOURCE_DIR}/resources.c PROPERTIES
    GENERATED TRUE
)

# GTEST
enable_testing()
find_package(GTest REQUIRED)

add_executable(runUnitTests EXCLUDE_FROM_ALL ${SOURCE_FILES} ${SOURCE_TESTS})
target_link_libraries(runUnitTests GTest::GTest GTest::Main)

target_link_libraries(runUnitTests ${GTKMM_LIBRARIES})
target_include_directories(runUnitTests PUBLIC ${GTKMM_INCLUDE_DIRS})

target_link_libraries(runUnitTests ${Boost_LOCALE_LIBRARIES})
target_include_directories(runUnitTests PUBLIC ${Boost_INCLUDE_DIRS})

target_link_libraries(runUnitTests ${LibXML++_LIBRARIES})
target_include_directories(runUnitTests PUBLIC ${LibXML++_INCLUDE_DIRS})

target_link_libraries(runUnitTests ${X11_LIBRARIES})
target_include_directories(runUnitTests PUBLIC ${X11_INCLUDE_DIRS})

target_link_libraries(runUnitTests ${DRM_LIBRARIES})
target_include_directories(runUnitTests PUBLIC ${DRM_INCLUDE_DIRS})

target_link_libraries(runUnitTests ${PCILIB_LIBRARIES})
target_include_directories(runUnitTests PUBLIC ${PCILIB_INCLUDE_DIRS})

target_link_libraries(runUnitTests ${OPENGL_gl_LIBRARY})
if(OpenGL_GLX_FOUND)
    target_link_libraries(runUnitTests ${OPENGL_glx_LIBRARY})
endif()

if (ENABLE_XWAYLAND)
    target_link_libraries(runUnitTests ${EGL_LIBRARIES})
    target_include_directories(runUnitTests PUBLIC ${EGL_INCLUDE_DIRS})
    target_compile_definitions(runUnitTests PRIVATE ENABLE_XWAYLAND)
endif()

install(TARGETS adriconf DESTINATION bin)
