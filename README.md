# 前言
过去十几年，随着半导体技术的进步，嵌入式处理器的性能也有了质的飞跃，其软件系统也变得越来越庞大。

嵌入式开发中，软件开发的方式主要有裸机开发和实时操作系统（RTOS，Real-Time Operation System），性能强大一点的处理器可以支持嵌入式Linux。

功能越简单的产品，通常使用裸机就能够轻松完成任务。但是，随着嵌入式设备功能越来越复杂，实时操作系统（RTOS）的优势逐渐展现出来。RTOS引入了抢占式的任务调度功能，极大降低用户开发多任务的难度。同时，更统一的架构增加了代码的移植性。

RTOS中最主流的两种是FreeRTOS和RT-Thread。目前，FreeRTOS主要有AWS在维护，用户主要是物联网（IoT）设备、工业控制系统和消费电子产品的开发者，它具有极低的内核开销，适合资源极为有限的系统。RT-Thread主要由中国开源社区在维护，相比之下，RT-Thread占用的资源更多，但它的高集成度大大降低了开发门槛。它们都遵循开源协议。

STM32CubeMX软件原生支持FreeRTOS，但是它提供的接口相对复杂。RT-Thread提供了友好的界面，使得开发者能够轻松实现功能。

本文提供一个使用RT-Thread读写SD卡的例子，通过简单的配置实现一个文件系统。

以下开始实操

# 安装RT-Thread Studio软件
到RT-Thread Studio[官网](https://www.rt-thread.org/studio.html)下载一个安装包并安装软件

# 新建RT-Thread Studio工程
打开软件，选择 文件->新建->RT-Thread项目
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/9d8e382ad3714f3797392522864aa3fd.png)
我这里选择STM32F407VE，默认只有F1。可以在RT-Thread SDK管理器中安装需要的包，我这里选择0.2.3（注意这里踩过坑，RT-Thread SDK的版本要和RT-Thread drivers版本对应，否则会出现奇怪的bug）
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/86d0ebb2431046338bc9213871c072b2.png)
生成工程之后点击编译，等待编译完成后下载到单片机，打开箭头所示的终端，就能看到程序运行后自动输出串口数据。至此，说明硬件基本功能正常
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/7617b6ae4111411b84180ee6d21105a1.png)

# 添加SD模块
打开RT-Thread Settings，勾选DFS、Fatfs、SDIO这三个组件，Ctrl+S保存会自动更新组件到代码
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/d038e7984ecc48db968eab804d31e233.png)
打开board.h文件，按照文件中的instruction进行操作，首先使能SDIO功能
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/7bb987ea003f4265a07b9e6333ba0380.png)
# 使用STM32CubeMx生成对应的初始化函数
双击这个文件，打开CubeMx软件![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/a0a2bbc3b2784980adc2779cf1ac1bb2.png)
因为我们这里只用到这个两个函数，其他模块可以不用管
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/8ba34458fb124d42b55959d9390b3fe6.png)
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/405c71d5005144f9a35c1c7c75279360.png)

注意这里要勾选"Generate peripheral initialization as a pair of '.c/.h' files per peripheral"，否则可能出现编译不过，具体原因为止
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/6c7eb0ea2eab4151a55e0da7b8dc9f74.png)

这里选择Keil，接着点击生成代码
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/be19de27ef8d4452b9a173d79ef685ee.png)
拷贝生成的初始化函数到board.c文件的最后
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/055cdf075a524926bbdf8be4bff10180.png)
# 修改主函数

```c
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
```

# 编译下载看现象
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/9af21f7909714744a82c2f64690cfb42.png)
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/3ef6a0a2f6ed4ea8a21825a1887d2384.png)

成功！！

# 关于RT-Thread总结
优点：提供了一个简单的代码生成界面，以及各种集成的模块，大大降低了开发门槛和上手难度

不足：RT-Thread和STM32CubeMx的兼容性不是很好，有些时候生成的文件会产生冲突，需要仔细甄别
# 代码
> https://github.com/dwgan/RT-Thread-SD
