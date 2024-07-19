#!/bin/bash

# 帮助函数
usage() {
    echo "用法: $0 <目标目录> <原后缀> <新后缀> [<原后缀> <新后缀> ...]"
    echo ""
    echo "参数说明:"
    echo "  <目标目录>   指定要搜索和重命名文件的目录."
    echo "  <原后缀>     要替换的文件后缀，不包括句点（.）."
    echo "  <新后缀>     用于替换的后缀，不包括句点（.）."
    echo "  [<原后缀> <新后缀>]   可选，您可以一次性替换多个后缀."
    echo ""
    echo "示例:"
    echo "  $0 /path/to/dir txt md"
    echo "  $0 /path/to/dir txt md csv txt"
    echo "  # 将目录下所有 .txt 文件替换为 .md, 并且将所有 .csv 文件替换为 .txt"
    exit 1
}

# 检查参数数量
if [ "$#" -lt 3 ]; then
    usage
fi

# 获取输入参数
TARGET_DIR=$1
shift # 移除第一个参数
LOG_FILE="rename_log.txt"

# 确保目标目录存在
if [ ! -d "$TARGET_DIR" ]; then
    echo "错误: 目录 $TARGET_DIR 不存在."
    exit 1
fi

# 清空或创建日志文件
echo "重命名操作日志 - $(date)" > "$LOG_FILE"

# 循环处理每对后缀
while [ "$#" -gt 1 ]; do
    OLD_SUFFIX=$1
    NEW_SUFFIX=$2
    shift 2 # 移动到下一个后缀对

    # 使用find命令寻找并替换后缀
    find "$TARGET_DIR" -type f -name "*.$OLD_SUFFIX" | while read -r file; do
        # 生成新的文件名
        new_file="${file%.$OLD_SUFFIX}.$NEW_SUFFIX"

        # 重命名文件
        if mv "$file" "$new_file"; then
            echo "已将 $file 重命名为 $new_file" | tee -a "$LOG_FILE"
        else
            echo "错误: 无法重命名 $file" | tee -a "$LOG_FILE"
        fi
    done
done

echo "后缀名替换完成. 详细日志请查看 $LOG_FILE."