/*
 * Copyright (c) 2006-2025, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2025-01-10     RT-Thread    first version
 */

#include <rtthread.h>

#define DBG_TAG "main"
#define DBG_LVL DBG_LOG
#include <rtdbg.h>

#include <stdio.h>
#include <string.h>


void read_test_file(void)
{
    FILE *file;
    char buffer[100];

    // 打开文件进行读取
    file = fopen("/test.txt", "r");
    if (file == RT_NULL)
    {
        rt_kprintf("Failed to open file /test.txt for reading\r\n");
        return;
    }

    // 读取数据
    if (fgets(buffer, sizeof(buffer), file) != RT_NULL)
    {
        rt_kprintf("Read from file: %s\r\n", buffer);
    }
    else
    {
        rt_kprintf("Failed to read data from /test.txt\r\n");
    }

    // 关闭文件
    fclose(file);
}


void write_test_file(void)
{
    FILE *file;
    const char *data = "this is a test string!";

    // 打开文件（如果文件不存在则创建）
    file = fopen("/test.txt", "w+"); // 使用绝对路径，确保文件挂载点正确
    if (file == RT_NULL)
    {
        rt_kprintf("Failed to open file /test.txt\r\n");
        return;
    }

    // 写入数据到文件
    if (fwrite(data, 1, strlen(data), file) != strlen(data))
    {
        rt_kprintf("Failed to write data to /test.txt\r\n");
        fclose(file);
        return;
    }

    // 刷新缓冲区，确保数据写入 SD 卡
    fflush(file);

    // 关闭文件
    fclose(file);

    rt_kprintf("Data written to /test.txt successfully\r\n");
}

void mnt_init(void)
{
    rt_thread_mdelay(500);//这段延时必须加上，系统上电过程中存在延时，否则会出现先挂载后注册块设备sd0的情况
    mkfs("elm","sd0");//挂在前需格式化
    if(dfs_mount("sd0","/","elm",0,0)==0) //挂载文件系统，参数：块设备名称、挂载目录、文件系统类型、读写标志、私有数据0
    {
        rt_kprintf("dfs mount success\r\n");
    }
    else
    {
        rt_kprintf("dfs mount failed\r\n");
    }
}

int main(void)
{
    int count = 1;
    // 初始化文件系统
    mnt_init();
    // 调用文件写入函数
    write_test_file();
    // 调用文件读取函数
    read_test_file();
    while (count++)
    {
//        LOG_D("Hello RT-Thread!");
        rt_thread_mdelay(1000);
    }

    return RT_EOK;
}
