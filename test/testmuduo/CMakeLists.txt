
# 配置头文件搜索路径
include_directories()
# 配置库文件搜索路径
link_directories()

#设置需要编译的 源文件列表
set(SRC_LIST ./nuduo_server.cpp)

# 设置可执行文件最终目录
# set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

# 设置库文件最终目录
# set(LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

#把 . 指定路径下的 所有源文件名字 放入变量名--不用手动一个个输入
# aux_source_directory(. SRC_LIST) #暂时不用

#生成可执行文件
add_executable(server ${SRC_LIST})

# 表示server 这个目标程序 需要链接的 库
target_link_libraries(server muduo_net muduo_base pthread)
