
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
###  ***select网络模型 [version1.3](./Server1.3)***


  + 将接收客户，消息处理，任务处理分开不同线程处理
      
      |task|post |worker|
      |:---:|:---:|:---:|
      |客户|Tcpserver|EventServer|
      |消息|EventServer|TaskServer|
      |任务|TaskServer|客户端|

  + 心跳检测
    大量客户存在时，退出客户存在退出不完全情况，在客户中添加计时检测，长时间没有接收消息默认客户退出
  + 定时定量发送
  判断缓冲区满时发送数据，存在数据定时将缓冲数据发送
    
  + 线程封装
    1. cv封装 用来安全条件唤醒，阻塞线程 
    2. 线程控制函数：创建，运行，销毁
      
        |函数|操作|
        |:---:|:---:|
        |Start|启动线程，获得线程控制函数|
        |isRun|线程是否启动运行状态|
        |OnWork|启动线程控制函数，唤醒线程|
        |Exit|在工作函数中退出|
        |Close|关闭线程，|

  + 缓冲封装
    
     |类型|读|写|
     |:---:|:---:|:---:|
     |单条命令|push|pop|
     |socket|read4socket|write2socket|
  + 收发异步
    ~~~c++
        auto rlen = client->RecvData();
        //接收客户数据
        if (rlen < 0)
        {
        	return false;
        }
        //网络事件处理
        _pEvent->onRecv(client);
        //处理消息
        while (client->hasData())
        {
        	onMsg(client, client->GetCommand());
        }
        return true;
    ~~~
   收 从socket中读取到缓冲中，处理网络事件，从缓冲中pop取出命令，交由任务线程处理消息，将处理后的消息加入发缓冲中
   发 调用发送将缓冲的数据写入socket，
  + [内存池](./tree/learn/内存池)
