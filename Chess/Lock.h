#ifndef _PTHREADLOCK_H_  
#define _PTHREADLOCK_H_  

#ifdef WIN32  
#define WINOS // 这里可以注释掉就是linux形式的互斥量  
#else  
#undef  WINOS  
#endif  


#ifdef WINOS  
#include <Windows.h>  
#include <process.h>   
#else  
#include <pthread.h> 
#endif

class CEvent
{
public:
	CEvent();
	~CEvent();
	void init();
	void release();
	void waite();// 当获取不到数据，那么waite挂起线程，等待其它线程通知释放  
	void unwaite();// 生产了数据那么需要调用unwaite.  
	void reset();
private:
#ifdef WINOS  
	HANDLE m_event;//事件如果有信号那么可以正常执行，如果无信号那么只能等待  
#else  
	pthread_cond_t m_condition;
#endif  
};
class ILock
{
public:
	ILock();
	virtual	~ILock();
	virtual void init() = 0;
	virtual void lock() = 0;
	virtual void unlock() = 0;
	virtual void release() = 0;
	void waite();
	void unwaite();
private:
	CEvent* m_pEvent;
};
class CMutexLock : public ILock
{
public:
	CMutexLock();
	~CMutexLock();
	void init();
	void release();
	void lock();
	void unlock();// 设计的时候，不要unwaite放置到unlock里面去，否则会导致职责不分明，如果有内部控制的还会导致无法唤醒。  
private:
#ifdef WINOS  
	HANDLE m_mutex;
#else  
	pthread_mutex_t m_mutex;
	pthread_mutexattr_t m_mutexAttr;
#endif  
};

class CCriticalLock : public ILock
{
public:
	CCriticalLock();
	~CCriticalLock();
	void init();
	void release();
	void lock();
	void unlock();
private:
	CRITICAL_SECTION m_critical;
};
#endif 
