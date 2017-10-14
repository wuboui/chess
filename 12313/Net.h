#include <vector>
#include<assert.h>
#include<winsock2.h>
#include<MSWSock.h>

#pragma comment(lib,"ws2_32.lib")
#define MAX_BUFFER_LEN        8192
#define RELEASE(x)                      {if(x != NULL ){delete x;x=NULL;}}
class IOCPClass;
typedef enum _OPERATION_TYPE
{
	ACCEPT_POSTED,                     // 标志投递的Accept操作
	SEND_POSTED,                       // 标志投递的是发送操作
	RECV_POSTED,                       // 标志投递的是接收操作
	NULL_POSTED                        // 用于初始化，无意义

}OPERATION_TYPE;
typedef struct _tagThreadParams_WORKER
{
	IOCPClass* pIOCPModel;                                   // 类指针，用于调用类中的函数
	int         nThreadNo;                                    // 线程编号

} THREADPARAMS_WORKER, *PTHREADPARAM_WORKER;
typedef struct _PER_IO_CONTEXT
{
	OVERLAPPED     m_Overlapped;                               // 每一个重叠网络操作的重叠结构(针对每一个Socket的每一个操作，都要有一个)              
	SOCKET         m_sockAccept;                               // 这个网络操作所使用的Socket
	WSABUF         m_wsaBuf;                                   // WSA类型的缓冲区，用于给重叠操作传参数的 
	char           m_szBuffer[MAX_BUFFER_LEN];                 // 这个是WSABUF里具体存字符的缓冲区
	OPERATION_TYPE m_OpType;                                   // 标识网络操作的类型(对应上面的枚举)
	size_t         m_SocketIndex;
	// 初始化
	_PER_IO_CONTEXT()
	{
		ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
		m_sockAccept = INVALID_SOCKET;
		m_wsaBuf.buf = m_szBuffer;
		m_wsaBuf.len = MAX_BUFFER_LEN;
		m_OpType = NULL_POSTED;
	}
	// 释放掉Socket
	~_PER_IO_CONTEXT()
	{
		if (m_sockAccept != INVALID_SOCKET)
		{
			closesocket(m_sockAccept);
			m_sockAccept = INVALID_SOCKET;
		}
	}
	// 重置缓冲区内容
	void ResetBuffer()
	{
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
	}

} PER_IO_CONTEXT, *PPER_IO_CONTEXT;
//====================================================================================
//
//				单句柄数据结构体定义(用于每一个完成端口，也就是每一个Socket的参数)
//
//====================================================================================

typedef struct _PER_SOCKET_CONTEXT
{
	SOCKET      m_Socket;                                  // 每一个客户端连接的Socket
	SOCKADDR_IN m_ClientAddr;                              // 客户端的地址
	std::vector<_PER_IO_CONTEXT*> m_arrayIoContext;             // 客户端网络操作的上下文数据，
	// 也就是说对于每一个客户端Socket，是可以在上面同时投递多个IO请求的

	// 初始化
	_PER_SOCKET_CONTEXT()
	{
		m_Socket = INVALID_SOCKET;
		memset(&m_ClientAddr, 0, sizeof(m_ClientAddr));
	}

	// 释放资源
	~_PER_SOCKET_CONTEXT()
	{
		if (m_Socket != INVALID_SOCKET)
		{
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}
		// 释放掉所有的IO上下文数据
		for (auto  i = m_arrayIoContext.begin(); i!=m_arrayIoContext.end(); ++i )
		{
			delete *i;
			*i = NULL;
		}
		m_arrayIoContext.swap(m_arrayIoContext);
	}

	// 获取一个新的IoContext
	_PER_IO_CONTEXT* GetNewIoContext()
	{
		_PER_IO_CONTEXT* p = new _PER_IO_CONTEXT;

		m_arrayIoContext.push_back(p);
		p->m_SocketIndex = m_arrayIoContext.size() - 1;
		return p;
	}

	// 从数组中移除一个指定的IoContext
	void RemoveContext(_PER_IO_CONTEXT* pContext)
	{
		assert(pContext);

		for (auto i = m_arrayIoContext.begin(); i != m_arrayIoContext.end(); ++i)
		{
			;	
			if (pContext == *i)
			{
				delete pContext;
				pContext = NULL;
				m_arrayIoContext.erase(i);
				break;
			}
		}
	}

} PER_SOCKET_CONTEXT, *PPER_SOCKET_CONTEXT;