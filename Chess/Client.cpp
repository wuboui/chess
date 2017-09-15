#include "Client.h"
bool Client::HandleRecv(void * pData, int NetHead)
{
    switch (NetHead)
    {
    case MSG_GAMEBEGIN:
    case MSG_GAMEFINSH:
    case MSG_MOVE_PIECE:
    {
        return   m_Ui->HandleMove(pData);
    }
    default:
        break;
    }
    return true;
}

bool Client::HandleMove(void * pData)
{
    return false;
}


