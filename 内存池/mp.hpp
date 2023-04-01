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
* �ڴ��״̬
* VALID:�ڴ���з���
* INVALID:ϵͳ����
*/
enum MBS
{
	VALID,
	INVALID
};
/*
* �ڴ����Ϣ
* mbs:�ڴ�״̬
* mpp:�ڴ��λ��
* offset:ƫ��
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
* �ڴ����Ϣ
* mb_size:�ڴ���С
* mbimp_num:�ڴ�����ڴ������
* index:δ�����λ��
* free_num:�ͷ��ڴ���
* mp_next:��һ��ͬ��С�ڴ��
* mp_last:��һ��ͬ��С�ڴ��
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
* �ڴ��
* mbm:�ڴ�ع���
* mpp:�ڴ�������ַ
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
* �ڴ����
* mp:�ڴ��
*/
class MPM
{
private:
	MP* createMP(size_t size)
	{
		//�����ڴ��
		MP* mp = MP_ALLOC(size);
		memcpy(mp->data, mp + sizeof(MP), MP_ALIGNMENT * MP_ALIGNMENT_NUM);
		mp->mpi.mp_next = nullptr;
		mp->mpi.mpm = this;
		//��ʼ�ڻ�����Ϣ
		mp->mpi.initMPI(MB_NUM(size),size, 0,0);

		auto iter = pools.find(size);
		if (iter == pools.end())
		{
			//����
			mp->mpi.mp_last = nullptr;
			pools.insert(std::pair<size_t, MP*>(size, mp));
		}
		else
		{
			//����
			mp->mpi.mp_last = iter->second;
			iter->second->mpi.mp_next = mp;
		}
		return mp;
	}
	void* allocMB(MP* mp,const size_t size)
	{
		/*  �鿴�Ƿ��������  */
		if (mp->mpi.allowApplication())
		{
			//��ʼһ���ڴ��
			MP *mp=createMP(size);
			//����
			return allocMB(mp,size);
		}
		/*  ��������  */
		//�����ڴ��
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
			//�����̳߳ش�С
			MB* mb = (MB*)malloc(size+sizeof(MBI));
			mb->header.mbs = INVALID;
			return mb - sizeof(MBI);
		}
		//���������ڴ���С
		size_t bsize = CACULATE_BLOCK_SZIE(size);

		MP* mp = nullptr;
		//��һ���� �洢
		auto iter=pools.find(bsize);
		if (iter == pools.end())
		{
			//û���ҵ���ʼһ���̳߳�
			mp = createMP(bsize);
		}
		else
		{
			//�ҵ�ʹ��
			mp = iter->second;
		}
		//�����ڴ��
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
			//��ͷ ���̳߳���ɾ��
			pools.erase(mp->mpi.mb_size);
			if (mp->mpi.mp_next != nullptr)
			{
				//�����г� ������ĳؼ���
				pools.insert(std::pair<size_t, MP*>(mp->mpi.mb_size, mp->mpi.mp_next));
				//ȥ����
				mp->mpi.mp_next->mpi.mp_last = nullptr;
			}
		} 
		else
		{
			//��β
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
* �ڴ�ط������+1
*  1. mb_size<64������ 64��2000
*  2. 64<mb_size<128,���� 128��1000
*  3. 128<mb_size<256������ 256��500
*  4. 256<mb_size<512������ 512��250
*  5. 512<mb_size<1024������ 1024��125
*  6. mb_size>1024 ϵͳ����
* 
* ��ʼ���ݵ�һ��Ҫ�����ڴ��С������һ���ڴ��
*  
* �����ڴ�飺
*	���� �ڴ�ع��� �е� �ڴ�أ��鿴 �ڴ�� �е� �ڴ����� �е� �ڴ���С
*       ����;�Ƚ��ڴ�� index �� mb_size 
             С�ڣ������ڴ� 
			 ���ڣ��ٴη���һ��ͬһ����С���ڴ�أ������ڴ���з���
		�����㣺�����ڴ�ع����ٴη��䣬���ʵ��ڴ�أ����ڴ���������ڴ��
*
* �ͷ��ڴ�飺
*  �鿴 �ڴ����� �е����ͷŴ�С
* 
*/