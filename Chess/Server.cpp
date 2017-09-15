#pragma comment(lib,"ws2_32")
#include <windows.h>
#include<WinSock2.h>
#include <process.h>
#include <stdio.h>
#define  SERVER_PORT 4567

#define  READ 1
#define  WRITE 2

typedef struct 
{
    SOCKET s_client;
    sockaddr_in client_address;
}CLIENT,*PCLIENT;

typedef struct
{
    OVERLAPPED ol;
    char recv_data[100];
    int op;
}IO_RECV,*P_IORECV;

bool InitVersion()
{
    WSADATA data;
    unsigned int sock_version = MAKEWORD(2, 2);
    if (::WSAStartup(sock_version, &data) != 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}


void releas_version()
{
    ::WSACleanup();
}

static  HANDLE g_completion = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, 0, 0, 0);

//处理客户端
unsigned int __stdcall deal_client(void *p)
{
    P_IORECV temp_overlappeed;
    PCLIENT temp_client;
    DWORD trans_len;
    while (1)
    {
        BOOL b = ::GetQueuedCompletionStatus(g_completion, &trans_len, (unsigned long *)&temp_client, (OVERLAPPED **)&temp_overlappeed, WSA_INFINITE);
        if (b)
        {
            OutputDebugString("读取数据\n");
            if (temp_overlappeed->op == READ)
            {
                temp_overlappeed->recv_data[trans_len] = 0;
                printf("%s\n=====", temp_overlappeed->recv_data);

                WSABUF buf;
                buf.buf = temp_overlappeed->recv_data;
                buf.len = 100;
                DWORD dwRecv;
                DWORD flg = 0;
                temp_overlappeed->op = READ;
                ::WSARecv(temp_client->s_client, &buf,1, &trans_len, &flg, &temp_overlappeed->ol
                    , NULL);
                printf("%d", WSAGetLastError());
            }
        }
    }
    return 0;
}

void 