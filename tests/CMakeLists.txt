# Check if we are running from inside main config
if (NOT TOP_CMAKE_WAS_CALLED)
    message(FATAL_ERROR "
    You did not 'cmake' the top CMakeLists.txt file. Use the one in the top dir.
    It is advice to delete all wrongly generated cmake stuff => CMakeFiles & CMakeCache.txt")
endif ()

set(SOURCE_TESTS
        ${CMAKE_SOURCE_DIR}/tests/ConfigurationResolverTest.cpp
        ${CMAKE_SOURCE_DIR}/tests/DeviceTest.cpp
        ${CMAKE_SOURCE_DIR}/tests/ApplicationTest.cpp
        ${CMAKE_SOURCE_DIR}/tests/ApplicationOptionTest.cpp
        ${CMAKE_SOURCE_DIR}/tests/DriverConfigurationTest.cpp
        ${CMAKE_SOURCE_DIR}/tests/DriverOptionTest.cpp
        ${CMAKE_SOURCE_DIR}/tests/SectionTest.cpp
        ${CMAKE_SOURCE_DIR}/tests/GPUInfoTest.cpp
        )

# GTEST
enable_testing()
find_package(GTest REQUIRED)

add_executable(runUnitTests EXCLUDE_FROM_ALL ${SHARED_SOURCE_FILES} ${SOURCE_TESTS})
target_link_libraries(runUnitTests GTest::GTest GTest::Main)

target_link_libraries(runUnitTests ${GTKMM_LIBRARIES})
target_include_directories(runUnitTests PUBLIC ${GTKMM_INCLUDE_DIRS})

target_link_libraries(runUnitTests ${Boost_LOCALE_LIBRARIES})
target_link_libraries(runUnitTests ${Boost_FILESYSTEM_LIBRARIES})
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
if (OpenGL_GLX_FOUND)
    target_link_libraries(runUnitTests ${OPENGL_glx_LIBRARY})
endif ()

if (ENABLE_XWAYLAND)
    target_link_libraries(runUnitTests ${EGL_LIBRARIES})
    target_include_directories(runUnitTests PUBLIC ${EGL_INCLUDE_DIRS})
    target_compile_definitions(runUnitTests PRIVATE ENABLE_XWAYLAND)
endif ()

target_compile_options(runUnitTests PRIVATE -Wall -Wextra -pedantic)