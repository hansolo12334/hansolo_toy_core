cmake_minimum_required(VERSION 2.20)
project(test_image)
set(CMAKE_CXX_STANDARD 17)


find_package(OpenSSL REQUIRED)
find_package(OpenCV REQUIRED)
add_executable(test ./test.cc)
target_link_libraries(test OpenSSL::SSL ${OpenCV_LIBS})