cmake_minimum_required(VERSION 3.0)
project(main) #工程名

# 配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)

# 配置最终的可执行文件输出的路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 配置 头文件 搜索路径
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/include/server)
include_directories(${PROJECT_SOURCE_DIR}/thirdparty) 
include_directories(${PROJECT_SOURCE_DIR}/include/server/db) #数据层头文件
include_directories(${PROJECT_SOURCE_DIR}/include/server/model) #数据层头文件
include_directories(${PROJECT_SOURCE_DIR}/include/server/redis) #redis服务头文件

# 加载子目录  src   既然进去, 就有 CMakeLists.txt
add_subdirectory(src)

