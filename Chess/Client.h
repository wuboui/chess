#pragma once
#include<stdio.h>
#include<stdlib.h>
#include "Common.h"
#include "UIchess.h"
#include "ChessLogic.h"
#include "Net.h"
class UIChess;
/*
#include<gdiplus.h>
#pragma comment(lib,"gdiplus.lib")*/
class Client :public TCPSocketClient
{
public:
    UIChess* m_Ui;
    virtual bool  HandleRecv(void * pData, int NetHead);

   virtual bool HandleMove(void *pData) ;
   Client(UIChess* ui) 
   {
       m_Ui = ui;
   };
};
