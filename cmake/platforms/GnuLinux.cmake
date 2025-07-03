# Adapter
target_sources(${BINARY_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Huenicorn/Platforms/GnuLinux/GnuLinuxAdapter.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Platforms/GnuLinux/GnuLinuxAdapter.cpp
)


# Check grabber-related libraries
# Begin X11-related
find_package(X11)
# End X11-related

# Begin Pipewire-related
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")
find_package(Gio)
pkg_search_module(LIB_PIPEWIRE OPTIONAL libpipewire-0.3)
pkg_check_modules(LIBGLIB glib-2.0 gio-2.0)
# End Pipewire-related


if(${X11_FOUND})
  set(X11_GRABBER_AVAILABLE TRUE)
  message("Able to build X11 Grabber !")

  set(X11_GRABBER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Platforms/GnuLinux/X11Grabber.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Huenicorn/Platforms/GnuLinux/X11Grabber.hpp
  )
endif()

if(LIB_PIPEWIRE_FOUND AND GIO_FOUND AND LIBGLIB_FOUND)
  set(PIPEWIRE_GRABBER_AVAILABLE TRUE)
  message("Able to build Pipewire Grabber !")

  set(PIPEWIRE_GRABBER_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Platforms/GnuLinux/PipewireGrabber.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/Platforms/GnuLinux/XdgDesktopPortal.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Huenicorn/Platforms/GnuLinux/PipewireGrabber.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Huenicorn/Platforms/GnuLinux/XdgDesktopPortal.hpp
  )
endif()


if(NOT (PIPEWIRE_GRABBER_AVAILABLE OR X11_GRABBER_AVAILABLE))
  message(FATAL_ERROR "Missing dependencies to compile a least one grabber")
endif()


if(X11_GRABBER_AVAILABLE)
  target_compile_definitions(${BINARY_NAME} PUBLIC X11_GRABBER_AVAILABLE=1)
  target_sources(${BINARY_NAME} PRIVATE
    ${X11_GRABBER_SOURCES}
  )

  target_link_libraries(${BINARY_NAME} PUBLIC
    X11::X11
    X11::Xext
    X11::Xrandr
  )
endif(X11_GRABBER_AVAILABLE)


if(PIPEWIRE_GRABBER_AVAILABLE)
  target_compile_definitions(${BINARY_NAME} PUBLIC PIPEWIRE_GRABBER_AVAILABLE=1)
  target_sources(${BINARY_NAME} PRIVATE
    ${PIPEWIRE_GRABBER_SOURCES}
  )

  target_include_directories(${BINARY_NAME} PUBLIC
    ${LIB_PIPEWIRE_INCLUDE_DIRS}
    ${GIO_INCLUDE_DIRS}
    ${GLIB_INCLUDE_DIRS}
  )

  target_link_libraries(${BINARY_NAME} PUBLIC
    GIO::GIO
    ${LIB_PIPEWIRE_LIBRARIES}
    ${LIBGLIB_LIBRARIES}
  )
endif(PIPEWIRE_GRABBER_AVAILABLE)
