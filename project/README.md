
## C++服务器

***
### 用***Socket API*** 建立简易 ***TCP 服务器 客户端  version1.0*** 
  ```
    服务端：建立socket，连接客户端，发送消息给客户端
    客户端：建立socket，连接服务端，接收服务端消息
  ```
  | TCP服务端  | TCP客户端 | 
  | :-----:| :----: | 
  | 建立socket | 建立socket | 
| 绑定端口bind |  | 
|监听网络端口listen| 连接服务器connect|
|客户端连接accept||
|发送消息send|接受消息recv|
|关闭socket|关闭socket closesocket|
|[服务端代码](https://github.com/1904240502/c-with-class/blob/project/project/EasyTcpServer/server.cpp)|[客户端代码](https://github.com/1904240502/c-with-class/blob/project/project/EastyTcpclient/client.cpp)|

***
### ***持续请求的c/s网络模型 version1.1*** 

  ```
    服务端：建立socket，连接客户端，接收客户端请求，处理请求，返回请求信息
    客户端：建立socket，连接服务端，输入处理请求，发送请求给服务端，接收服务端返回数据
  ```

| TCP服务端  | TCP客户端 | 
| :-----:| :----: | 
| 建立socket | 建立socket | 
| 绑定端口bind |  | 
|监听网络端口listen|连接服务器connect|
|接受连接accept|用户输入请求cin|
||客户端处理请求strcmp|
||向服务端发送请求send|
|接收客户端请求recv||
|处理请求strcmp||
|向客户端返回请求send|接收服务端返回数据recv|
|关闭socket|关闭socket closesocket|
|[服务端代码](https://github.com/1904240502/c-with-class/blob/project/project/midTcpServer/server.cpp)|[客户端代码](https://github.com/1904240502/c-with-class/blob/project/project/midTcpclient/client.cpp)|


+ 数据报
  
  类型：登录，登录结果，登出，登出结果，错误

  报头：数据长度，数据类型

  数据：报头，包体

***
### ***select网络模型 version1.2*** 

  ```
    服务端：建立socket，连接客户端，select监听，接收客户端请求，处理请求，返回请求信息
    客户端：建立socket，连接服务端，创建线程输入发送请求，select监听，接收服务端返回数据
  ```
  + |[服务端代码](https://github.com/1904240502/c-with-class/blob/project/project/SelectServer/server.cpp)|[客户端代码](https://github.com/1904240502/c-with-class/blob/project/project/SelectClient/client.cpp)|
  + |[select介绍](https://github.com/1904240502/c-with-class/blob/project/project/SelectServer/server.cpp)|[send & recv](https://github.com/1904240502/c-with-class/blob/project/project/SelectClient/client.cpp)|

    服务端可接收多个客户端，客户端线程中实现请求输入发送，主线程中接收服务端消息
    两端都可以处理其他业务，不用阻塞等待
***


