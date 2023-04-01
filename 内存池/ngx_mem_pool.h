#ifndef  __NGX_MEMERY_POOL_H__
#define __NGX_MEMERY_POOL_H__
#include<cstdlib>
#include<memory>
/*
* c++实现ngnix的内存池
*/
using u_char = unsigned char;
using ngx_uint_t = unsigned int;
using ngx_int_t = int;

#define ngx_memzero(buf,n) (void) memset(buf, 0, n)
#define ngx_align(d, a)     (((d) + (a - 1)) & ~(a - 1))//将数值d调整到临近a的倍数
#define ngx_align_ptr(p, a)                                                   \
    (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

#define NGX_MIN_POOL_SIZE ngx_align((sizeof(ngx_pool_s) + 2 * sizeof(ngx_pool_large_s)), NGX_POOL_ALIGNMENT)
const int NGX_ALIGNMENT = sizeof(unsigned long);
const int ngx_pagesize = 4096;  //页大小
const int NGX_MAX_ALLOC_FROM_POOL = (ngx_pagesize - 1); //最大分配空间
const int NGX_DEFAULT_POOL_SIZE = (16 * 1024);  //默认开辟池大小
const int NGX_POOL_ALIGNMENT = 16; //内存对齐
/*
* 清理
*/
using ngx_pool_cleanup_pt = void(*)(void*);
struct ngx_pool_cleanup_s {
    ngx_pool_cleanup_pt   handler;//清理的callback
    void* data;//callback的参数
    ngx_pool_cleanup_s* next;//清理的链表
};
/*
* 大块内存池
*/
struct ngx_pool_large_s {
    ngx_pool_large_s* next;//大内存链表
    void* alloc;//大块内存地址
};
/*
* 小块内存池
*/
struct ngx_pool_s;
struct ngx_pool_data_t{
    u_char* last;      //可分配位置
    u_char* end;       //可分配结束位置
    ngx_pool_s* next;  //下一个池
    ngx_uint_t            failed;  //分配失败次数
};
/*
* 池结构
*/
struct ngx_pool_s {
    ngx_pool_data_t       d; //内存池的数据块
    size_t                max;//内存池数据的最大值
    ngx_pool_s* current;      //当前内存池
    ngx_pool_large_s* large;  //大内存
    ngx_pool_cleanup_s* cleanup; //释放内存的callback
};

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

//const int NGX_MIN_POOL_SIZE = ngx_align((sizeof(ngx_pool_s) + 2 * sizeof(ngx_pool_large_s)), NGX_POOL_ALIGNMENT);
#endif //__NGX_MEMERY_POOL_H__
