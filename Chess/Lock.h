#ifndef _PTHREADLOCK_H_  
#define _PTHREADLOCK_H_  

#ifdef WIN32  
#define WINOS // �������ע�͵�����linux��ʽ�Ļ�����  
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
	void waite();// ����ȡ�������ݣ���ôwaite�����̣߳��ȴ������߳�֪ͨ�ͷ�  
	void unwaite();// ������������ô��Ҫ����unwaite.  
	void reset();
private:
#ifdef WINOS  
	HANDLE m_event;//�¼�������ź���ô��������ִ�У�������ź���ôֻ�ܵȴ�  
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
	void unlock();// ��Ƶ�ʱ�򣬲�Ҫunwaite���õ�unlock����ȥ������ᵼ��ְ�𲻷�����������ڲ����ƵĻ��ᵼ���޷����ѡ�  
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
