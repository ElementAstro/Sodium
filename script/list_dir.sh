
#!/bin/bash
 
##递归遍历
traverse_dir()
{
    filepath=$1
    
    for file in `ls -a $filepath`
    do
        if [ -d ${filepath}/$file ]
        then
            if [[ $file != '.' && $file != '..' ]]
            then
                #递归
                traverse_dir ${filepath}/$file
            fi
        else
            #调用查找指定后缀文件
            check_suffix ${filepath}/$file
        fi
    done
}
 
 
##获取后缀为txt或ini的文件
check_suffix()
{
    file=$1
    
    if [ "${file##*.}"x = "cpp"x ] || [ "${file##*.}"x = "h"x ] || [ "${file##*.}"x = "hpp"x ];then
        echo $file
    fi    
}
 
#测试指定目录  /data_output/ci/history
traverse_dir src