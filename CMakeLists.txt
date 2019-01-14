#CMake最低版本：3.0
cmake_minimum_required(VERSION 3.0)
#项目名称:MLParser
project(MLParser)
#指定C++标准为C++11
set(CMAKE_CXX_STANDARD 11)
#搜索所有源文件
file(GLOB C_SOURCES src/*.c)
file(GLOB CPP_SOURCES src/*.cpp)
#指定目标类型为静态库
add_library(${PROJECT_NAME} STATIC ${C_SOURCES} ${CPP_SOURCES})
#指定头文件搜索目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
#安装设置
install(TARGETS ${PROJECT_NAME} DESTINATION lib)
#搜索所有头文件
file(GLOB C_HEADERS include/*.h)
file(GLOB CPP_HEADERS include/*.hpp)
#安装设置
install(FILES ${C_HEADERS} DESTINATION include)
install(FILES ${CPP_HEADERS} DESTINATION include)