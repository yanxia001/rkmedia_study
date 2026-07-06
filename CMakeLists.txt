
cmake_minimum_required(VERSION 3.8)
message(STATUS "cmake version ${CMAKE_VERSION}")


#此处第一个需修改为自己的路径
set(CMAKE_HOME /home/hxy/)
set(CMAKE_SDK ${CMAKE_HOME}/RV1126_RV1109_LINUX_SDK_V2.2.5.1_20231011/)

set(CMAKE_BUILD ${CMAKE_SDK}/buildroot/output/rockchip_rv1126_rv1109_spi_nand)
set(TOOLCHAIN_DIR ${CMAKE_SDK}/buildroot/output/rockchip_rv1126_rv1109_spi_nand/host/)


set(CMAKE_CXX_COMPILER ${TOOLCHAIN_DIR}/bin/arm-linux-gnueabihf-g++)
set(CMAKE_C_COMPILER ${TOOLCHAIN_DIR}/bin/arm-linux-gnueabihf-gcc)
set(SYSROOT ${TOOLCHAIN_DIR}/arm-buildroot-linux-gnueabihf/sysroot/usr/include)
set(CMAKE_SYSROOT ${TOOLCHAIN_DIR}/arm-buildroot-linux-gnueabihf/sysroot)


set(CMAKE_CXX_STANDARD 11)
add_definitions(-g -O0 -ggdb -gdwarf -funwind-tables -rdynamic)
add_definitions(-Wno-write-strings -Wno-return-type)

set(OPENCV_LIBS opencv_core opencv_imgcodecs opencv_imgproc opencv_features2d opencv_flann opencv_highgui )
set(FFMPEG_LIBS avformat avutil avcodec avdevice fdk-aac)
set(RKMEDIA_LIBS easymedia rga rkaiq rockx)


set(THIRD_LIBS third_media)
link_directories(include ${CMAKE_HOME}/rv1126-rtk/external/rkmedia/src/rkaudio)

link_directories(include ${CMAKE_SDK}/external/rkmedia/examples/librtsp)
link_directories(include ${CMAKE_BUILD}/target/usr/lib)

link_directories(include ${CMAKE_BUILD}/target/oem/usr/lib)#for librockx
#for opencv
link_directories(include ${CMAKE_BUILD}/build/opencv3-3.4.12/buildroot-build/lib)



include_directories(.)
include_directories(./librtsp)
include_directories(${SYSROOT})
include_directories(${SYSROOT}/rga)
include_directories(${SYSROOT}/easymedia)
include_directories(${SYSROOT}/rkaiq/uAPI)
include_directories(${SYSROOT}/rkaiq/xcore)
include_directories(${SYSROOT}/rkaiq/algos)
include_directories(${SYSROOT}/rkaiq/common)
include_directories(${SYSROOT}/rkaiq/iq_parser)
include_directories(${CMAKE_SDK}/external/rockx/sdk/rockx-rv1109-Linux/include)
include_directories(${CMAKE_BUILD}/build/opencv3-3.4.12/modules/imgproc/include/opencv2)
include_directories(${CMAKE_BUILD}/build/freetype-2.10.1/include/freetype)
include_directories(${CMAKE_SDK}/external/rkmedia/examples)
include_directories(${CMAKE_BUILD}build/ffmpeg-4.1.3)


include_directories(${CMAKE_BUILD}/build/opencv3-3.4.12)


include_directories(./include)
#project(xyd)
add_definitions(-DRKAIQ)
aux_source_directory (src SRC_LIST)
aux_source_directory (${CMAKE_SDK}/external/rkmedia/examples/common COMMON)
#最终就会生成执行文件 mymain
add_executable(mymain ${SRC_LIST} ${COMMON})
#target_link_libraries(main ${OPENCV_LIBS} ${RKMEDIA_LIBS} ${THIRD_LIBS} pthread dl librtsp.a) 
target_link_libraries(mymain ${OPENCV_LIBS} ${FFMPEG_LIBS} ${RKMEDIA_LIBS} ${THIRD_LIBS} pthread dl librtsp.a ) 
