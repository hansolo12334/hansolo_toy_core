cmake_minimum_required(VERSION 3.20)
project(hansolo_thread_rebuild)

include(./msgs/protoGen.cmake)

# find_package(OpenSSL REQUIRED)
find_package(OpenCV REQUIRED)

file(GLOB_RECURSE all_include_dirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ./include/*)
foreach(dir ${all_include_dirs})
    get_filename_component(dir_path ${dir} PATH)
    include_directories(${dir_path})
endforeach()

file(GLOB_RECURSE all_lib RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ./src/*)
add_library(hansolo_core
    ${all_lib}
    )
target_link_libraries(hansolo_core hw_grpc_proto ${OpenCV_LIBS})



foreach(_target
  hansolocore
  hansolotopic
  test_new_node
  test_new_node1
  test_new_node2
  test_image
  test_image_receive
  test_image_receive2
)
  add_executable(${_target} "./${_target}.cc")
  target_link_libraries(${_target}
    hansolo_core
    hw_grpc_proto
    absl::flags
    absl::flags_parse
    ${_REFLECTION}
    ${_GRPC_GRPCPP}
    ${_PROTOBUF_LIBPROTOBUF}
    pthread
    )
endforeach()


# add_executable(test_image test_image.cc )
# target_link_libraries(test_image absl::flags  hw_grpc_proto
# absl::flags_parse OpenSSL::SSL ${OpenCV_LIBS} ${_PROTOBUF_LIBPROTOBUF})
