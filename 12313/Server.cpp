#include "Server.h"
#include <thread>
#include<stdio.h>
#include "UserInfo.h"
int count = 0;
IOCPClass::IOCPClass(void)
{
	m_hIOCP = NULL;
	m_hShutdownEvent = NULL;
	m_iThreadCount = 0;
	m_lpfnAcceptEx = NULL;
	m_lpfnGetAcceptExSockAddrs = NULL;
	m_pListenContext = NULL;
	m_GameDeskManage = new DeskManage;
}

size_t IOCPClass::GetClienIndex(_PER_IO_CONTEXT* Client)
{
	size_t i = 0;
	for (; i < m_pListenContext->m_arrayIoContext.size(); ++i)
	{
		if (Client == m_pListenContext->m_arrayIoContext.at(i)) break;
	}
    return i;
}

bool IOCPClass::_InitIOCP()
{
	m_hIOCP = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == m_hIOCP)
	{
		_ShowMessage("Create IOCP faild");
		return false;
	}
	m_iThreadCount = 2 * 2;
	m_phWorkThreads = new HANDLE[m_iThreadCount];
	for (int i = 0; i < m_iThreadCount; i++)
	{
		THREADPARAMS_WORKER* pThreadParams = new THREADPARAMS_WORKER;
		pThreadParams->pIOCPModel = this;
		pThreadParams->nThreadNo = i + 1;
		m_phWorkThreads[i] = new std::thread(std::bind(&IOCPClass::_WorkThread,this,(void *)pThreadParams));
	}
	return 1;
}
bool IOCPClass::_InitListen()
{
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	GUID GuidGetAccpetExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;

	struct sockaddr_in ServerAddress;
	m_pListenContext = new _PER_SOCKET_CONTEXT;
	m_pListenContext->m_Socket = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_pListenContext->m_Socket)
	{
		_ShowMessage("Socket Error ,ErroNo= %d  ",WSAGetLastError());
		return false;
	}
	else
	{
		_ShowMessage("WSASocket 完成  ");
	}
	if (NULL == CreateIoCompletionPort((HANDLE)m_pListenContext->m_Socket, m_hIOCP,(DWORD)m_pListenContext, NULL))
	{
		_ShowMessage("IOCP,Error,ErrorNo ",WSAGetLastError());
		return false;
	}
	else
		_ShowMessage("IOCP Success ");
	ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));
	ServerAddress.sin_addr.s_addr = inet_addr("192.168.1.32");
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_port = htons(12332);
	if (SOCKET_ERROR == bind(m_pListenContext->m_Socket, (sockaddr *)&ServerAddress, sizeof(ServerAddress)))
	{
		_ShowMessage("bind Error ErrNo=%d ",WSAGetLastError());
		return false;
	}
	else
	{
		_ShowMessage("bind Sucess  ");
	}
	if (SOCKET_ERROR == listen(m_pListenContext->m_Socket, SOMAXCONN))
	{
		_ShowMessage("Listen Error ErrNo=%d ",WSAGetLastError());
		return false;
	}
	else
		_ShowMessage("Listen Success  ");
	// 使用AcceptEx 函数，获取两个函数的指针
	DWORD dwByte = 0;
	if (SOCKET_ERROR == WSAIoctl(m_pListenContext->m_Socket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx,
		sizeof(GuidAcceptEx),
		&m_lpfnAcceptEx,
		sizeof(m_lpfnAcceptEx),
		&dwByte,
		NULL,
		NULL))
	{
		_ShowMessage("获取AcceptEx 指针失败,错误=%d  ",WSAGetLastError());
		//释放资源
		return false;
	}

	if (SOCKET_ERROR == WSAIoctl(
		m_pListenContext->m_Socket,
		SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidGetAccpetExSockAddrs,
		sizeof(GuidGetAccpetExSockAddrs),
		&m_lpfnGetAcceptExSockAddrs,
		sizeof(m_lpfnGetAcceptExSockAddrs),
		&dwByte,
		NULL,
		NULL))
	{
		_ShowMessage("获取 GuidGetAcceptExSockAddrs 指针失败,错误代码=%d ", WSAGetLastError());
		//释放资源
		return false;
	}
	for (int i = 0; i < 10/*投递Accept 最大数目 */; i++)
	{
		PER_IO_CONTEXT* pAcceptIO = m_pListenContext->GetNewIoContext();
		if (false == this->_PostAccept(pAcceptIO))
		{
			m_pListenContext->RemoveContext(pAcceptIO);
			return false;
		}
	}
	_ShowMessage("Post %d 个Accept", (int)10);
	for (int i = 0; i < 4; i++)
	{
		THREADPARAMS_WORKER* pThreadParams = new THREADPARAMS_WORKER;
		pThreadParams->pIOCPModel = this;
		pThreadParams->nThreadNo = i + 1;
		new std::thread(std::bind(&IOCPClass::_HandleRcev, this, (void *)pThreadParams));
	}
		return true;
}

