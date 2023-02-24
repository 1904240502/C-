
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
|[服务端代码](./EasyTcpServer/server.cpp)|[客户端代码](./EastyTcpclient/client.cpp)|

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
|[服务端代码](./midTcpServer/server.cpp)|[客户端代码](./midTcpclient/client.cpp)|


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
  + |[服务端代码](./SelectServer/server.cpp)|[客户端代码](./SelectClient/client.cpp)|
  + |[select介绍](./知识点/select.md)|[send & recv](./知识点/send%26recv.md)|

    服务端可接收多个客户端，客户端线程中实现请求输入发送，主线程中接收服务端消息
    两端都可以处理其他业务，不用阻塞等待
    
  + 封装成类
    
    + 客户端

      初始化创建一个socket对象，连接对象如果没有创建socket创建开启线程循环输入发送请求，如果连接到了服务端循环接收消息接收消息获得消息指令，消息数据,按照数据指令处理消息数据

    + 服务端
      
      初始化创建一个socket对象，创建服务端绑定端口，监听网络如果没有创建socket先创建，
      循环开始服务端处理，监听用户fd状态，有新客户端加入，将新客户socket加入客户端集并告知所有客户端。依次处理有消息的客户端，接收到消息，根据消息指令来接收客户端消息并向客户端发送处理结果，如果客户端关闭或客户请求退出，在客户端集中删除该客户端 
***




