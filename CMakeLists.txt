cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME huenicorn)
project(${PROJECT_NAME} VERSION 1.0.12)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Build options
option(BUILD_QT "Build Qt6 frontend" ON)
option(BUILD_GTK "Build GTK4 frontend" OFF)
option(BUILD_DOCUMENTATION "Build documentation" OFF)
option(BUILD_TESTS "Build tests" OFF)

# Check mandatory libraries
find_package(PkgConfig REQUIRED)
find_package(OpenCV REQUIRED COMPONENTS imgproc)
find_package(CURL REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
find_library(MBEDTLS_LIBS mbedtls REQUIRED)
find_library(MBEDTLS_X509 mbedx509 REQUIRED)
find_library(MBEDTLS_CRYPTO mbedcrypto REQUIRED)

# Generate version header
set(VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/include/Huenicorn/Version.hpp")

add_custom_target(generate_version_header DEPENDS ${VERSION_HEADER})

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/config/Version.hpp.in
  ${VERSION_HEADER}
)

# ============================================================================
# libhuenicorn - Core library
# ============================================================================

add_library(huenicorn_core STATIC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ApiTools.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Channel.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Config.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Credentials.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/CurlHttpClient.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/DtlsClient.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/DummyGrabber.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/EntertainmentConfiguration.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/EntertainmentConfigurationSelector.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HttpRequestUtils.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/HuenicornCore.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ImageProcessing.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Interpolation.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Logger.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/PlatformSelector.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Streamer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/TickSynchronizer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/UV.cpp
  ${VERSION_HEADER}
)

add_dependencies(huenicorn_core generate_version_header)

# Platform-specific sources for core
if(UNIX AND NOT APPLE)
  include(cmake/platforms/GnuLinux.cmake)
endif()

target_include_directories(huenicorn_core PUBLIC
  ${OpenCV_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/include
  ${CMAKE_CURRENT_BINARY_DIR}/include
)

target_link_libraries(huenicorn_core PUBLIC
  ${OpenCV_LIBS}
  ${CURL_LIBRARIES}
  ${MBEDTLS_LIBS}
  ${MBEDTLS_X509}
  ${MBEDTLS_CRYPTO}
  nlohmann_json::nlohmann_json
)

target_compile_options(huenicorn_core PRIVATE
  -Wall
  -Wextra
  -Wnon-virtual-dtor
  -pedantic
  -Wno-deprecated-declarations
  -Wno-deprecated-copy
)

# ============================================================================
# Qt6 Frontend
# ============================================================================

if(BUILD_QT)
  find_package(Qt6 REQUIRED COMPONENTS Widgets)

  add_executable(huenicorn-qt
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qt/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qt/MainWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qt/ScreenPartitionWidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qt/ChannelListWidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qt/SettingsDialog.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qt/TrayIcon.cpp
  )

  target_include_directories(huenicorn-qt PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/qt
  )

  target_link_libraries(huenicorn-qt PRIVATE
    huenicorn_core
    Qt6::Widgets
  )

  target_compile_options(huenicorn-qt PRIVATE
    -Wall -Wextra -pedantic
  )
endif()

# ============================================================================
# GTK4 Frontend
# ============================================================================

if(BUILD_GTK)
  pkg_check_modules(GTK4 REQUIRED gtk4)
  pkg_check_modules(ADWAITA REQUIRED libadwaita-1)

  add_executable(huenicorn-gtk
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gtk/main.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gtk/AppState.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gtk/MainWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gtk/ScreenPartitionWidget.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gtk/SettingsDialog.cpp
  )

  target_include_directories(huenicorn-gtk PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/gtk
    ${GTK4_INCLUDE_DIRS}
    ${ADWAITA_INCLUDE_DIRS}
  )

  target_link_libraries(huenicorn-gtk PRIVATE
    huenicorn_core
    ${GTK4_LIBRARIES}
    ${ADWAITA_LIBRARIES}
  )
endif()

# ============================================================================
# Documentation
# ============================================================================

if(BUILD_DOCUMENTATION)
  find_package(Doxygen REQUIRED dot OPTIONAL_COMPONENTS mscgen dia)
  set(doxyfile_in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile.doxygen)
  set(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/doc)
  set(DOXYGEN_EXTRACT_ALL YES)
  set(DOXYGEN_FULL_PATH_NAMES NO)
  doxygen_add_docs(doxygen ${PROJECT_SOURCE_DIR} COMMENT "Generate man pages")
endif()