bool IOCPClass::_WorkThread(LPVOID lpParam)
{
	THREADPARAMS_WORKER* pParam = (THREADPARAMS_WORKER *)lpParam;
	IOCPClass* IOCPModel = (IOCPClass *)pParam->pIOCPModel;
	int ThreadID = (int)pParam->nThreadNo;
	_ShowMessage("WorkThread Start ID:%d  ",ThreadID);
	
	DWORD dwBytes = 0;
	while (WAIT_OBJECT_0 != WaitForSingleObject(IOCPModel->m_hShutdownEvent, 0))
	{
		OVERLAPPED *pOverlapped = NULL;
		PER_SOCKET_CONTEXT *pSocketContext = NULL;
		BOOL bReaturn = GetQueuedCompletionStatus(
			IOCPModel->m_hIOCP,
			&dwBytes,
			(PULONG_PTR)&pSocketContext,	
			&pOverlapped,
			INFINITE);
		if (NULL == (DWORD)pSocketContext)
		{
			_ShowMessage("SocketContext is NULL");
			break;
		}
		if (!bReaturn)
		{
			_ShowMessage("No Return");
			//Show Error Msg
		    continue;
		}
		else
		{
			PER_IO_CONTEXT* pIOContext = CONTAINING_RECORD(pOverlapped, PER_IO_CONTEXT, m_Overlapped);
			if (0 == dwBytes && (RECV_POSTED == pIOContext->m_OpType || SEND_POSTED == pIOContext->m_OpType))
			{
				_ShowMessage("Client%s:%d Close  ",inet_ntoa(pSocketContext->m_ClientAddr.sin_addr),ntohs(pSocketContext->m_ClientAddr.sin_port));
			    //
			}
			else
			{
				switch (pIOContext->m_OpType)
				{
				case RECV_POSTED:
				   {
									IOCPModel->_DoRecv(pSocketContext, pIOContext,dwBytes);
									break;
				    }
				case ACCEPT_POSTED:
				{
									  IOCPModel->_DoAccept(pSocketContext, pIOContext);
									  break;
				}
				case SEND_POSTED:
				{
									IOCPModel->_DoSend(pSocketContext, pIOContext);
									break;
				}
				default:
					break;
				}
			}
		}
	}
	return true;
}
void IOCPClass::_HandleRcev(LPVOID lpParam)
{
	THREADPARAMS_WORKER* lparam = (THREADPARAMS_WORKER*)lpParam;
	IOCPClass* IOCPModel = lparam->pIOCPModel;
	int ThreadNo = lparam->nThreadNo;
	_ShowMessage("Recv线程启动 ID=%d", ThreadNo);
	while (WAIT_OBJECT_0 != WaitForSingleObject(IOCPModel->m_hShutdownEvent, 0))
	{
		std::unique_lock<std::mutex> uniLock(m_RecvMutex);
		if (m_qTask.empty())
		{
			m_cRecvCond.wait(uniLock);
		}
		void* pData = m_qTask.front();
		
		SocketRecv* nethead = (SocketRecv*)pData;
		_ShowMessage("1.%d,%d,id=%d,conut=%d", nethead->head->iHead, nethead->head->iMsgSize,ThreadNo,count++);
		m_qTask.pop();
		uniLock.unlock();
		/*NetHead**/ 
		_ShowMessage("1.%d,%d,id=%d,conut=%d", nethead->head->iHead, nethead->head->iMsgSize, ThreadNo, count++);
		switch (nethead->head->iHead)
		{
		case 20:
		{
				   S_C_MOVE* pMoveData = (S_C_MOVE*)nethead->pData;
				   _ShowMessage("MoveData=%d",pMoveData->x1);
				  
				   /*pMoveData->x1 *= iChessWidth;
				   pMoveData->y1 *= iChessHeight;
				   pMoveData->x2 *= iChessWidth;
				   pMoveData->y2 *= iChessHeight; */
				   pMoveData->flag = 1;
				   for (int i = 0; i < 2; ++i)
				   {
					   _SendGameData(pMoveData, sizeof(S_C_MOVE), 20,nethead->iSocketIndex );

				   }
				   break;
		}
		case 24:
		{
				   C_S_COME* pComeData = (C_S_COME*)nethead + 1;
				   //进行桌子添加
				   UserInfo* user = new UserInfo;
				   user->m_iSocketIndex = pComeData->index;
				   if (m_GameDeskManage->m_desk.empty() || m_GameDeskManage->m_desk.at(m_GameDeskManage->m_desk.size()-1)->IsFull())
				   {
					   Desk* GameDesk = new Desk;
					   GameDesk->DeskNo = 1; 
					   if(GameDesk->_setUserInfo(user))
					        m_GameDeskManage->m_desk.push_back(GameDesk);
				   }
				   else
				   {
					   int i = m_GameDeskManage->m_desk.size() - 1;
					   m_GameDeskManage->m_desk.at(i)->_setUserInfo(user);
				   }
		}
			
		default:
			break;
		}
		delete nethead;
	}
}
bool IOCPClass::_PostAccept(PER_IO_CONTEXT* pAcceptContext)
{
	assert(m_pListenContext->m_Socket);

	DWORD dwByte = 0;
	pAcceptContext->m_OpType = ACCEPT_POSTED;
	WSABUF *p_wbuf = &pAcceptContext->m_wsaBuf;
	OVERLAPPED *p_ol = &pAcceptContext->m_Overlapped;
	//为新连入的客户端准备socket
	pAcceptContext->m_sockAccept = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == pAcceptContext->m_sockAccept)
	{
		_ShowMessage("create Socket for Accept failed，error = %d  ", WSAGetLastError());
		return false;
	}

	if (FALSE == m_lpfnAcceptEx(m_pListenContext->m_Socket, pAcceptContext->m_sockAccept, p_wbuf->buf, p_wbuf->len - ((sizeof(SOCKADDR_IN)+16) * 2),
		sizeof(SOCKADDR_IN)+16, sizeof(SOCKADDR_IN)+16, &dwByte, p_ol))
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			_ShowMessage("Post Accept failed, Error= %d ", WSAGetLastError());
			return false;
		}
	}
	return true;
}

