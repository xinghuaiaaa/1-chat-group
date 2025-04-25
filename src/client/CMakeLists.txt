# 所有源文件
aux_source_directory(. SRC_LIST)

# 生成可执行
add_executable(Chatclient ${SRC_LIST})

# 链接库  -- 仅有两个线程, 读取和写入
target_link_libraries(Chatclient pthread )