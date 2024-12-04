# 定义生成 protobuf 和 grpc 文件的自定义函数
function(generate_protobuf_and_grpc PROTO_FILE)
    # 检查并设置 protoc 的路径
    find_program(PROTOC_EXECUTABLE protoc REQUIRED)

    # 设置 gRPC 插件的路径
    if(NOT DEFINED grpc_cpp_plugin_location)
        message(FATAL_ERROR "grpc_cpp_plugin_location is not defined.")
    endif()

    # 设置构建目录为生成文件的输出目录
    set(OUT_DIR ${CMAKE_CURRENT_BINARY_DIR})

    # 创建输出目录（如果没有的话）
    file(MAKE_DIRECTORY ${OUT_DIR})

    # 生成 protobuf 文件 (.pb.h 和 .pb.cc)
    execute_process(
        COMMAND ${PROTOC_EXECUTABLE} --cpp_out=${OUT_DIR} ${PROTO_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE PROTOC_RESULT
    )

    if(PROTOC_RESULT)
        message(FATAL_ERROR "Failed to generate protobuf files from ${PROTO_FILE}")
    endif()

    # 生成 grpc 文件 (.grpc.pb.h 和 .grpc.pb.cc)
    execute_process(
        COMMAND ${PROTOC_EXECUTABLE} --grpc_out=${OUT_DIR} --plugin=protoc-gen-grpc=${grpc_cpp_plugin_location} ${PROTO_FILE}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GRPC_RESULT
    )

    if(GRPC_RESULT)
        message(FATAL_ERROR "Failed to generate grpc files from ${PROTO_FILE}")
    endif()

    # 提示成功
    message(STATUS "Successfully generated protobuf and grpc files for ${PROTO_FILE}")
endfunction()
