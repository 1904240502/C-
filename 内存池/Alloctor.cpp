#include"Alloctor.h"

//#include"MemoryMgr.hpp"
//void* operator new(size_t nSize)
//{
//	return MemoryMgr::Instance().allocMem(nSize);
//}
//
//void operator delete(void* p)
//{
//	MemoryMgr::Instance().freeMem(p);
//}
//
//void* operator new[](size_t nSize)
//{
//	return MemoryMgr::Instance().allocMem(nSize);
//}
//
//void operator delete[](void* p)
//{
//	MemoryMgr::Instance().freeMem(p);
//}

#include"mp.hpp"
void* operator new(size_t nSize)
{
	return MPM::instance().allocMem(nSize);
	
}

void operator delete(void* p)
{
	MPM::instance().freeMem(p);
}

void* operator new[](size_t nSize)
{
	return MPM::instance().allocMem(nSize);
}

void operator delete[](void* p)
{
	MPM::instance().freeMem(p);
}


void* mem_alloc(size_t size)
{
	return malloc(size);
}

void mem_free(void* p)
{
	free(p);
}

