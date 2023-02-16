## C++百万并发网络通信引擎架构与实现



### 用***Socket API*** 建立简易 ***TCP 服务器 客户端*** 


| TCP服务端  | TCP客户端 | 
| :-----:| :----: | 
| 建立socket | 建立socket | 
| 绑定端口bind | 连接服务器connect | 
|监听listen| 接受消息recv|
|接受连接accept|关闭socket closesocket|
|发送消息send||
|关闭socket||

