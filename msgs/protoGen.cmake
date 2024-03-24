cmake_minimum_required(VERSION 3.20)

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


#寻找msg文件夹下的所有proto文件 自动化生成h和cc文件
file(GLOB_RECURSE all_proto RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ./msgs/*.proto)

set(hw_proto_srcs)
set(hw_proto_hdrs)
set(hw_grpc_srcs )
set(hw_grpc_hdrs)
set(hw_proto_paths)
set(hw_protos)

foreach(_protofile ${all_proto})
get_filename_component(proto_name_we ${_protofile} NAME_WE)

list(APPEND hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name_we}.pb.cc")
list(APPEND hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name_we}.pb.h")
list(APPEND hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name_we}.grpc.pb.cc")
list(APPEND hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/${proto_name_we}.grpc.pb.h")

get_filename_component(hw_proto ${_protofile} ABSOLUTE)
get_filename_component(hw_proto_path "${hw_proto}" PATH)

list(APPEND hw_proto_paths ${hw_proto_path})

list(APPEND hw_protos ${hw_proto})

endforeach(_protofile
)

add_custom_command(
      OUTPUT ${hw_proto_srcs} ${hw_proto_hdrs} ${hw_grpc_srcs} ${hw_grpc_hdrs}
      COMMAND ${_PROTOBUF_PROTOC}
      ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
        --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
        -I ${hw_proto_path}
        --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
        ${hw_protos}
      DEPENDS ${hw_protos})

# hw_grpc_proto
include_directories("${CMAKE_CURRENT_BINARY_DIR}")
add_library(hw_grpc_proto
            ${hw_grpc_srcs}
            ${hw_grpc_hdrs}
            ${hw_proto_srcs}
            ${hw_proto_hdrs}
            # ${hw_proto_srcs1}
            # ${hw_proto_hdrs1}
 )

target_link_libraries(hw_grpc_proto
  ${_REFLECTION}
  ${_GRPC_GRPCPP}
  ${_PROTOBUF_LIBPROTOBUF})





# get_filename_component(hw_proto "./msgs/coreConnection.proto" ABSOLUTE)
# get_filename_component(hw_proto_path "${hw_proto}" PATH)


# set(hw_proto_srcs "${CMAKE_CURRENT_BINARY_DIR}/coreConnection.pb.cc")
# set(hw_proto_hdrs "${CMAKE_CURRENT_BINARY_DIR}/coreConnection.pb.h")
# set(hw_grpc_srcs "${CMAKE_CURRENT_BINARY_DIR}/coreConnection.grpc.pb.cc")
# set(hw_grpc_hdrs "${CMAKE_CURRENT_BINARY_DIR}/coreConnection.grpc.pb.h")


# get_filename_component(hw_proto1 "./msgs/stdMsg.proto" ABSOLUTE)
# get_filename_component(hw_proto_path1 "${hw_proto1}" PATH)



# set(hw_proto_srcs1 "${CMAKE_CURRENT_BINARY_DIR}/stdMsg.pb.cc")
# set(hw_proto_hdrs1 "${CMAKE_CURRENT_BINARY_DIR}/stdMsg.pb.h")


# add_custom_command(
#       OUTPUT "${hw_proto_srcs}" "${hw_proto_hdrs}" "${hw_grpc_srcs}" "${hw_grpc_hdrs}" "${hw_proto_srcs1}" "${hw_proto_hdrs1}"
#       COMMAND ${_PROTOBUF_PROTOC}
#       ARGS --grpc_out "${CMAKE_CURRENT_BINARY_DIR}"
#         --cpp_out "${CMAKE_CURRENT_BINARY_DIR}"
#         -I "${hw_proto_path}"
#         -I "${hw_proto_path1}"
#         --plugin=protoc-gen-grpc="${_GRPC_CPP_PLUGIN_EXECUTABLE}"
#         "${hw_proto}" "${hw_proto1}"
#       DEPENDS "${hw_proto}" "${hw_proto1}")