bool IOCPClass::_PostRecv(PER_IO_CONTEXT* pIoContext)
{
	DWORD dwFlag = 0;
	DWORD dwBytes = 0;
	WSABUF *p_wbuf = &pIoContext->m_wsaBuf;
	pIoContext->m_wsaBuf.buf += pIoContext->m_recvBuffLen;
	OVERLAPPED* p_ol = &pIoContext->m_Overlapped;
	//pIoContext->ResetBuffer();
	pIoContext->m_OpType = RECV_POSTED;
	if (SOCKET_ERROR == WSARecv(pIoContext->m_sockAccept, p_wbuf, 1, &dwBytes, &dwFlag, p_ol, NULL))
	{
		if (WSA_IO_PENDING != WSAGetLastError())
		{
			_ShowMessage("Post Recv Error,ErrorNo= %d  ", WSAGetLastError());
			return false;
		}
	}
	m_Socket.push_back(pIoContext->m_sockAccept);
	_ShowMessage("PostRecv Success");
	return true;
}

bool IOCPClass::_DoRecv(PER_SOCKET_CONTEXT *pSockerContext, PER_IO_CONTEXT* pIoContext,DWORD bytes)
{
	
	sockaddr_in* Client_addr = &pSockerContext->m_ClientAddr;
	_ShowMessage("收到  %s:%d", inet_ntoa(Client_addr->sin_addr), ntohs(Client_addr->sin_port));

	DWORD dwRecvd = 0;
	do 
	{
		/*char* pRecvBufeer = new char[8];
		int recvcod = ::recv(pSockerContext->m_Socket, (char*)pRecvBufeer, 8, 0);
		NetHead* pData = (NetHead*)pRecvBufeer;
		_ShowMessage("NetHead=%d,size=%d,%d", pData->iHead, pData->iMsgSize, recvcod);
		dwRecvd += recvcod;
		recvcod = ::recv(pSockerContext->m_Socket, (char*)(pData+1), 17, 0);
		dwRecvd += recvcod;*/
		/*switch (pData->iHead)
		{
		case 20:
		default:
		break;
		}*/
		
		_ShowMessage("收到数据:%s,", pIoContext->m_szBuffer + pIoContext->m_recvBuffLen);
		NetHead* pData = (NetHead*)(pIoContext->m_szBuffer + pIoContext->m_recvBuffLen + dwRecvd);
		SocketRecv* RecvData = new SocketRecv;
		RecvData->head = pData;
		RecvData->iSocketIndex = GetSocketIndex(pIoContext->m_sockAccept);
		RecvData->pData = (char *)(pData+ 1);
		_ShowMessage("NetHead=%d,size=%d", pData->iHead, pData->iMsgSize);
		_AddRecvTask(RecvData);
		switch (pData->iHead)
		{
		case 23:
		{

				   C_S_COME* pCome = new C_S_COME;
				   pCome->index = GetClienIndex(pIoContext);
				   /*S_C_MOVE * pMoveData = (S_C_MOVE*)(pIoContext->m_szBuffer + dwRecvd+8);
				  / * pMoveData->x1 *= iChessWidth;
				   pMoveData->y1 *= iChessHeight;
				   pMoveData->x2 *= iChessWidth;
				   pMoveData->y2 *= iChessHeight;* /
				   pMoveData->flag = 1;
				   _SendGameData(pMoveData, sizeof(S_C_MOVE), 20, pIoContext->m_sockAccept);*/
				   _SendGameData(pCome, sizeof(C_S_COME), MSG_COME_IN, pIoContext->m_sockAccept);
		}
			
		default:
		break;
		}
		dwRecvd += (sizeof(NetHead)+pData->iMsgSize);
		//_AddTask(pData);
	} while (dwRecvd<bytes);
	pIoContext->m_recvBuffLen += bytes;
	return _PostRecv(pIoContext);
}

