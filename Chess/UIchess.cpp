
#include "UIchess.h"

WCHAR * wFileName[18] =
{
    L"png/blackhorse.png",
    L"png/blackhorse.png",
    L"png/blackhorse.png",
    L"png/blackhorse.png",
    L"png/blackhorse.png",
    L"png/blackhorse.png",
    L"png/blackhorse.png",
    L"png/blackhorse.png",
    L"png/blackpawn.png",
    L"png/blackhorse.png",
    L"png/blackhorse.png",
    L"png/blackhorse.png",
    L"png/blackhorse.png",
    L"png/blackhorse.png",
    L"png/blackhorse.png",
    L"png/blackhorse.png",
    L"png/blackhorse.png",
    L"png/whitepawn.png",
};

void UIChess::initChess()
{
    bool bChange = false;
    for (int i = 0; i < ChessBoard; i++)
    {
        if (i < 8)
        {
            ChessArr[i] = new ChessPieces(wFileName[i], iChessWidth, iChessHeight, iStartPosX + (i % 8)*iChessWidth, iStartPosY + (i / 8)*iChessHeight);
        }
        else if (i >= 8 && i < 16)
        {
            ChessArr[i] = new ChessPieces(wFileName[8], iChessWidth, iChessHeight, iStartPosX + (i % 8)*iChessWidth, iStartPosY + (i / 8)*iChessHeight);
        }
        else if (i >= 56 && i < 64)
        {
            ChessArr[i] = new ChessPieces(wFileName[i - 48], iChessWidth, iChessHeight, iStartPosX + (i % 8)*iChessWidth, iStartPosY + (i / 8)*iChessHeight);
			
        }
        else if (i >= 48 && i < 56)
        {
            ChessArr[i] = new ChessPieces(wFileName[17], iChessWidth, iChessHeight, iStartPosX + (i % 8)*iChessWidth, iStartPosY + (i / 8)*iChessHeight);
        }
        else
        {
            ChessArr[i] = new ChessPieces(NULL, iChessWidth, iChessHeight, iStartPosX + (i % 8)*iChessWidth, iStartPosY + (i / 8)*iChessHeight);
        }
        if (bChange &&i % 2 == 0)
        {
            ChessArr[i]->bDrawBG = true;
        }
        else if (i % 2 != 0 && !bChange)
        {
            ChessArr[i]->bDrawBG = true;
        }
        else;
        if ((i + 1) % 8 == 0) bChange = !bChange;
    }
}

bool UIChess::HandleMove(void * pData)
{
	S_C_MOVE * pMoveData = (S_C_MOVE *)pData;

        int iIndex = 0;
		pMoveData->flag = 1;
        if (pMoveData->flag)
        {
            
            if (pMoveData->x1 < iChessCount  && pMoveData->y1 < iChessCount  && pMoveData->x1 >= 0&&pMoveData->y1 >= 0)
            {

				iIndex = pMoveData->x2   + pMoveData->y2 * iChessCount;
                int iClickIndex= pMoveData->x1  + pMoveData->y1 * iChessCount;
                if (iClickIndex != iIndex && iClickIndex != 255)
                {
                    ChessArr[iClickIndex]->bClick = false;
                    Gdiplus::Image* temp = ChessArr[iClickIndex]->IImage;
                    ChessArr[iClickIndex]->IImage = ChessArr[iIndex]->IImage;
                    ChessArr[iIndex]->IImage = temp;

                }
                iClickIndex = iIndex;
                ChessArr[iIndex]->bClick = true;
            }
   
        }
        else
        {
        }
        return true;
		
}
