#include "Net.h"


bool TCPSocketClient::ConnectToServer()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int ret;
   // SOCKET sClient;
    struct sockaddr_in saServer;
    //char *ptr;
    BOOL fSuccess = TRUE;
    //WinSock 初始化
    wVersionRequested = MAKEWORD(2, 2);
    ret = WSAStartup(wVersionRequested, &wsaData);
    if (ret != 0)
    {
		_ShowMessge(L"Start Error");
		return false;
    }
    sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    //确认winSock DLL支持版本2.2
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        WSACleanup();
		_ShowMessge(L"Version Invalid");
		return false;
    }
	if (::WSAAsyncSelect(sClient, hwnd, WM_SOCKET_MESSAGE, FD_READ | FD_CONNECT | FD_CLOSE) == SOCKET_ERROR)
	{
		_ShowMessge(L"Select Error");
		return false;
	}
    //构建服务器地址信息
    saServer.sin_family = AF_INET;
    saServer.sin_port = htons(SERVER_PORT);
    saServer.sin_addr.S_un.S_addr=inet_addr("192.168.1.32");
    //HANDLE hThread = (HANDLE)::_beginthreadex(NULL, 0, readThread, (void *)sClient, 0, NULL);


    //连接服务器
    ret = ::connect(sClient, (struct sockaddr *)&saServer, sizeof(saServer));
	/*if (SOCKET_ERROR == ret)
	{
		return ErConnectError;
	}
	*/
    return true;
}



int TCPSocketClient::SendData()
{
    char sendMessage[] = "hello Server";
	
		if (send(sClient, (char *)&sendMessage, sizeof(sendMessage), 0) == SOCKET_ERROR)
		{
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				_ShowMessge(L"Send Faild");
				return false;
			}
			
		}
		else;
	
   
    return true;
}

int TCPSocketClient::SendGameData(void * pData, int iNetHead, int iSize)
{
    if (sClient != SOCKET_ERROR)
    {
        char SendBuff[1500];
		memset(SendBuff, '\0', 1500);
        NetHead nHead;
        nHead.iHead = iNetHead;
        nHead.iMsgSize = iSize;
        int iSendCount = 0;
        int iSendSize = iSize + sizeof(NetHead);
		memcpy(SendBuff, &nHead, sizeof(NetHead));
        if (iSize > 0)
            CopyMemory(SendBuff + sizeof(NetHead), pData, iSize);
        int iSended = 0;
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
		_ShowMessge(L"Send Faild");
		return false;
	}	
}

LRESULT TCPSocketClient::OnSocketNoti(WPARAM wParam, LPARAM lParam)
{
	WORD ret = WSAGETSELECTEVENT(lParam);
    switch (WSAGETSELECTEVENT(lParam))
    {
    case FD_CLOSE:
    case FD_CONNECT:
    {
					   UINT uErrorCode = WSAGETSELECTERROR(lParam);
					   if (uErrorCode != 0)  closesocket(sClient);
					  
					   return 0;
        break;
    }
    case FD_READ:
    {
        char buffer[1500];
        memset(buffer, '\0', 1500);
        int iLen = ::recv(sClient, buffer, 1500, 0);
        NetHead* pNetData = (NetHead *)buffer;
		pNetData->iHead = MSG_MOVE_PIECE;
        /*if(pNetData->iMsgSize > ) 考虑分片传输*/
        this->HandleRecv(pNetData+1,pNetData->iHead);
        break;
    }
    }
    return 0;
}

bool TCPSocketClient::HandleRecv(void * pData, int NetHead)
{
    return true;
}

bool TCPSocketClient::Start_Socket( HWND hWnd)
{
	this->hwnd = hWnd;
    ConnectToServer();
	return true;
}

bool TCPSocketClient::Stop()
{
	closesocket(sClient);
	return true;
}

void TCPSocketClient::_ShowMessge(wchar_t* Msg)
{
	if (this->hwnd != NULL)
	MessageBox(this->hwnd, Msg, L"Notice", MB_OK);
}