bool IOCPClass::_DoAccept(PER_SOCKET_CONTEXT *pSockerContext, PER_IO_CONTEXT* pIoContext)
{
	SOCKADDR_IN* ClientAddr = NULL;
	SOCKADDR_IN* LocalAddr = NULL;
	int iRemoteLen = sizeof(SOCKADDR_IN), iLocalLen = sizeof(SOCKADDR_IN);
	m_lpfnGetAcceptExSockAddrs(pIoContext->m_wsaBuf.buf, pIoContext->m_wsaBuf.len - (iLocalLen + 16) * 2,
		iLocalLen + 16, iRemoteLen + 16, (LPSOCKADDR*)&LocalAddr, &iLocalLen,(LPSOCKADDR*)& ClientAddr,&iRemoteLen);
	_ShowMessage("Client Addr=%s:%d Accept  ", inet_ntoa(ClientAddr->sin_addr), ntohs(ClientAddr->sin_port));
	_ShowMessage("Client Addr=%s:%d,Msg=%s  ", inet_ntoa(ClientAddr->sin_addr), ntohs(ClientAddr->sin_port), pIoContext->m_wsaBuf.buf);
	PER_SOCKET_CONTEXT* pNewAcceptContext = new PER_SOCKET_CONTEXT; 
	pNewAcceptContext->m_Socket = pIoContext->m_sockAccept;
	memcpy_s(&(pNewAcceptContext->m_ClientAddr), sizeof(SOCKADDR_IN), ClientAddr, sizeof(SOCKADDR_IN));
	if (false == _BindIOCP(pNewAcceptContext))
	{
		RELEASE(pNewAcceptContext);
		return false;
	}

	PER_IO_CONTEXT* pNewIoContext = pNewAcceptContext->GetNewIoContext();
	pNewIoContext->m_OpType = RECV_POSTED;
	pNewIoContext->m_sockAccept = pNewAcceptContext->m_Socket;

	if (false == _PostRecv(pNewIoContext))
	{
		_ShowMessage("Post Recv Error,ErrNo= %d ",WSAGetLastError());
		pNewAcceptContext->RemoveContext(pNewIoContext);
	}
	
	return _PostAccept(pIoContext);
}

