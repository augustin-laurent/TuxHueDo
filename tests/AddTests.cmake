message("Building tests")

find_package(OpenCV REQUIRED COMPONENTS imgproc opencv_highgui)

add_executable(interpolationTests
  #Huenicorn
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Huenicorn/ImageProcessing.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Huenicorn/Interpolation.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/include/Huenicorn/Logger.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/ImageProcessing.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Interpolation.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/Logger.cpp

  # Test
  ${CMAKE_CURRENT_SOURCE_DIR}/tests/src/interpolationTests.cpp
)

target_include_directories(interpolationTests PUBLIC
  include
)

target_link_libraries(interpolationTests PUBLIC
  ${OpenCV_LIBS}
)