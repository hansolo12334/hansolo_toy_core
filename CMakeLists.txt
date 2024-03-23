cmake_minimum_required(VERSION 3.20)
project(hansolo_thread_rebuild)

set(PROTOBUF_ROOT "/usr/local/include/protobuf25_1")
set(CMAKE_PREFIX_PATH
    ${CMAKE_PREFIX_PATH}
    ${PROTOBUF_ROOT}/lib/cmake/
)
option(protobuf_MODULE_COMPATIBLE TRUE)
set(_GRPC_GRPCPP gRPC::grpc++)


find_package(Protobuf CONFIG REQUIRED )
message(STATUS "Using protobuf ${Protobuf_VERSION}")
set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_REFLECTION gRPC::grpc++_reflection)
set(_PROTOBUF_PROTOC $<TARGET_FILE:protobuf::protoc>)

find_package(gRPC CONFIG REQUIRED)
message(STATUS "Using gRPC ${gRPC_VERSION}")
set(_GRPC_GRPCPP gRPC::grpc++)
set(_GRPC_CPP_PLUGIN_EXECUTABLE $<TARGET_FILE:gRPC::grpc_cpp_plugin>)
# Proto file


# foreach(_proto_file
#         coreConnection.proto
#         stdMsg.proto)
#     get_filename_component(hw_proto "./msg/${_proto_file}"  ABSOLUTE)
#     get_filename_component(hw_proto_path "${hw_proto}" PATH)

#     set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/${_proto_file}.pb.cc")
#     set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${_proto_file}.pb.h")
#     set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/${_proto_file}.grpc.pb.cc")
#     set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${_proto_file}.grpc.pb.h")

#     add_custom_command(
#       OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}"
#       COMMAND ${_PROTOBUF_PROTOC}
#       ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
#         --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
#         -I "${hw_proto_path}"
#         --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
#         "${hw_proto}"
#       DEPENDS "${hw_proto}")

#     list(APPEND hw_proto_list   ${hw_grpc_srcs}
#                                 ${hw_grpc_hdrs}
#                                 ${hw_proto_srcs}
#                                 ${hw_proto_hdrs})
# endforeach(_proto_file)

get_filename_component(hw_proto "./msgs/coreConnection.proto" ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)


set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/coreConnection.pb.cc")
set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/coreConnection.pb.h")
set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/coreConnection.grpc.pb.cc")
set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/coreConnection.grpc.pb.h")


get_filename_component(hw_proto1 "./msgs/stdMsg.proto" ABSOLUTE)
get_filename_component(hw_proto_path1 "${hw_proto1}" PATH)


set(hw_proto_srcs1 "${CMAKE_CURRENT_BINARY_DIR}/stdMsg.pb.cc")
set(hw_proto_hdrs1 "${CMAKE_CURRENT_BINARY_DIR}/stdMsg.pb.h")



add_custom_command(
      OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}" "${hw_proto_srcs1}" "${hw_proto_hdrs1}"
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I "${hw_proto_path}"
        -I "${hw_proto_path1}"
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        "${hw_proto}" "${hw_proto1}"
      DEPENDS "${hw_proto}" "${hw_proto1}")

# add_custom_command(
# OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}" "${hw_proto_srcs1}" "${hw_proto_hdrs1}" "${hw_grpc_srcs1}" "${hw_grpc_hdrs1}"
# COMMAND ${_PROTOBUF_PROTOC}
# ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
#     --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
#     -I "${hw_proto_path}" "${hw_proto_path1}"
#     --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
#     "${hw_proto}" "${hw_proto1}"
# DEPENDS "${hw_proto}" "${hw_proto1}")





include_directories("${CMAKE_CURRENT_BINARY_DIR}")



# hw_grpc_proto

add_library(hw_grpc_proto
            ${hw_grpc_srcs}
            ${hw_grpc_hdrs}
            ${hw_proto_srcs}
            ${hw_proto_hdrs}
            ${hw_proto_srcs1}
            ${hw_proto_hdrs1}
 )
target_link_libraries(hw_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})

include_directories(./include)
add_library(hansolo_core
    # ./src/hansolo_publisher.cc
    ./src/hansolo_node_new.cc
    ./src/hansolo_master_new.cc
    ./src/hansolo_tcp.cc
    ./src/hansolo_tcp_thread.cc
    ./src/wrap.cc
    )
target_link_libraries(hansolo_core hw_grpc_proto)

foreach(_target
  test_new_core
  test_new_node
  test_new_node1
  test_new_node2
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
    pthread)
endforeach()


# add_executable(test_new_core ./test_new_core.cc )
# target_link_libraries(test_new_core )

# add_executable(test_core ./test_core.cc ${PROTO_SRCS} ${PROTO_HDRS})
# target_link_libraries(test_core
#     hansolo_core
#     ${PROTOBUF_LIBRARY}
#     pthread)

# add_executable(test_node ./test_node.cc ${PROTO_SRCS} ${PROTO_HDRS})
# target_link_libraries(test_node
#     hansolo_core
#     ${PROTOBUF_LIBRARY}
#     pthread)

# add_executable(test_node1 ./test_node1.cc ${PROTO_SRCS} ${PROTO_HDRS})
# target_link_libraries(test_node1
#     hansolo_core
#     ${PROTOBUF_LIBRARY}
#     pthread)
