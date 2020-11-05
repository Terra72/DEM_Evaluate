cmake_minimum_required(VERSION 3.0.0)
project(dem_eval VERSION 0.1.0)
find_package( OpenCV REQUIRED )

include(CTest)
enable_testing()

add_executable(dem_eval main.cpp)
target_link_libraries( dem_eval ${OpenCV_LIBS} ) 
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
