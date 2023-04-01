## 内存池
    学习借鉴一些常见的内存池
---
### [SGI STL allocator](./myalloctor.hpp)
    一级空间配置器 allocator<T>：malloc free 
    二级空间配置器 alloc：freelist自由链表
+ alloc

    + 内存池信息
    ~~~c++
        enum {_ALIGN = 8};
        enum {_MAX_BYTES = 128};//最大分配值
        enum {_NFREELISTS = 16};//空闲列表数 16*8=128
    ~~~
    + 自由链表
    ~~~c++
        union _Obj {//chunk的信息
        union _Obj* _M_free_list_link;
        char _M_client_data[1]; /* The client sees this. */
        };

        static _Obj* volatile free_list[_NFREELISTS];//自由链表
    ~~~
    + chunk分配情况
    ~~~c++
        static char* start_free;//起始位置
        static char* end_free;//结束位置
        static size_t heap_size;//堆的大小
        template <bool __threads, int __inst>
        char* __default_alloc_template<__threads,__inst>::start_free = 0;
        template <bool __threads, int __inst>
        char* __default_alloc_template<__threads, __inst>::end_free = 0;
        template <bool __threads, int __inst>
        size_t __default_alloc_template<__threads, __inst>::heap_size = 0;
    ~~~
    + 辅助函数
    ~~~c++
        /*将 __bytes 上调至最邻近的 8 的倍数*/
        static size_t _S_round_up(size_t bytes)
        { 
            return (((bytes) + (size_t) _ALIGN-1) & ~((size_t) _ALIGN - 1)); 
        }

        /*返回 __bytes 大小的chunk块位于 free-list 中的编号*/
        static size_t _S_freelist_index(size_t bytes) 
        {
            return (((bytes) + (size_t)_ALIGN-1)/(size_t)_ALIGN - 1); 
        }
    ~~~
    + 内存池管理
    ~~~c++
        // 分配内存的入口函数
        static void* allocate(size_t size)
        // 负责把分配好的chunk块进行连接，添加到自由链表当中
        static void* refill(size_t size);
        // 分配相应内存字节大小的chunk块，并且给下面三个成员变量初始化
        static char* chunk_alloc(size_t size, int& nobjs);
        // 把chunk块归还到内存池
        static void deallocate(void* p, size_t size);
        // 内存池扩容函数
        template <bool threads, int inst>
        void*__default_alloc_template<threads, inst>::reallocate(void* p,size_t old_sz, size_t new_sz);
    ~~~
+ 内存创建过程

    1. size > 128 一级配置器
    2. size <= 128 二级配置器 在 free_list 中选择合适的  如果有空间直接分配 没空间调用 refill 获取后分配
    3. refill  调用 chunk_alloc 分配(20)块 只分配1块的直接分配返回 多余1块的分配，设置next返回 
    4. chunk_alloc 
         
        4.1 end-start 满足直接分配 
    
        4.2 不满足 若可用太少将ta分配给映射free_list,再在free_list中寻找空闲的一块放入start-end中来分配 

        4.3 若还是不能分配调用 malloc_alloc::allocate 分配
    5. allocate 用malloc再次尝试 不行调用 _S_oom_malloc 申请
    6. _S_oom_malloc 设置回调函数释放内存 申请空间

+ 内存释放过程
   1. 大块 malloc_alloc::deallocate 直接释放
   2. 小块 链接到free_list

#### 总结
   
    每次重新申请chunk时都一部分使用一部分备用，备用的可给其他字节数使用
    在备用内存池申请空间不满足申请时会将剩余的分配到映射的free_list使用
    申请空间失败可将free_list中未分配的放回备用拿来使用
    使用内存回调函数在内存不够的时候释放内存    
---    
### Nginx Memory Pool 
    Nginx 内存池管理的实现类似于上文STL空间配置器，把内存分配归结为大内存分配和小内存分配。若申请的内存大小比同页的内存池最大值 max 还大，则是大内存分配，否则为小内存分配。
|.h|.cpp|
| :-----:| :----: | 
|[ngx_mem_pool.h](./ngx_mem_pool.h) | [ngx_mem_pool.cpp](./ngx_mem_pool.cpp)|

+ 重要结构
    + 池结构
    ~~~c++
        struct ngx_pool_s {
            ngx_pool_data_t       d; //内存池的数据块
            size_t                max;//内存池数据的最大值
            ngx_pool_s* current;      //当前内存池
            ngx_pool_large_s* large;  //大内存
            ngx_pool_cleanup_s* cleanup; //释放内存的callback
        };
    ~~~
    + 小内存
    ~~~c++
        struct ngx_pool_s;
        struct ngx_pool_data_t{
            u_char* last;      //可分配位置
            u_char* end;       //可分配结束位置
            ngx_pool_s* next;  //下一个池
            ngx_uint_t            failed;  //分配失败次数
        };
    ~~~
    + 大内存
    ~~~c++
        struct ngx_pool_large_s {
            ngx_pool_large_s* next;//大内存链表
            void* alloc;//大块内存地址
        };
    ~~~
    + 清理
    ~~~c++
        using ngx_pool_cleanup_pt = void(*)(void*);
        struct ngx_pool_cleanup_s {
            ngx_pool_cleanup_pt   handler;//清理的callback
            void* data;//callback的参数
            ngx_pool_cleanup_s* next;//清理的链表
        };
    ~~~
    + c++ 实现
    ~~~c++
        class ngx_pool
        {
        public:
            void* ngx_create_pool(size_t size);//创建内存池
            void ngx_destroy_pool(); //销毁内存池
            void ngx_reset_pool();//重置内存池

            void* ngx_palloc(size_t size); //从内存池中申请空间，考虑字节对齐
            void* ngx_pnalloc(size_t size);//从内存池中申请空间，不考虑字节对齐
            void* ngx_pcalloc(size_t size);//调用ngx_palloc


            void  ngx_pfree(void* p); //释放大块内存

            ngx_pool_cleanup_s* ngx_pool_cleanup_add(size_t size);//清理callback
        private:
            inline void* ngx_palloc_small(size_t size,ngx_uint_t align);//小块内存分配
            void* ngx_palloc_block( size_t size);//分配小块内存池
            void* ngx_palloc_large(size_t size);//大块内存分配
        private:
            ngx_pool_s* pool;
        };
    ~~~
+ 内存创建过程
    1. 创建size大小的空间，初始化ngx_pool_data_t 等数据
    2. 分配ngx_pnalloc，ngx_pcalloc，ngx_palloc 判断大小内存
    3. ngx_palloc_small 进行小内存分配，从current开始遍历pool链表，若有空间移动last返回地址 无空间调用ngx_palloc_block分配一个池
    4. ngx_palloc_block 计算池大小，初始化数据，在新池中分配空间，更新current，链接到现有内存池
    5. ngx_palloc_large 分配内存 若有空闲large字段 挂载到large字段 无空闲large字段 小内存分配large字段 头插法加入large链

+ 内存销毁过程
    1. 若注册了cleanup，则遍历该链表结构，依次调用handler函数清理数据
    2. 遍历 large 链表，释放大块内存
    3. 遍历所有分配的内存池，释放内存池结构

+ 内存重置过程
    1. 遍历大块内存链表，释放大块内存
    2. 小内存last，cuerrnt，large重置

+ cleanup 同large 加入了handler

---
### 自建内存池

+ [MemoryMgr.hpp](./MemoryMgr.hpp)
+ [mp.hpp](./mp.hpp)