bool IOCPClass::_DoSend(PER_SOCKET_CONTEXT *pSockerContext, PER_IO_CONTEXT* pIoContext)
{
	return true;
}

bool IOCPClass::_BindIOCP(PER_SOCKET_CONTEXT *pSocketContext)
{
	HANDLE hTemp = CreateIoCompletionPort((HANDLE)pSocketContext->m_Socket, m_hIOCP, (DWORD)pSocketContext, 0);

	if (NULL == hTemp)
	{
		_ShowMessage("执行CreateIoCompletionPort()出现错误.错误代码：%d", GetLastError());
		return false;
	}

	return true;
}


bool IOCPClass::_SendGameData(LPVOID lpData, int nSize, int iNetHead,SOCKET sClient)
{
	if (SOCKET_ERROR != sClient)
	{
		char SendBuff[1500];
		int iSendSize = sizeof(NetHead);
		memset(SendBuff, '\0', 1500);
		NetHead nethead;
		nethead.iHead = iNetHead;
		nethead.iMsgSize = nSize;
		iSendSize += nSize;
		memcpy_s(SendBuff, sizeof(NetHead), &nethead, sizeof(nethead));
		if (nSize > 0)
			CopyMemory(SendBuff + sizeof(NetHead), lpData, nSize);
		int iSended = 0;
		int iSendCount = 0;
		do
		{
			int ret = ::send(sClient, (char *)SendBuff + iSended, iSendSize - iSended, 0);
			if (ret == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSAEWOULDBLOCK) //网络有阻塞
				{
					if (iSendCount++ > 100)//判断重发次数是否超过100次
						return SOCKET_ERROR;//直接返回 错误
					else
					{
						Sleep(10);	//等待 10 ms，和上面的重发100次，
						continue;   //重发数据
					}
				}
				else
					return SOCKET_ERROR;
			}
			iSended += ret;
			iSendCount = 0;
		} while (iSended < iSendSize);
		return true;
	}
	else
	{
		_ShowMessage("Send Faild");
		return false;
	}
}

