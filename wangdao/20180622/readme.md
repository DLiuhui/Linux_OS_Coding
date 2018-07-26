王道2018-06-22
Linux文件操作
1、定义一个学生结构体类型struct student，里边含有学号，姓名，分数，定义结构体数组struct student s[3],给数组赋初值后，写入文件，然后通过lseek偏移到开头，然后再读取进行打印输出
2、新建一个文件，里边内容为hello，通过mmap映射该文件后，修改hello为world，然后解除映射
3、通过mkfifo命令新建两个管道文件1.fifo，2.fifo，然后两个进程通过管道文件实现即时聊天效果
