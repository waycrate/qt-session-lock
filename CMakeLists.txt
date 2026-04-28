cmake_minimum_required(VERSION 3.16)

project(sessionlockqt
    VERSION "2.0.1"
    DESCRIPTION "Session Lock Binding for qt6"
)

set(QT_QML_DIR "/usr/lib/qt6/qml" CACHE STRING "QT plugin dir")

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX /usr)
endif ()

include(GNUInstallDirs)
include(GenerateExportHeader)
include(CMakePackageConfigHelpers)

set(CONFIG_CMAKE_INSTALL_DIR "${CMAKE_INSTALL_LIBDIR}/cmake/SessionLockQt"
    CACHE STRING "Install dir for cmake config files")

set(INCLUDE_DIR "${CMAKE_INSTALL_INCLUDEDIR}/SessionLockQt")

find_package(Qt6 REQUIRED COMPONENTS Core Gui WaylandClient
             WaylandClientPrivate Qml Quick)
find_package(Qt6WaylandScannerTools REQUIRED)

find_package(Wayland 1.3 COMPONENTS Client Server)
find_package(PkgConfig REQUIRED)

add_library(SessionLockQtInterface SHARED)

qt6_generate_wayland_protocol_client_sources(SessionLockQtInterface FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/ext-session-lock-v1.xml
)

target_sources(SessionLockQtInterface PRIVATE
    src/interfaces/window.h
    src/interfaces/window.cpp
    src/interfaces/command.h
    src/interfaces/command.cpp
    src/qwaylandextsessionlocksurface.h
    src/qwaylandextsessionlocksurface.cpp
    src/qwaylandextsessionlockmanagerintegration.h
    src/qwaylandextsessionlockmanagerintegration.cpp
    src/qwaylandextsessionlockintegrationplugin.h
    src/qwaylandextsessionlockintegrationplugin.cpp
)

generate_export_header(SessionLockQtInterface)

target_link_libraries(SessionLockQtInterface PUBLIC
    Qt6::Gui
    Qt6::Core
    Qt6::WaylandClient
    Wayland::Client
)

target_link_libraries(SessionLockQtInterface PRIVATE
    Qt6::WaylandClientPrivate
)

set_target_properties(SessionLockQtInterface PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION}
    SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR}
    EXPORT_NAME Interface
)

target_include_directories(SessionLockQtInterface PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/interfaces>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/SessionLockQt>
)

qt_add_qml_module(SessionLockQtQml
    URI Waycrate.SessionLock
    VERSION 1.0
    SOURCES
        src/qml/SessionLock.h
        src/qml/SessionLock.cpp
)

target_link_libraries(SessionLockQtQml PUBLIC
    Qt6::Quick
    Qt6::WaylandClient
    Qt6::WaylandClientPrivate
    SessionLockQtInterface
)

target_include_directories(SessionLockQtQml PRIVATE
    ${CMAKE_SOURCE_DIR}/src/qml
)
set_target_properties(SessionLockQtQml PROPERTIES
    VERSION ${CMAKE_PROJECT_VERSION}
    SOVERSION ${CMAKE_PROJECT_VERSION_MAJOR}
    EXPORT_NAME QtQml
)

install(TARGETS SessionLockQtQml
        EXPORT SessionLockQtTargets
        BUNDLE DESTINATION .
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(TARGETS SessionLockQtQmlplugin
        DESTINATION ${QT_QML_DIR})

install(TARGETS SessionLockQtInterface
        EXPORT SessionLockQtTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

install(EXPORT SessionLockQtTargets
        FILE SessionLockQtTargets.cmake
        NAMESPACE SessionLockQt::
        DESTINATION ${CONFIG_CMAKE_INSTALL_DIR}
)

configure_package_config_file(cmake/SessionLockQtConfig.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/SessionLockQtConfig.cmake"
    INSTALL_DESTINATION ${CONFIG_CMAKE_INSTALL_DIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SessionLockQtConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/SessionLockQtConfigVersion.cmake
        DESTINATION ${CONFIG_CMAKE_INSTALL_DIR})

set(HEADERS
    src/interfaces/window.h
    src/interfaces/command.h
    ${CMAKE_CURRENT_BINARY_DIR}/sessionlockqtinterface_export.h
)

install(FILES
    ${HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/SessionLockQt COMPONENT Devel
)

add_executable(sessionlock-test test/main.cpp)

target_link_libraries(sessionlock-test PRIVATE
    SessionLockQtInterface
    Qt6::Qml
    Qt6::Gui
)

target_include_directories(sessionlock-test PUBLIC
    src
)

write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/SessionLockQtConfigVersion.cmake"
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/SessionLockQtConfigVersion.cmake
        DESTINATION ${CONFIG_CMAKE_INSTALL_DIR})
