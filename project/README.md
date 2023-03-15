
## C++服务器

***
### 用Socket API建立简易 TCP 服务器 客户端  version1.0
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
|[服务端代码](./Server1.0/server.cpp)|[客户端代码](./Client1.0/client.cpp)|

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
|[服务端代码](./Server1.1/server.cpp)|[客户端代码](./Client1.1/client.cpp)|

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
  + |[服务端代码](./Server1.2_1/server.cpp)|[客户端代码](./Client1.2_1/client.cpp)|
  + |[select介绍](./知识点/select.md)|[send & recv](./知识点/send%26recv.md)|

    服务端可接收多个客户端，客户端线程中实现请求输入发送，主线程中接收服务端消息
    两端都可以处理其他业务，不用阻塞等待
    
  + 封装成类
    
    + [客户端](./Client1.2_2)

      初始化创建一个socket对象，连接对象如果没有创建socket创建开启线程循环输入发送请求，如果连接到了服务端循环接收消息接收消息获得消息指令，消息数据,按照数据指令处理消息数据

    + [服务端](./Server1.2_2)
      
      初始化创建一个socket对象，创建服务端绑定端口，监听网络如果没有创建socket先创建，
      循环开始服务端处理，监听用户fd状态，有新客户端加入，将新客户socket加入客户端集并告知所有客户端。依次处理有消息的客户端，接收到消息，根据消息指令来接收客户端消息并向客户端发送处理结果，如果客户端关闭或客户请求退出，在客户端集中删除该客户端 

  + 处理粘包————【1Gbps】
    
        recv & send是将缓冲区的数据进行copy的作用，但是在copy中可能由于数据过大，为了接收一个完整数据，需要多次copy，才能将数据完整接收到一个自定义缓冲中。

    + [客户端](./Client1.2_3)
        ```c++
        int len = sizeof(DataHeader);
        //从缓冲中copy数据到中转缓冲
	      int rlen = RecvData(_BUF, BUF_SIZE);
	      //转到客户端自建缓冲
	      memcpy(_BUFS + _BUF_POS, _BUF, rlen);
	      _BUF_POS += rlen;
	      //接收数据
	      while (_BUF_POS>=len)
	      {
		      //获得数据长度
		      DataHeader* header = (DataHeader*)_BUFS;
		      //缓冲中已经有完整的包
		      if (_BUF_POS >= header->dataLength)
		      {
			      int nlen = header->dataLength;
			      //处理消息
			      ExeCom(header);
			      //从缓冲中取出包
			      memcpy(_BUFS, _BUFS + nlen, _BUF_POS- nlen);
			      _BUF_POS -= nlen;
		      }
		      else
              { break;}        
        }
        ```
    + [服务端](./Server1.2_3)
        
        创建客户类用来记录连接客户消息，将原本保存的fd变为client，copy数据到服务端buf，在将buf的数据copy到客户端的缓冲中。添加精准时间chrono用来计算传输速率。
  + 线程————【Gbps】
    
    + [客户端](./Client1.2_4)

        为了测试服务端压力，将客户端改为一次创建多个客户，但是客户端会在创建连接服务端浪费时间，因此创建多个线程在线程中加快连接发送
    + [服务端](./Server1.2_4)
        
        服务端监听到有客户连接，在主线程中监听是否有新客户加入，有新客户加入分配到线程客户数少的线程。其他线程中监听客户消息。客户——临界资源，建立客户缓冲队列。
        ```mermaid
        graph TB
        A(Client) 
        B{exit}
        C((创建客户))
        D((SendData))
        Z(结束)

        A--线程--> B--yes-->Z
        B--no-->业务处理-->Z
        A--线程--> C-->D
        B--no-->D-.while.-D
        
        
        E(Server)
        F[producer]
        G[consumer]
        H((select_客户))
        I((select_消息))
        J[最少客户]
        K{缓冲队列}
        L[RecvData]
        M[SendData]
    
        E-- main -->F
        F-->H-->J-.while.-H
        C-.->H
        
        J-.加入客户缓冲队列.->G

        E--线程 -->G
        G-->K
        K--不空-->加入客户队列-->I
        K--空-->I-->L-->M-.while.->K
        D-.->I
       ```
       ~~~
       服务端在传递客户给线程处理时，会出现类似消费者问题，缓冲是临界资源，要进行互斥访问，因此添加到ClientSet和将ClientSet->Clients要进行加锁。
       ~~~
***




