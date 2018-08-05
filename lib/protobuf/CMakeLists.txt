set(Protobuf_INCLUDE_DIR ${third_party_dir}/include)
set(Protobuf_LIBRARY ${third_party_dir}/lib/libprotobuf.so)
set(Protobuf_PROTOC_EXECUTABLE ${third_party_dir}/bin/protoc)

find_package(Protobuf REQUIRED)

include_directories(${PROTOBUF_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")

set(proto_srcs scene.proto)
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${proto_srcs})
#protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS EXPORT_MACRO DLL_EXPORT ${proto_srcs})
protobuf_generate_python(PROTO_PY ${proto_srcs})

message("INC ${PROTOBUF_INCLUDE_DIRS}")
message("LIB ${PROTOBUF_LIBRARIES}")
message("PROTOC ${PROTOBUF_PROTOC_EXECUTABLE}")
message("PROTO_PY=${PROTO_PY}")

set(protobuf_dir "${CMAKE_BINARY_DIR}/lib")
message("protobuf_dir ${protobuf_dir}")

add_library(proto SHARED ${PROTO_SRCS} ${PROTO_HDRS})
target_include_directories(proto PUBLIC
    $<BUILD_INTERFACE:${protobuf_dir}>
)

target_link_libraries(proto ${PROTOBUF_LIBRARIES})

