## C++服务器
***

### 用***Socket API*** 建立简易 ***TCP 服务器 客户端*** 

```
服务端：等待新客户端连接，向客户端发送数据，再次循环等待新客户端
客户端：连接服务端，接受服务端消息
```

| TCP服务端  | TCP客户端 | 
| :-----:| :----: | 
| 建立socket | 建立socket | 
| 绑定端口bind |  | 
|监听listen| 连接服务器connect|
|接受连接accept||
|发送消息send|接受消息recv|
|关闭socket|关闭socket closesocket|
|[服务端代码](https://github.com/1904240502/c-with-class/blob/project/project/EasyTcpServer/server.cpp)|[客户端代码](https://github.com/1904240502/c-with-class/blob/project/project/EastyTcpclient/client.cpp)|
***

### *** 建立能持续请求的c/s网络程序 ***

```
服务端：等待新客户端连接，接受客户端请求，处理请求后发送消息给客户端
客户端：连接服务端，输入请求，向服务端发送请求，接受服务端处理后的消息
```

| TCP服务端  | TCP客户端 | 
| :-----:| :----: | 
| 建立socket | 建立socket | 
| 绑定端口bind |  | 
|监听listen| 连接服务器connect|
|接受连接accept|客户输入请求cin|
||处理请求strcmp|
||向服务端发送请求send|
|接受客户端请求recv||
|处理请求strcmp||
|向客户发送消息send|接受消息recv|
|关闭socket|关闭socket closesocket|
|[服务端代码](https://github.com/1904240502/c-with-class/blob/project/project/midTcpServer/server.cpp)|[客户端代码](https://github.com/1904240502/c-with-class/blob/project/project/midTcpclient/client.cpp)|
***

