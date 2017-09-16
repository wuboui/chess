#include "Net.h"
#include "Common.h"
#include <cstring>

class IOCPClass
{
protected:
	bool _InitIOCP();
	bool _InitListen();
	bool  _WorkThread(LPVOID lpParam);
	bool _PostAccept(PER_IO_CONTEXT* pAcceptContext);
	bool _PostRecv(PER_IO_CONTEXT* pIoContext);
	bool _DoRecv(PER_SOCKET_CONTEXT *pSockerContext,PER_IO_CONTEXT* pIoContext);
	bool _DoAccept(PER_SOCKET_CONTEXT *pSockerContext, PER_IO_CONTEXT* pIoContext);
	bool _DoSend(PER_SOCKET_CONTEXT *pSockerContext, PER_IO_CONTEXT* pIoContext);
	bool _BindIOCP(PER_SOCKET_CONTEXT *pSocketContext);
	void _ShowMessage(const char* szFormat, ...) const;
	void _AddTask(void* pData);
	void _HandleRcev(LPVOID lpParam);
private:
	HANDLE   m_hShutdownEvent;
	HANDLE m_hIOCP;
	HANDLE* m_phWorkThreads;
	HWND hwnd;
	int m_iThreadCount;
	_PER_SOCKET_CONTEXT* m_pListenContext;
	LPFN_ACCEPTEX                m_lpfnAcceptEx;                // AcceptEx �� GetAcceptExSockaddrs �ĺ���ָ�룬���ڵ�����������չ����
	LPFN_GETACCEPTEXSOCKADDRS    m_lpfnGetAcceptExSockAddrs;
	std::queue<void* >      m_qTask;
	std::mutex              m_RecvMutex;
	std::condition_variable      m_cRecvCond;
	std::mutex                   m_RecvMutex;
	std::queue<void*>            m_qRecvTask;
	
public:
	bool Start();
	void Stop();
	bool LoadSocketlib();
	void SetHwnd(HWND hWnd);
	void SetRecvTask(std::queue<void*> &Queue,std::mutex &mutex,std::condition_variable &cond);
	IOCPClass(void);
	std::condition_variable  m_cRecvCond;
};