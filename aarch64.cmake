# 设置交叉编译器的位置

# set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++) 
# set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc)

set(CMAKE_CXX_COMPILER aarch64-linux-gnu-g++-9) 
set(CMAKE_C_COMPILER aarch64-linux-gnu-gcc-9)

# set(CMAKE_CXX_COMPILER
#     /home/yu/toolchains/gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-g++
# )
# set(CMAKE_C_COMPILER
#     /home/yu/toolchains/gcc-arm-9.2-2019.12-x86_64-aarch64-none-linux-gnu/bin/aarch64-none-linux-gnu-gcc
# )

# set(CMAKE_CXX_COMPILER
#     /opt/gcc-aarch64-linux-gnu/bin/aarch64-linux-gnu-g++
# )
# set(CMAKE_C_COMPILER
#     /opt/gcc-aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
# )

set(CMAKE_IGNORE_PATH /usr/include/ /usr/local/include/)
set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath,/home/yu/aarch64/ffmpeg/lib/,-rpath,/home/yu/aarch64/x264/lib/"
)

# 指定目标系统的类型
set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR aarch64)

# set(MY_SYSROOT /home/yu/rk3566-sysroot) set(CMAKE_SYSROOT
# /opt/gcc-aarch64-linux-gnu/aarch64-linux-gnu/libc/)

set(Qt5_DIR /home/yu/aarch64/qt6.5.3-arm-linux/lib/cmake/Qt6/)

# set(QT_DIR /usr/local/Qt-6.5.3/lib/cmake/Qt6/) set(Qt6_DIR
# /usr/local/Qt-6.5.3/lib/cmake/Qt6/) set(Qt6CoreTools_DIR
# /usr/local/Qt-6.5.3/lib/cmake/Qt6CoreTools/) set(Qt6GuiTools_DIR
# /usr/local/Qt-6.5.3/lib/cmake/Qt6GuiTools/)

set(onnxruntime_DIR
    /home/yu/codefield/stm32_host_yolo/onnxruntime-linux-aarch64-1.19.2/lib64/cmake/onnxruntime/
)
# set(Qt5_DIR /home/yu/aarch64/qt5.12.10-arm/lib/cmake/Qt5/)

set(x264_LIBS /home/yu/aarch64/x264/lib/libx264.so)

set(OPENGL_opengl_LIBRARY
    ${MY_SYSROOT}/usr/lib/aarch64-linux-gnu/libEGL.so
    ${MY_SYSROOT}/usr/lib/aarch64-linux-gnu/libGLdispatch.so
    ${MY_SYSROOT}/usr/lib/aarch64-linux-gnu/libGLESv1_CM.so
    ${MY_SYSROOT}/usr/lib/aarch64-linux-gnu/libGLESv2.so
    ${MY_SYSROOT}/usr/lib/aarch64-linux-gnu/libGL.so
    ${MY_SYSROOT}/usr/lib/aarch64-linux-gnu/libOpenGL.so)

set(OPENGL_glx_LIBRARY /usr/lib/aarch64-linux-gnu/libGLX.so)

set(OPENGL_INCLUDE_DIR ${MY_SYSROOT}/usr/include)

set(FFMPEG_LIBS
    /home/yu/aarch64/ffmpeg/lib/libavcodec.so
    /home/yu/aarch64/ffmpeg/lib/libpostproc.so
    /home/yu/aarch64/ffmpeg/lib/libswresample.so
    /home/yu/aarch64/ffmpeg/lib/libswscale.so
    /home/yu/aarch64/ffmpeg/lib/libavutil.so
    /home/yu/aarch64/ffmpeg/lib/libavdevice.so
    /home/yu/aarch64/ffmpeg/lib/libavfilter.so
    /home/yu/aarch64/ffmpeg/lib/libavformat.so)

set(CMAKE_FIND_ROOT_PATH
    ${MY_SYSROOT}
    /home/yu/aarch64/spdlog/lib/cmake/spdlog/
    /home/yu/aarch64/grpc/lib/cmake/grpc/
    /home/yu/aarch64/grpc/lib/cmake/absl/
    /home/yu/aarch64/grpc/lib/cmake/protobuf/
    /home/yu/aarch64/grpc/lib/cmake/re2/
    /home/yu/aarch64/grpc/lib/cmake/c-ares/
    /home/yu/aarch64/grpc/lib/cmake/utf8_range/
    /home/yu/aarch64/ffmpeg
    /home/yu/aarch64/opencv/lib/cmake/opencv4/
    /home/yu/aarch64/qt5.12.10-arm/lib/cmake/Qt5
    /home/yu/aarch64/fmt/lib/cmake/fmt/
    /home/yu/codefield/stm32_host_yolo/onnxruntime-linux-aarch64-1.19.2/lib64/cmake/onnxruntime/
)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY BOTH)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE BOTH)
