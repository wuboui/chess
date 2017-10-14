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
	ACCEPT_POSTED,                     // ��־Ͷ�ݵ�Accept����
	SEND_POSTED,                       // ��־Ͷ�ݵ��Ƿ��Ͳ���
	RECV_POSTED,                       // ��־Ͷ�ݵ��ǽ��ղ���
	NULL_POSTED                        // ���ڳ�ʼ����������

}OPERATION_TYPE;
typedef struct _tagThreadParams_WORKER
{
	IOCPClass* pIOCPModel;                                   // ��ָ�룬���ڵ������еĺ���
	int         nThreadNo;                                    // �̱߳��

} THREADPARAMS_WORKER, *PTHREADPARAM_WORKER;
typedef struct _PER_IO_CONTEXT
{
	OVERLAPPED     m_Overlapped;                               // ÿһ���ص�����������ص��ṹ(���ÿһ��Socket��ÿһ����������Ҫ��һ��)              
	SOCKET         m_sockAccept;                               // ������������ʹ�õ�Socket
	WSABUF         m_wsaBuf;                                   // WSA���͵Ļ����������ڸ��ص������������� 
	char           m_szBuffer[MAX_BUFFER_LEN];                 // �����WSABUF�������ַ��Ļ�����
	OPERATION_TYPE m_OpType;                                   // ��ʶ�������������(��Ӧ�����ö��)
	size_t         m_SocketIndex;
	// ��ʼ��
	_PER_IO_CONTEXT()
	{
		ZeroMemory(&m_Overlapped, sizeof(m_Overlapped));
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
		m_sockAccept = INVALID_SOCKET;
		m_wsaBuf.buf = m_szBuffer;
		m_wsaBuf.len = MAX_BUFFER_LEN;
		m_OpType = NULL_POSTED;
	}
	// �ͷŵ�Socket
	~_PER_IO_CONTEXT()
	{
		if (m_sockAccept != INVALID_SOCKET)
		{
			closesocket(m_sockAccept);
			m_sockAccept = INVALID_SOCKET;
		}
	}
	// ���û���������
	void ResetBuffer()
	{
		ZeroMemory(m_szBuffer, MAX_BUFFER_LEN);
	}

} PER_IO_CONTEXT, *PPER_IO_CONTEXT;
//====================================================================================
//
//				��������ݽṹ�嶨��(����ÿһ����ɶ˿ڣ�Ҳ����ÿһ��Socket�Ĳ���)
//
//====================================================================================

typedef struct _PER_SOCKET_CONTEXT
{
	SOCKET      m_Socket;                                  // ÿһ���ͻ������ӵ�Socket
	SOCKADDR_IN m_ClientAddr;                              // �ͻ��˵ĵ�ַ
	std::vector<_PER_IO_CONTEXT*> m_arrayIoContext;             // �ͻ���������������������ݣ�
	// Ҳ����˵����ÿһ���ͻ���Socket���ǿ���������ͬʱͶ�ݶ��IO�����

	// ��ʼ��
	_PER_SOCKET_CONTEXT()
	{
		m_Socket = INVALID_SOCKET;
		memset(&m_ClientAddr, 0, sizeof(m_ClientAddr));
	}

	// �ͷ���Դ
	~_PER_SOCKET_CONTEXT()
	{
		if (m_Socket != INVALID_SOCKET)
		{
			closesocket(m_Socket);
			m_Socket = INVALID_SOCKET;
		}
		// �ͷŵ����е�IO����������
		for (auto  i = m_arrayIoContext.begin(); i!=m_arrayIoContext.end(); ++i )
		{
			delete *i;
			*i = NULL;
		}
		m_arrayIoContext.swap(m_arrayIoContext);
	}

	// ��ȡһ���µ�IoContext
	_PER_IO_CONTEXT* GetNewIoContext()
	{
		_PER_IO_CONTEXT* p = new _PER_IO_CONTEXT;

		m_arrayIoContext.push_back(p);
		p->m_SocketIndex = m_arrayIoContext.size() - 1;
		return p;
	}

	// ���������Ƴ�һ��ָ����IoContext
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