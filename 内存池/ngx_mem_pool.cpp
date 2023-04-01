#include "ngx_mem_pool.h"

void * ngx_pool::ngx_create_pool(size_t size)
{
    //����ռ�
    pool = static_cast<ngx_pool_s*>(malloc(size));
    if (pool == nullptr) {
        return nullptr;
    }
    
    //��ʼ�� С�ڴ��
    pool->d.last = (u_char*)pool + sizeof(ngx_pool_s);
    pool->d.end = (u_char*)pool + size;
    pool->d.next = nullptr;
    pool->d.failed = 0;

    //����ɷ����С
    size = size - sizeof(ngx_pool_s);
    pool->max = (size < NGX_MAX_ALLOC_FROM_POOL) ? size : NGX_MAX_ALLOC_FROM_POOL;

    pool->current = pool;
    pool->large = nullptr;
    pool->cleanup = nullptr;
	return pool;
}

void ngx_pool::ngx_destroy_pool()
{
    ngx_pool_s* p, * n;
    ngx_pool_large_s* l;
    ngx_pool_cleanup_s* c;

    //����handler������������
    for (c = pool->cleanup; c; c = c->next) {
        if (c->handler) {
            c->handler(c->data);
        }
    }
    //������ڴ�
    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            free(l->alloc);
        }
    }
    //����С�ڴ��
    for (p = pool, n = pool->d.next; /* void */; p = n, n = n->d.next) {
       free(p);
        if (n == NULL) {
            break;
        }
    }
}

void ngx_pool::ngx_reset_pool()
{
    ngx_pool_s* p;
    ngx_pool_large_s* l;
    //�ͷŴ��ڴ�
    for (l = pool->large; l; l = l->next) {
        if (l->alloc) {
            free(l->alloc);
        }
    }
    /*
    * С�ڴ��ʼ������
    * С�ڴ�ֻ���ڵ�һ����ʱ��Ű�������ڳص���Ϣ
    * ������ֻ�� ngx_pool_data_t
    */

    if(pool)  
    {//��һ��
        p = pool; 
        p->d.last = (u_char*)p + sizeof(ngx_pool_s);
        p->d.failed = 0;
    }
    for (p = pool->d.next; p; p = p->d.next) {
        //next��
        p->d.last = (u_char*)p + sizeof(ngx_pool_s);
        p->d.failed = 0;
    }
    pool->current = pool;
    pool->large = NULL;
}

void* ngx_pool::ngx_palloc(size_t size)
{
    if (size <= pool->max) {
        return ngx_palloc_small(size, 1);
    }
    return ngx_palloc_large( size);
}

void* ngx_pool::ngx_pnalloc(size_t size)
{

    if (size <= pool->max) {
        return ngx_palloc_small(size, 0);
    }
    return ngx_palloc_large(size);
}

void* ngx_pool::ngx_pcalloc(size_t size)
{
    void* p;

    p = ngx_palloc(size);
    if (p) {
        ngx_memzero(p, size);
    }

    return p;
}

void ngx_pool::ngx_pfree(void* p)
{
    ngx_pool_large_s* l;

    for (l = pool->large; l; l = l->next) {
        if (p == l->alloc) {
            free(l->alloc);
            l->alloc = nullptr;
            return ;
        }
    }
}

ngx_pool_cleanup_s* ngx_pool::ngx_pool_cleanup_add(size_t size)
{
    ngx_pool_cleanup_s* c;

    c =static_cast<ngx_pool_cleanup_s*>( ngx_palloc(sizeof(ngx_pool_cleanup_s)));
    if (c == nullptr) {
        return nullptr;
    }

    if (size) {
        c->data = ngx_palloc(size);
        if (c->data == nullptr) {
            return nullptr;
        }

    }
    else {
        c->data = nullptr;
    }

    c->handler = nullptr;
    c->next = pool->cleanup;

    pool->cleanup = c;

    return c;
}

inline void* ngx_pool::ngx_palloc_small(size_t size, ngx_uint_t align)
{
    u_char* m;
    ngx_pool_s* p;

    p = pool->current;

    do {
        m = p->d.last;

        //�����������С
        if (align) {
            m = ngx_align_ptr(m, NGX_ALIGNMENT);
        }
        //���Է���
        if ((size_t)(p->d.end - m) >= size) {
            p->d.last = m + size;

            return m;
        }
        //�����Է�������һ��
        p = p->d.next;

    } while (p);

    return ngx_palloc_block(size);
}

void* ngx_pool::ngx_palloc_block( size_t size)
{
    u_char* m;
    size_t       psize;
    ngx_pool_s* p, * newpool;

    //����ش�С
    psize = (size_t)(pool->d.end - (u_char*)pool);

    m = static_cast<u_char*>(malloc(psize));
    if (m == NULL) {
        return NULL;
    }
   
    newpool = reinterpret_cast<ngx_pool_s*>(m);

    newpool->d.end = m + psize;
    newpool->d.next = nullptr;
    newpool->d.failed = 0;

    m += sizeof(ngx_pool_data_t);
    m = ngx_align_ptr(m, NGX_ALIGNMENT);
    newpool->d.last = m + size;

    for (p = pool->current; p->d.next; p = p->d.next) {
        if (p->d.failed++ > 4) {
            //ʧ��4�������ƶ�currentָ��
            pool->current = p->d.next;
        }
    }

    //�����ں���
    p->d.next = newpool;

    return m;
}

void* ngx_pool::ngx_palloc_large(size_t size)
{
    void* p;
    ngx_uint_t         n;
    ngx_pool_large_s* large;

    p = malloc(size);
    if (p == nullptr) {
        return nullptr;
    }

    n = 0;

    //�Ѿ�������large�ֶ� ���ص��ڴ�ص�large�ֶ�
    for (large = pool->large; large; large = large->next) {
        if (large->alloc == nullptr) {
            large->alloc = p;
            return p;
        }

        if (n++ > 3) {
            break;
        }
    }
    //δ������large�ֶ� ��С�ڴ��з�����ڴ�Ŀ�
    large =static_cast<ngx_pool_large_s *>( ngx_palloc_small(sizeof(ngx_pool_large_s), 1));
    if (large == nullptr) {
        free(p);
        return nullptr;
    }

    //ͷ�巢�� ����ֶμ���large��
    large->alloc = p;
    large->next = pool->large;
    pool->large = large;

    return p;
}
