#pragma once
#include "Common.h"
#include<stdio.h>
#include<stdlib.h>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <process.h>
#include <WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
#define  SERVER_PORT 12332 //ÕìÌý¶Ë¿ÚºÅ
#define WM_SOCKET_MESSAGE  WM_USER+12

class  TCPSocketClient 
{
public:
    bool ConnectToServer();
    int SendData();
    int SendGameData(void *pData,int NetHead,int iSize);
    LRESULT OnSocketNoti(WPARAM wParam, LPARAM lParam);
    virtual  bool HandleRecv(void * pData, int NetHead) ;
    bool Start_Socket( HWND hWnd);
	bool Stop();
	wchar_t* CodeToMsg(int iCode);
    TCPSocketClient()
    {
        sClient = NULL;
		hwnd = NULL;
    };
private:
	HWND hwnd;
	SOCKET sClient;// ¿Í»§¶Ësocket
protected:
	void _ShowMessge(wchar_t* Msg);
};