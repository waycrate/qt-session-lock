cmake_minimum_required(VERSION 3.16)

project(sessionlockqt)

set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")

find_package(Qt6 REQUIRED COMPONENTS Core WaylandClient)
find_package(Qt6WaylandScannerTools REQUIRED)

find_package(Wayland 1.3 COMPONENTS Client Server)
find_package(PkgConfig REQUIRED)


pkg_check_modules(XKBCOMMON xkbcommon REQUIRED IMPORTED_TARGET)

add_library(SessionLockQtInterface)

qt6_generate_wayland_protocol_client_sources(SessionLockQtInterface FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/misc/ext-session-lock-v1.xml
)

target_link_libraries(SessionLockQtInterface PUBLIC
  Qt6::Core
)

add_library(sessionlockqt SHARED
  src/qwaylandextsessionlocksurface.h
  src/qwaylandextsessionlocksurface.cpp
  src/qwaylandextsessionlockmanagerintegration.h
  src/qwaylandextsessionlockmanagerintegration.cpp
)

target_link_libraries(sessionlockqt
  Qt6::WaylandClient
  Qt6::WaylandClientPrivate
  Qt6::Core
  SessionLockQtInterface
  PkgConfig::XKBCOMMON
)
