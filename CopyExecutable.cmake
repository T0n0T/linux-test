# 假设传入的参数为 TARGET_NAME
function(copy_executable TARGET_NAME)
    # 定义输出文件夹路径
    set(OUTPUT_DIR ${CMAKE_SOURCE_DIR}/bin)

    # 确保输出目录存在
    file(MAKE_DIRECTORY ${OUTPUT_DIR})

    # 在构建后复制文件到指定目录
    add_custom_command(
        TARGET ${TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${TARGET_NAME}> ${OUTPUT_DIR}
        COMMENT "Copying ${TARGET_NAME} to output directory."
    )
endfunction(copy_executable)
