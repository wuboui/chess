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
private:
	HANDLE   m_hShutdownEvent;
	HANDLE m_hIOCP;
	HANDLE* m_phWorkThreads;
	HWND hwnd;
	int m_iThreadCount;
	_PER_SOCKET_CONTEXT* m_pListenContext;
	LPFN_ACCEPTEX                m_lpfnAcceptEx;                // AcceptEx 和 GetAcceptExSockaddrs 的函数指针，用于调用这两个扩展函数
	LPFN_GETACCEPTEXSOCKADDRS    m_lpfnGetAcceptExSockAddrs;
public:
	bool Start();
	void Stop();
	bool LoadSocketlib();
	void SetHwnd(HWND hWnd);
	IOCPClass(void);
};
