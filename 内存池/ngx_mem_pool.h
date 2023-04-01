#ifndef  __NGX_MEMERY_POOL_H__
#define __NGX_MEMERY_POOL_H__
#include<cstdlib>
#include<memory>
/*
* c++ʵ��ngnix���ڴ��
*/
using u_char = unsigned char;
using ngx_uint_t = unsigned int;
using ngx_int_t = int;

#define ngx_memzero(buf,n) (void) memset(buf, 0, n)
#define ngx_align(d, a)     (((d) + (a - 1)) & ~(a - 1))//����ֵd�������ٽ�a�ı���
#define ngx_align_ptr(p, a)                                                   \
    (u_char *) (((uintptr_t) (p) + ((uintptr_t) a - 1)) & ~((uintptr_t) a - 1))

#define NGX_MIN_POOL_SIZE ngx_align((sizeof(ngx_pool_s) + 2 * sizeof(ngx_pool_large_s)), NGX_POOL_ALIGNMENT)
const int NGX_ALIGNMENT = sizeof(unsigned long);
const int ngx_pagesize = 4096;  //ҳ��С
const int NGX_MAX_ALLOC_FROM_POOL = (ngx_pagesize - 1); //������ռ�
const int NGX_DEFAULT_POOL_SIZE = (16 * 1024);  //Ĭ�Ͽ��ٳش�С
const int NGX_POOL_ALIGNMENT = 16; //�ڴ����
/*
* ����
*/
using ngx_pool_cleanup_pt = void(*)(void*);
struct ngx_pool_cleanup_s {
    ngx_pool_cleanup_pt   handler;//�����callback
    void* data;//callback�Ĳ���
    ngx_pool_cleanup_s* next;//���������
};
/*
* ����ڴ��
*/
struct ngx_pool_large_s {
    ngx_pool_large_s* next;//���ڴ�����
    void* alloc;//����ڴ��ַ
};
/*
* С���ڴ��
*/
struct ngx_pool_s;
struct ngx_pool_data_t{
    u_char* last;      //�ɷ���λ��
    u_char* end;       //�ɷ������λ��
    ngx_pool_s* next;  //��һ����
    ngx_uint_t            failed;  //����ʧ�ܴ���
};
/*
* �ؽṹ
*/
struct ngx_pool_s {
    ngx_pool_data_t       d; //�ڴ�ص����ݿ�
    size_t                max;//�ڴ�����ݵ����ֵ
    ngx_pool_s* current;      //��ǰ�ڴ��
    ngx_pool_large_s* large;  //���ڴ�
    ngx_pool_cleanup_s* cleanup; //�ͷ��ڴ��callback
};

class ngx_pool
{
public:
    void* ngx_create_pool(size_t size);//�����ڴ��
    void ngx_destroy_pool(); //�����ڴ��
    void ngx_reset_pool();//�����ڴ��

    void* ngx_palloc(size_t size); //���ڴ��������ռ䣬�����ֽڶ���
    void* ngx_pnalloc(size_t size);//���ڴ��������ռ䣬�������ֽڶ���
    void* ngx_pcalloc(size_t size);//����ngx_palloc

  
    void  ngx_pfree(void* p); //�ͷŴ���ڴ�

    ngx_pool_cleanup_s* ngx_pool_cleanup_add(size_t size);//����callback
private:
    inline void* ngx_palloc_small(size_t size,ngx_uint_t align);//С���ڴ����
    void* ngx_palloc_block( size_t size);//����С���ڴ��
    void* ngx_palloc_large(size_t size);//����ڴ����
private:
    ngx_pool_s* pool;
};

//const int NGX_MIN_POOL_SIZE = ngx_align((sizeof(ngx_pool_s) + 2 * sizeof(ngx_pool_large_s)), NGX_POOL_ALIGNMENT);
#endif //__NGX_MEMERY_POOL_H__
