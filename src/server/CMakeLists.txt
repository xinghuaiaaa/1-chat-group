# 所有源文件
aux_source_directory(. SRC_LIST)
# 数据库层步骤 需要添加
aux_source_directory(./db DB_LIST)
#数据层源文件
aux_source_directory(./model MODEL_LIST)
# 生成可执行
add_executable(Chatserver ${SRC_LIST} ${DB_LIST} ${MODEL_LIST})

# 链接库
target_link_libraries(Chatserver muduo_net muduo_base pthread mysqlclient)