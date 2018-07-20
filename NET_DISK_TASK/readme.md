线程池模型
服务器与客户端进行文件传输
服务器退出时会等待所有正在工作的子线程完成任务后再退出

共五期，完成前3期
第五期分线程完成部分：采用work—IO-queue 和 work-shell-queue 区分不同类型的任务，1/4的线程进行shell指令 3/4的进行文件I/O

编写服务器端， 服务器端启动， 然后启动客户端， 通过客户端可以输入以下命令进
行服务器上的文件查看：

用户输入
0.cd + 目录 进入对应目录
1.ls + 目录 列出相应目录文件
2.remove + 文件path 删除服务器上文件
3.pwd 显示目前所在路径
4.gets +文件path 下载服务器文件到本地
5.puts +本地文件path 将本地文件上传至服务器
//6.mkdir 增加目录
//7.rmdir 删除目录（目录必须为空目录，要删除非空目录留待拓展）

系统保留
100.login 登录指令
101.注册 让用户注册



其他命令不响应:
项目启动方法
./ftpserver ../conf/server.conf
客户端
./client ip port

二期

1、密码登录 注册

2.日志记录
1、 日志记录客户端请求信息， 及客户端连接时间
2、 日志记录客户端操作记录， 及操作时间

3.断点续传
1、 进行 gets hello.avi 时候， 判断本地是否存在 hello.avi， 如果存在， stat 获取 hello.avi 的大
小， 传递给服务器的是 gets hello.avi 1000

4.mmap 将大文件映射入内存， 进行网络传递
->采用sendfile进行传输
1、 当你发现文件大于 100M， 就进行 mmap 该文件， 然后再传输

三期
1、 数据库连接， 通过数据库存储用户名， salt 值(采用随机字符串生成)， 密码(密文形式存
		储)， 通过数据库存储日志

2、 密码验证的方式是服务器先传输 salt 值给客户端， 客户端 crypt 加密后， 传输密码密文给
服务器， 服务器匹配后， 判断是否成功

3、由数据库记录每一个用户的路径情况， 每一个用户的文件及目录采用数据库的表进行存
储， 每个文件的文件内容以其 MD5 码存在磁盘上， 并以 md5 码进行命名， 所有用户的文件
都存在一个目录里， 各自用户只能看到自己的文件， 不能看到其他人的文件， 通过 C 接口
得到每一个文件的 MD5， 查看http://bbs.chinaunix.net/thread-2125735-1-1.html， 或者直接 man MD5
用到openssl库 -lssl -lcrypto

日志函数：
bzero(server.log_buffer, LOG_BUFFER_SIZE);
sprintf(server.log_buffer, "ip:%s connect", inet_ntoa(clientaddr.sin_addr));
log_input(server.log_fd, server.log_lock, server.log_buffer);
bzero(server->log_buffer, LOG_BUFFER_SIZE);
sprintf(server->log_buffer, "user:%s cd %s", name, task->option);
log_input(server->log_fd, server->log_lock, server->log_buffer);


未完成
4、期p2p
多点下载， 多点下载是指客户端通过多个 sfd 连接不同的服务器， 将一份大文件拆分为几段，不同段从不同的服务器进行下载分为三级难度：
A:固定从某几台服务器下载， 而且文件多点下载一定一次性下载完毕
B:从某台 IP 服务器上得到具有数据源的服务器地址， 然后再从对应的地址进行下载， 而且文件一次性下载完毕
C:如果多点下载过程中， 客户端断开， 那么下次重新下载时， 需要再次进行多点下载， 所以客户端本地需要使用文件， 或者数据库存储多点下载， 每个位置下载到什么地方
想研究实际 FTP 协议， 详见
http://blog.csdn.net/yxyhack/article/details/1826256

5、
五期功能

下载文件

cd pwd  ls

puts  gets

服务器有不同的业务线程

