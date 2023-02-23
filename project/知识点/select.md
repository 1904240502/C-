## select学习
```
在一定时间内，监听用户的文件描述符——可读，可写，异常
```
***
+ 出现原因：
    ```
        recv在阻塞模式下，直到套节字上有数据可读，把数据读到buf中才会返回，在单线程中会导致程序锁死。在非阻塞模式下，没有数据可读返回值是-1，并且错误码是EAGAIN or EWOULDBLOCK
    ```
    recv & send 不了解点[这里](./send%26recv.md)
- **select**
    + 图解
        >>>![selsct模型图](./select.png)
        ```
            用户将需要进行IO操作的socket添加到select中，然后阻塞等待select系统调用返回。当数据到达时，socket被激活，select函数返回。用户线程正式发起read请求，读取数据并继续执行。

            从流程上来看，使用select函数进行IO请求和同步阻塞模型没有太大的区别，甚至还多了添加监视socket，以及调用select函数的额外操作，效率更差。但是，使用select以后最大的优势是用户可以在一个线程内同时处理多个socket的IO请求。用户可以注册多个socket，然后不断地调用select读取被激活的socket，即可达到在同一个线程内同时处理多个IO请求的目的。而在同步阻塞模型中，必须通过多线程的方式才能达到这个目的。
        ```
    + 函数
        ```c++
            int select(int nfds, fd_set *readset, fd_set *writeset, fd_set *exceptset,struct timeval *timeout);
        ```
        **nfds：**  被监听的文件描述符的总数，它比所有文件描述符集合中的文件描述符的最大值大1

        **readset：** 可读描述符

        **writeset：** 可写描述符
    
        **exceptset：** 异常描述符
    
        **timeout：** 设置select函数的超时时间，timeout == NULL 表示等待无限长的时间

        返回值：超时返回0;失败返回-1；成功返回大于0的整数，这个整数表示就绪描述符的数目

    + fd_set集
        ```c++
            #ifndef FD_SETSIZE
            #define FD_SETSIZE      64
            #endif /* FD_SETSIZE */

            typedef struct fd_set {
            u_int fd_count;               /* how many are SET? */
            SOCKET  fd_array[FD_SETSIZE];   /* an array of SOCKETs */
            } fd_set;
        ```
    + 操作函数

        **FD_ZERO()：** 清除一个集合

        **FD_SET() ：** 从集合中添加给定的文件描述符
    
        **FD_ISSET()：** 测试一个文件描述符是否是集合的一部分
    
        **FD_CLR()：** 从集合中删除给定的文件描述符
    - 注意

        有新fd加入select时，还要保存新的fd在一个集中：

            1. 是用于再select返回后，进行FD_ISSET判断。
            2. 是select返回后会把以前加入的但并无事件发生的fd清空，则每次开始select前都要重新从保存的集取得fd逐一加入（FD_ZERO最先）
***
### 总结
    1. select 监听的数量被限制
    2. 监听时采用的是轮询的方法
    3. 需要空间存放要监听的fd
