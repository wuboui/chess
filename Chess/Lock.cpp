#include "Lock.h"
CEvent::CEvent()
{
#ifdef WINOS  
	m_event = NULL;;
#else  
	m_condition = NULL;
#endif  
}

CEvent::~CEvent()
{

}

void CEvent::init()
{
#ifdef WINOS  
	//事件是有信号线程不阻塞，无信号阻塞线程睡眠。  
	// arg1是事件属性。  
	// arg2是手动还是自动调用ResetEvent将事件设置为无信号,SetEvent是将事件设置为有信号  
	// ResetEvent是否手动设置为无信号,WaitForSingleObject后如果是自动方式那么会自动调用ResetEvent将事件设置为无信号。  
	// arg3是初始状态信号，一般设置为FALSE无信号，让线程挂起阻塞。  
	// arg4是线程的名字。  
	m_event = CreateEvent(NULL, FALSE, FALSE, NULL);
#else  
	pthread_cond_init(&m_condition, NULL);
#endif  
}
void CEvent::release()
{
#ifdef WINOS  
	CloseHandle(m_event);
	m_event = NULL;
#else  
	pthread_cond_destroy(&m_condition);
	m_condition = NULL;
#endif  
}

void  CEvent::waite()
{
#ifdef WINOS  
	WaitForSingleObject(m_event, INFINITE);// 等待的事件，和时间  
#else  
	//会自动调用pthread_mutex_unlock(&m_mutex)释放互斥量，将当前线程挂起阻塞，等待对方线程pthread_cond_signal通知唤醒，  
	// 唤醒后pthread_cond_wait会调用pthread_mutex_lock重新锁定互斥量。  
	// pthread_cond_timedwait是阻塞一段时间。  
	pthread_cond_wait(&m_condition, &m_mutex);
#endif  
}

void CEvent::unwaite()
{
#ifdef WINOS  
	SetEvent(m_event);//设置为有信号，唤醒等待事件挂起的线程。  
#else  
	pthread_cond_signal(&m_condition);//pthread_cond_broadcast(pthread_cond_t * cond)唤醒在条件上等待的所有线程。  
#endif  
}

void CEvent::reset()
{
#ifdef WINOS  
	ResetEvent(m_event);//设置为无信号。  
#else  
	pthread_cond_signal(&m_condition);//pthread_cond_broadcast(pthread_cond_t * cond)唤醒在条件上等待的所有线程。  
#endif  
}

ILock::ILock()
{
	m_pEvent = new CEvent();
	m_pEvent->init();
}
ILock::~ILock()
{
	if (m_pEvent != NULL)
	{
		m_pEvent->release();
		//delete m_pEvent;
		m_pEvent = NULL;
	}
}

void ILock::waite()
{
	m_pEvent->waite();
}
void ILock::unwaite()
{
	m_pEvent->unwaite();
}


CMutexLock::CMutexLock()
{
	m_mutex = NULL;
#ifndef WINOS  
	m_mutexAttr == NULL;
#endif  
}

CMutexLock::~CMutexLock()
{
	release();
}
void CMutexLock::release()
{
#ifdef WINOS  
	if (m_mutex != NULL)
	{
		CloseHandle(m_mutex);//所有内核对象，或者用其它方式创建的，都可以用closeHandle将引用计数减1。  
		m_mutex = NULL;
	}

#else  
	if (m_mutexAttr != NULL)
	{
		pthread_mutexattr_destroy(&m_mutexAttr);
		m_mutexAttr = NULL;
	}

	if (m_mutex != NULL)
	{
		pthread_mutex_destroy(&m_mutex);
		m_mutex = NULL;
	}

#endif  
}

void CMutexLock::init()
{
#ifdef WINOS  
	// arg1 是NULL,互斥量用默认的安全描述信息,这个时候子进程不能继承该互斥量.  
	// arg2 是当前指明互斥量指向的线程为空,且被引用的次数是0，没有线程/进程拥有该互斥量；否则当前线程拥有该互斥量。  
	// arg3 互斥量的名字  
	m_mutex = CreateMutex(NULL, FALSE, NULL);
	DWORD dwLastError = GetLastError();
	if (dwLastError == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(m_mutex);
		m_mutex = NULL;
	}
#else  
	// arg1是初始化的互斥量，arg2是pthread_mutexattr_t属性指针，如果是NULL,那么没有线程拥有该初始化好的互斥量。  
	int nResult = pthread_mutex_init(&m_mutex, NULL);
	if (nResult == 0)
	{
		printf("pthread_mutex_init result OK.\n");
	}
	else
	{
		printf("pthread_mutex_init result error:%d\n", nResult);
	}
	pthread_mutexattr_init(&m_mutexAttr);
	// 设置 recursive 属性,使得linux下可以递归加锁，避免递归加锁死锁。  
	pthread_mutexattr_settype(&m_mutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&m_mutex, &m_mutexAttr);
#endif  
}

void CMutexLock::lock()
{
#ifdef WINOS  
	// arg2是等待毫秒时间，INFINITE是永远等待，直到该内核对象被触发可用；该函数是一个异步调用函数，互斥量拥有线程id非0，  
	// 那么该函数将被挂起阻塞，释放当前CPU拥有权，当被其它线程释放互斥量拥有线程id为0，将会唤醒当前阻塞的线程重新获取互斥量。  
	WaitForSingleObject(m_mutex, INFINITE);
	/*if(WaiteforSingleObject(m_hMutex, dwMilliSec) == WAIT_OBJECT_0)
	{
	return true;
	}
	return false;
	*/
#else  
	// 锁定互斥锁，如果该互斥锁被其它线程拥有，那么将被挂起阻塞，指定可用才回调返回；  
	// 线程自己多次锁定将会导致死锁；两个线程需要多个互斥锁相互等待对方的互斥锁，也会导致死锁。  
	pthread_mutex_lock(&m_mutex);
#endif  
}

void CMutexLock::unlock()
{
#ifdef WINOS  
	ReleaseMutex(m_mutex);// 将互斥量释放,会通知到WaitForSingleObject.  
#else  
	pthread_mutex_unlock(&m_mutex);
#endif  
}

CCriticalLock::CCriticalLock()
{
}

CCriticalLock::~CCriticalLock()
{
}

void CCriticalLock::init()
{
	InitializeCriticalSection(&m_critical);
}

void CCriticalLock::release()
{
	DeleteCriticalSection(&m_critical);
}


void CCriticalLock::lock()
{
	EnterCriticalSection(&m_critical);
}

void CCriticalLock::unlock()
{
	LeaveCriticalSection(&m_critical);
}