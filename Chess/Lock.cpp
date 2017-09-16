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
	//�¼������ź��̲߳����������ź������߳�˯�ߡ�  
	// arg1���¼����ԡ�  
	// arg2���ֶ������Զ�����ResetEvent���¼�����Ϊ���ź�,SetEvent�ǽ��¼�����Ϊ���ź�  
	// ResetEvent�Ƿ��ֶ�����Ϊ���ź�,WaitForSingleObject��������Զ���ʽ��ô���Զ�����ResetEvent���¼�����Ϊ���źš�  
	// arg3�ǳ�ʼ״̬�źţ�һ������ΪFALSE���źţ����̹߳���������  
	// arg4���̵߳����֡�  
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
	WaitForSingleObject(m_event, INFINITE);// �ȴ����¼�����ʱ��  
#else  
	//���Զ�����pthread_mutex_unlock(&m_mutex)�ͷŻ�����������ǰ�̹߳����������ȴ��Է��߳�pthread_cond_signal֪ͨ���ѣ�  
	// ���Ѻ�pthread_cond_wait�����pthread_mutex_lock����������������  
	// pthread_cond_timedwait������һ��ʱ�䡣  
	pthread_cond_wait(&m_condition, &m_mutex);
#endif  
}

void CEvent::unwaite()
{
#ifdef WINOS  
	SetEvent(m_event);//����Ϊ���źţ����ѵȴ��¼�������̡߳�  
#else  
	pthread_cond_signal(&m_condition);//pthread_cond_broadcast(pthread_cond_t * cond)�����������ϵȴ��������̡߳�  
#endif  
}

void CEvent::reset()
{
#ifdef WINOS  
	ResetEvent(m_event);//����Ϊ���źš�  
#else  
	pthread_cond_signal(&m_condition);//pthread_cond_broadcast(pthread_cond_t * cond)�����������ϵȴ��������̡߳�  
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
		CloseHandle(m_mutex);//�����ں˶��󣬻�����������ʽ�����ģ���������closeHandle�����ü�����1��  
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
	// arg1 ��NULL,��������Ĭ�ϵİ�ȫ������Ϣ,���ʱ���ӽ��̲��ܼ̳иû�����.  
	// arg2 �ǵ�ǰָ��������ָ����߳�Ϊ��,�ұ����õĴ�����0��û���߳�/����ӵ�иû�����������ǰ�߳�ӵ�иû�������  
	// arg3 ������������  
	m_mutex = CreateMutex(NULL, FALSE, NULL);
	DWORD dwLastError = GetLastError();
	if (dwLastError == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(m_mutex);
		m_mutex = NULL;
	}
#else  
	// arg1�ǳ�ʼ���Ļ�������arg2��pthread_mutexattr_t����ָ�룬�����NULL,��ôû���߳�ӵ�иó�ʼ���õĻ�������  
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
	// ���� recursive ����,ʹ��linux�¿��Եݹ����������ݹ����������  
	pthread_mutexattr_settype(&m_mutexAttr, PTHREAD_MUTEX_RECURSIVE_NP);
	pthread_mutex_init(&m_mutex, &m_mutexAttr);
#endif  
}

void CMutexLock::lock()
{
#ifdef WINOS  
	// arg2�ǵȴ�����ʱ�䣬INFINITE����Զ�ȴ���ֱ�����ں˶��󱻴������ã��ú�����һ���첽���ú�����������ӵ���߳�id��0��  
	// ��ô�ú������������������ͷŵ�ǰCPUӵ��Ȩ�����������߳��ͷŻ�����ӵ���߳�idΪ0�����ỽ�ѵ�ǰ�������߳����»�ȡ��������  
	WaitForSingleObject(m_mutex, INFINITE);
	/*if(WaiteforSingleObject(m_hMutex, dwMilliSec) == WAIT_OBJECT_0)
	{
	return true;
	}
	return false;
	*/
#else  
	// ����������������û������������߳�ӵ�У���ô��������������ָ�����òŻص����أ�  
	// �߳��Լ�����������ᵼ�������������߳���Ҫ����������໥�ȴ��Է��Ļ�������Ҳ�ᵼ��������  
	pthread_mutex_lock(&m_mutex);
#endif  
}

void CMutexLock::unlock()
{
#ifdef WINOS  
	ReleaseMutex(m_mutex);// ���������ͷ�,��֪ͨ��WaitForSingleObject.  
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