cmake_minimum_required(VERSION 2.8)

project( xsens_pc_simple )
#源文件目录,将src目录下的所有源文件的名字保存在变量DIR_SRCS中
aux_source_directory( src DIR_SRCS )
set( CMAKE_BUILD_TYPE "Release" )
set( CMAKE_CXX_FLAGS "-std=c++11 -O3" )
#指定可执行文件的输出文件夹
set( EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin )
#指定库文件的存放位置lib
set( LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib )
#将所有src中的源代码文件编译成一个可执行文件,命名为
add_executable( xsens_pc_uart ${DIR_SRCS} )
