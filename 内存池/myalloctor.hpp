#pragma once
#include<mutex>

template <int __inst>
class __malloc_alloc_template {

private:

    static void* _S_oom_malloc(size_t);
    static void* _S_oom_realloc(void*, size_t);
    static void (*__malloc_alloc_oom_handler)();

public:

    static void* allocate(size_t size)
    {
        void* result = malloc(size);
        if (nullptr == result) result = _S_oom_malloc(size);
        return result;
    }

    static void deallocate(void* p, size_t /* size */)
    {
        free(p);
    }

    static void* reallocate(void* p, size_t /* old_sz */, size_t new_sz)
    {
        void* result = realloc(p, new_sz);
        if (nullptr == result) result = _S_oom_realloc(p, new_sz);
        return result;
    }

    static void (*__set_malloc_handler(void (*f)()))()
    {
        void (*old)() = __malloc_alloc_oom_handler;
        __malloc_alloc_oom_handler = f;
        return(old);
    }

};

template <int __inst>
void* __malloc_alloc_template<__inst>::_S_oom_malloc(size_t size)
{
    //再次分配
    void (*my_malloc_handler)();
    void* result;
    //循环申请
    for (;;) {
        my_malloc_handler = __malloc_alloc_oom_handler; 
        if (nullptr== my_malloc_handler) { throw std::bad_alloc(); }
        (*my_malloc_handler)();//调用回调函数 来释放一些内存
        result = malloc(size);
        if (result) return(result);
    }
}

template <int __inst>
void* __malloc_alloc_template<__inst>::_S_oom_realloc(void* p, size_t size)
{
    void (*my_malloc_handler)();
    void* result;

    for (;;) {
        my_malloc_handler = __malloc_alloc_oom_handler;
        if (0 == my_malloc_handler) { throw std::bad_alloc(); }
        (*my_malloc_handler)();
        result = realloc(p, size);
        if (result) return(result);
    }
}

typedef __malloc_alloc_template<0> malloc_alloc;

template <int __inst>
void  (*__malloc_alloc_template<__inst>::__malloc_alloc_oom_handler)() = nullptr;


template<class T>
class myalloctor
{
private:
    enum { _ALIGN = 8 };//8 开始 8对齐
    enum { _MAX_BYTES = 128 };//最大的块
    enum { _NFREELISTS = 16 }; // free_list个数

   union _Obj {
        union _Obj* _M_free_list_link;
        char _M_client_data[1];    /* The client sees this.        */
    };

    // Chunk allocation state.
    static char* start_free;
    static char* end_free;
    static size_t heap_size;

    static _Obj* volatile free_list[_NFREELISTS];
     
    static std::mutex mutex;

    static size_t round_up(size_t bytes)
    {
        return (((bytes)+(size_t)_ALIGN - 1) & ~((size_t)_ALIGN - 1));
    }
    static size_t freelist_index(size_t bytes)
    {
        return (((bytes)+(size_t)_ALIGN - 1) / (size_t)_ALIGN - 1);
    }
    static void* refill(size_t size)
    {
        int nobjs = 20;
        char* chunk = chunk_alloc(size, nobjs);
        _Obj* volatile* my_free_list;
        _Obj* result;
        _Obj* current_obj;
        _Obj* next_obj;
        int i;

        if (1 == nobjs) return(chunk);
        my_free_list = free_list + freelist_index(size);

        /* 构建 free_list 的 chunk */
        result = reinterpret_cast<_Obj*> (chunk);
        *my_free_list = next_obj = reinterpret_cast<_Obj*>(chunk + size);
        for (i = 1; ; i++) { //循环设置next指针
            current_obj = next_obj;
            next_obj = reinterpret_cast<_Obj*>((char*)next_obj + size);
            if (nobjs - 1 == i) {
                current_obj->_M_free_list_link = 0;
                break;
            }
            else {
                current_obj->_M_free_list_link = next_obj; 
            }
        }
        return(result);
    }
    static char* chunk_alloc(size_t size, int& nobjs)
    {
        char* result;
        size_t total_bytes = size * nobjs;
        size_t bytes_left = end_free - start_free;

        if (bytes_left >= total_bytes) {// 分出 20 chunk
            result = start_free;
            start_free += total_bytes;
            return(result);
        }
        else if (bytes_left >= size) {// 分出最大可分配 >=1 chunk
            nobjs = static_cast<int>(bytes_left / size);
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;
            return(result);
        }
        else { // 无可分配 < 1 chunk
            size_t bytes_to_get =2 * total_bytes + round_up(heap_size >> 4);
            // 将剩余的分配给对应的free_list
            if (bytes_left > 0) {
                _Obj* volatile* my_free_list = free_list + freelist_index(bytes_left);
                ((_Obj*)start_free)->_M_free_list_link = *my_free_list;
                *my_free_list = (_Obj*)start_free;
            }
            //分配大块内存 0 chunk
            start_free = static_cast <char*>(malloc(bytes_to_get));
            if (nullptr == start_free) { //系统没有空间可分配
                size_t i;
                _Obj* volatile* my_free_list;
                _Obj* p;
               
                for (i = size; i <= (size_t)_MAX_BYTES; i += (size_t)_ALIGN){ //1)在比他大的里面找可以的拿来分配的放到free_list来分配（1 chunk)
                    my_free_list = free_list + freelist_index(i);
                    p = *my_free_list;
                    if (nullptr!=p) 
                    {
                        *my_free_list = p->_M_free_list_link;
                        start_free =reinterpret_cast<char*>(p);
                        end_free = start_free + i;
                        return(chunk_alloc(size, nobjs));
                        
                    }
                }
                end_free = 0;	//2)申请空间异常处理
                start_free = (char*)malloc_alloc::allocate(bytes_to_get);
                // This should either throw an
                // exception or remedy the situation.  Thus we assume it
                // succeeded.
            }
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            return(chunk_alloc(size, nobjs));
        }
    }
public:


