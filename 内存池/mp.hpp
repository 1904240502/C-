#ifndef __MEMORY_POOL_H__
#define __MEMORY_POOL_H__
#include<stdlib.h>
#include<map>

#define  MP_ALIGNMENT 64
#define  MP_ALIGNMENT_NUM 2000

#define CACULATE_BLOCK_SZIE(size) \
 (size / MP_ALIGNMENT) * MP_ALIGNMENT + size % MP_ALIGNMENT ? MP_ALIGNMENT : 0

#define MB_NUM(size) MP_ALIGNMENT*MP_ALIGNMENT_NUM/size

#define MP_ALLOC(size) (MP*)malloc(sizeof(MPI) +(MP_ALIGNMENT+sizeof(MBI))*MP_ALIGNMENT_NUM+1)

/*
* 内存块状态
* VALID:内存池中分配
* INVALID:系统分配
*/
enum MBS
{
	VALID,
	INVALID
};
/*
* 内存块信息
* mbs:内存状态
* mpp:内存池位置
* offset:偏移
*/
class MP;
class MBI
{
public:
	MBI() {
		mbs = INVALID;
		offset = 0;
		mpp = nullptr;
	}
	~MBI();
public:
	MBS mbs;
	MP* mpp;
	size_t offset;
};

class MB
{
public:
	MB();
	~MB();

public:
	MBI header;
	char body[0];
};
/*
* 内存池信息
* mb_size:内存块大小
* mbimp_num:内存池中内存块数量
* index:未分配的位置
* free_num:释放内存数
* mp_next:下一个同大小内存池
* mp_last:上一个同大小内存池
*/
class MPM;
class MPI
{
public:
	MPI() {
		mbimp_num = 0;
		index = 0;
		mb_size = 0;
		free_num = 0;
		mp_next = nullptr;
		mp_last = nullptr;
		mpm = nullptr;
	}
	~MPI() {
	}
	void initMPI(size_t num, size_t size,size_t offset,size_t free) {
		mbimp_num = num;
		index = offset;
		mb_size = size;
		free_num = free;
	}
	bool allowApplication()
	{
		return index > mbimp_num ? false : true;
	}
	bool allowDestory(MB* mb)
	{
		return free_num == mbimp_num ? true : false;
	}
protected:
	size_t mbimp_num;
public:
	size_t index;
	size_t mb_size;
	size_t free_num;
	MP* mp_next;
	MP* mp_last;
	MPM* mpm;
};

/*
* 内存池
* mbm:内存池管理
* mpp:内存池申请地址
*/
class MPM;
class MP
{
private:

public:
	MP() {
		
	}
	~MP() {
	}
	void* createMB(size_t size)
	{
		MB* mb =(MB*) data + size * mpi.index;
		mb->header.mbs = VALID;
		mb->header.offset = mpi.index;
		mb->header.mpp = this;
		mpi.index++;
		return mb - sizeof(MBI);
	}
	void freeMB(MB* mb)
	{
		mb->header.mbs = INVALID;
		mb->header.mpp->mpi.free_num++;
		if (mb->header.mpp->mpi.allowDestory(mb))
		{
			//MPM::instance().DestoryMP(mb->header.mpp);
			//mb->header.mpp->mpi.mpm->DestoryMP(mb->header.mpp);
		}
	}
public:
	MPI mpi;
	char data[0];
};


/*
* 内存管理
* mp:内存池
*/
class MPM
{
private:
	MP* createMP(size_t size)
	{
		//申请内存池
		MP* mp = MP_ALLOC(size);
		memcpy(mp->data, mp + sizeof(MP), MP_ALIGNMENT * MP_ALIGNMENT_NUM);
		mp->mpi.mp_next = nullptr;
		mp->mpi.mpm = this;
		//初始内化池信息
		mp->mpi.initMPI(MB_NUM(size),size, 0,0);

		auto iter = pools.find(size);
		if (iter == pools.end())
		{
			//加入
			mp->mpi.mp_last = nullptr;
			pools.insert(std::pair<size_t, MP*>(size, mp));
		}
		else
		{
			//连接
			mp->mpi.mp_last = iter->second;
			iter->second->mpi.mp_next = mp;
		}
		return mp;
	}
	void* allocMB(MP* mp,const size_t size)
	{
		/*  查看是否可以申请  */
		if (mp->mpi.allowApplication())
		{
			//初始一个内存池
			MP *mp=createMP(size);
			//分配
			return allocMB(mp,size);
		}
		/*  可以申请  */
		//分配内存块
		return mp->createMB(size);
	}
public:
	MPM()
	{

	}
	void* allocMem(size_t size)
	{
		if (MP_ALIGNMENT * MP_ALIGNMENT_NUM < size)
		{
			//大于线程池大小
			MB* mb = (MB*)malloc(size+sizeof(MBI));
			mb->header.mbs = INVALID;
			return mb - sizeof(MBI);
		}
		//计算分配的内存块大小
		size_t bsize = CACULATE_BLOCK_SZIE(size);

		MP* mp = nullptr;
		//找一个池 存储
		auto iter=pools.find(bsize);
		if (iter == pools.end())
		{
			//没有找到初始一个线程池
			mp = createMP(bsize);
		}
		else
		{
			//找到使用
			mp = iter->second;
		}
		//分配内存块
		return allocMB(mp, size);
	}
	void freeMem(void* ptr)
	{
		MB* mb = (MB*)((char *)ptr - sizeof(MBI));
		if (mb->header.mbs == INVALID)
		{
			free(mb);
		}
		else
		{
			mb->header.mpp->freeMB(mb);
		}
	}
	static MPM& instance()
	{
		static MPM mpm;
		return mpm;
	}
	void DestoryMP(MP* mp)
	{
		if (mp->mpi.mp_last == nullptr)
		{
			//表头 在线程池中删除
			pools.erase(mp->mpi.mb_size);
			if (mp->mpi.mp_next != nullptr)
			{
				//后面有池 将后面的池加入
				pools.insert(std::pair<size_t, MP*>(mp->mpi.mb_size, mp->mpi.mp_next));
				//去除链
				mp->mpi.mp_next->mpi.mp_last = nullptr;
			}
		} 
		else
		{
			//表尾
			if (mp->mpi.mp_next == nullptr)
			{
				mp->mpi.mp_last->mpi.mp_next = nullptr;
			}
			else
			{
				mp->mpi.mp_last->mpi.mp_next = mp->mpi.mp_next;
				mp->mpi.mp_next->mpi.mp_last = mp->mpi.mp_last;
			}
		}
		free(mp);
	}
private:
	std::map<size_t, MP*> pools;
};

#endif // !__MEMORY_POOL_H__



/*
* 内存池分配规则：+1
*  1. mb_size<64，分配 64×2000
*  2. 64<mb_size<128,分配 128×1000
*  3. 128<mb_size<256，分配 256×500
*  4. 256<mb_size<512，分配 512×250
*  5. 512<mb_size<1024，分配 1024×125
*  6. mb_size>1024 系统分配
* 
* 初始根据第一个要分配内存大小来建立一个内存池
*  
* 分配内存块：
*	遍历 内存池管理 中的 内存池，查看 内存池 中的 内存块管理 中的 内存块大小
*       满足;比较内存块 index 和 mb_size 
             小于：分配内存 
			 大于：再次分配一个同一个大小的内存池，在新内存池中分配
		不满足：按照内存池规则再次分配，合适的内存池，将内存分配在新内存池
*
* 释放内存块：
*  查看 内存块管理 中的已释放大小
* 
*/