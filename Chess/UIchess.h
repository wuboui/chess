#pragma once
#include "Common.h"
#include<stdlib.h>

class UIChess
{
public:
    int iOldX;
    int iOldY;
	ChessPieces* ChessArr[ChessBoard];
    UIChess()
    {
        iOldY = 0;
        iOldX = 0;
    }
	bool HandleMove(void * pData);
	void initChess();
};