    //constexpr myalloctor() noexcept {}

    //constexpr myalloctor(const myalloctor&) noexcept = default;
    //template <class _Other>
    //constexpr myalloctor(const myalloctor<_Other>&) noexcept {}

    //入口
    T* allocator(size_t size)
    {
        void* ret = nullptr; //分配的位置
       
        if (size > (size_t)_MAX_BYTES) {
            /*-----------一级分配器--------------*/
            ret = malloc_alloc::allocate(size);
        }
        else {
            /*-----------二级分配器--------------*/

            //映射到free_list的地址 （数组地址）
            _Obj* volatile * my_free_list = free_list + freelist_index(size);
             
            std::lock_guard<std::mutex> guard(mutex);
    
            //映射块的 起始
            _Obj* __restrict result = *my_free_list;
            if (result == nullptr)
            {
                //初始分配 & 再次申请
                ret = refill(round_up(size));
            } 
            else {
                //第一块给出，指向下一块
                *my_free_list = result->_M_free_list_link;
                ret = result;
            }
        }

        return ret;
    }
    //释放内存
    void deallocate(void* p, size_t  size )
    {
        if (size > (size_t)_MAX_BYTES)
            malloc_alloc::deallocate(p, size);
        else 
        {
            _Obj* volatile* __my_free_list
                = free_list + freelist_index(size);
            _Obj* q = (_Obj*)p;
           
            std::lock_guard<std::mutex> guard(mutex);

            q->_M_free_list_link = *__my_free_list;
            *__my_free_list = q;
            // lock is released here
        }
    }
    //重分配
    void* reallocate(void* p, size_t  old_sz , size_t new_sz)
    {
        void* result;
        size_t copy_sz;

        //大块重分配
        if (old_sz > (size_t)_MAX_BYTES && new_sz > (size_t)_MAX_BYTES) {
            return(realloc(p, new_sz));
        }
        //小块重新分配
        if (round_up(old_sz) == round_up(new_sz)) return(p);
        result = malloc_alloc::allocate(new_sz);
        copy_sz = new_sz > old_sz ? old_sz : new_sz;
        memcpy(result, p, copy_sz);
        deallocate(p, old_sz);
        return(result);
    }
    void construct(T* p, const T& val)
    {
        new (p) T(val);
    }
    void destory(T* p)
    {
        p->~T();
    }

    using value_type = T;
};

template <class T>
char* myalloctor<T>::start_free = nullptr;

template <class T>
char* myalloctor<T>::end_free = nullptr;

template <class T>
size_t myalloctor<T>::heap_size = 0;

template <class T> 
typename myalloctor<T>::_Obj* volatile myalloctor<T>::free_list[_NFREELISTS] = { nullptr };

template<class T>
std::mutex myalloctor<T>::mutex;