bool IOCPClass::_SendGameData(LPVOID lpData, int nSize, int iNetHead, int iIndex)
{
	SOCKET sClient = GetClienSocket(iIndex);
	if (SOCKET_ERROR != sClient)
	{
		char SendBuff[1500];
		int iSendSize = sizeof(NetHead);
		memset(SendBuff, '\0', 1500);
		NetHead nethead;
		nethead.iHead = iNetHead;
		nethead.iMsgSize = nSize;
		iSendSize += nSize;
		memcpy_s(SendBuff, sizeof(NetHead), &nethead, sizeof(nethead));
		if (nSize > 0)
			CopyMemory(SendBuff + sizeof(NetHead), lpData, nSize);
		int iSended = 0;
		int iSendCount = 0;
		do
		{
			int ret = ::send(sClient, (char *)SendBuff + iSended, iSendSize - iSended, 0);
			if (ret == SOCKET_ERROR)
			{
				if (::WSAGetLastError() == WSAEWOULDBLOCK) //网络有阻塞
				{
					if (iSendCount++ > 100)//判断重发次数是否超过100次
						return SOCKET_ERROR;//直接返回 错误
					else
					{
						Sleep(10);	//等待 10 ms，和上面的重发100次，
						continue;   //重发数据
					}
				}
				else
					return SOCKET_ERROR;
			}
			iSended += ret;
			iSendCount = 0;
		} while (iSended < iSendSize);
		return true;
	}
	else
	{
		_ShowMessage("Send Faild");
		return false;
	}
}

void IOCPClass::_ShowMessage(const char* szFormat, ...) const
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	char* cMsg = new char[200];
	memset(cMsg, '\0', 200);
	
	va_list arglist;
	va_start(arglist, szFormat);
	sprintf(cMsg, "[%2d:%2d:]", time.wMinute, time.wSecond);
	vsprintf(cMsg+8,szFormat, arglist);
	if (NULL != hwnd)
	{
		SendMessageA(GetDlgItem(hwnd, 1022), LB_ADDSTRING, 0, (LPARAM)cMsg);
		int i=SendMessageA(GetDlgItem(hwnd, 1022), LB_GETCOUNT, NULL, NULL);
		SendMessageA(GetDlgItem(hwnd, 1022), LB_SETCURSEL, i - 1, NULL);
	}
		
}

void IOCPClass::_AddRecvTask(void* pData)
{
	
	m_qTask.push(pData);
	m_cRecvCond.notify_one();
}

void IOCPClass::SetHwnd(HWND hWnd)
{
	this->hwnd = hWnd;
}

void IOCPClass::SetRecvTask(std::queue<void*> &Queue, std::mutex &mutex, std::condition_variable &cond)
{
	this->m_qTask = Queue;
}


bool IOCPClass::Start()
{
	m_hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (false == LoadSocketlib())
	{
		_ShowMessage("Load Failed ");
		return false;
	}
	if (false == this->_InitIOCP())
	{
		_ShowMessage("InitIOCP failed ");
		return false;
	}
	else
		_ShowMessage("InitIOCP Success  ");
	if (false == this->_InitListen())
	{
		_ShowMessage("InitListen failed ");
		return false;
	}
	else
		_ShowMessage("Init Listen Success ");
	return true;
}

void IOCPClass::Stop()
{
	while (m_pListenContext !=NULL && m_pListenContext->m_Socket !=INVALID_SOCKET)
	{
		SetEvent(m_hShutdownEvent);

		for (int i = 0; i < 4; i++)
		{
			// 通知所有的完成端口操作退出
			PostQueuedCompletionStatus(m_hIOCP, 0, (DWORD)NULL, NULL);
		}

		// 等待所有的客户端资源退出
		WaitForMultipleObjects(4, m_phWorkThreads, TRUE, INFINITE);
		m_pListenContext = NULL;
	} 
}

bool IOCPClass::LoadSocketlib()
{
	WSADATA wsaData;
	int nResult;
	nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	// 错误(一般都不可能出现)
	if (NO_ERROR != nResult)
	{
		_ShowMessage("初始化WinSock 2.2失败！ ");
		return false;
	}

	return true;
}
/*

int main()
{
	IOCPClass* IOCpModel = new IOCPClass;
	IOCpModel->Start();
	IOCpModel->Stop();
	return 0;
}*/