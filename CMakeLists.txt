cmake_minimum_required(VERSION 3.16)

project(sessionlockqt)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui WaylandClient Qml)
find_package(Qt6WaylandScannerTools REQUIRED)

find_package(Wayland 1.3 COMPONENTS Client Server)
find_package(PkgConfig REQUIRED)


pkg_check_modules(XKBCOMMON xkbcommon REQUIRED IMPORTED_TARGET)

add_library(SessionLockQtInterface SHARED)

qt6_generate_wayland_protocol_client_sources(SessionLockQtInterface FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/misc/ext-session-lock-v1.xml
)

target_sources(SessionLockQtInterface PRIVATE
    src/interfaces/window.h
    src/interfaces/window.cpp
    src/interfaces/shell.h
    src/interfaces/shell.cpp
)

target_link_libraries(SessionLockQtInterface PUBLIC Qt6::Gui Qt6::Core)
target_link_libraries(SessionLockQtInterface PRIVATE Qt6::WaylandClientPrivate Wayland::Client PkgConfig::XKBCOMMON)

add_library(sessionlockqt SHARED
    src/qwaylandextsessionlocksurface.h
    src/qwaylandextsessionlocksurface.cpp
    src/qwaylandextsessionlockmanagerintegration.h
    src/qwaylandextsessionlockmanagerintegration.cpp
    src/qwaylandextsessionlockintegrationplugin.h
    src/qwaylandextsessionlockintegrationplugin.cpp
)

target_link_libraries(sessionlockqt
    Qt6::WaylandClient
    Qt6::WaylandClientPrivate
    Qt6::Core
    SessionLockQtInterface
    PkgConfig::XKBCOMMON
)

set_target_properties(sessionlockqt PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/wayland-shell-integration"
)

target_include_directories(sessionlockqt PUBLIC
    src/interfaces
)

add_executable(sessionlock-test test/main.cpp)
target_link_libraries(sessionlock-test PRIVATE SessionLockQtInterface Qt6::Qml Qt6::Gui Wayland::Client Qt6::WaylandClientPrivate)
target_include_directories(sessionlock-test PUBLIC
    src
)
