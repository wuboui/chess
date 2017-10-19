#include"ChessLogic.h"

bool ChessLogic::CanMove(int x1, int y1, int x2, int y2, ChessPieces Chess[])
{
	return true;
    switch (Chess[x1*iChessCount + y1].iType)
    {
    case Horse: return CanMove_Horse(x1, y1, x2, y2);
    default:
        break;
    }
    return false;
}

bool ChessLogic::CanMove_Horse(int x1, int y1, int x2, int y2)
{
    bool flag = false;
    if (abs(x1 - x2) == 2 && abs(y2 - y1) == 1)
        flag = true;
    else if (abs(x1 - x2) == 1  && abs(y2 - y1) == 2)
        flag = true;
    return flag;
}

bool ChessLogic::CanMove_Elephant(int x1, int y1, int x2, int y2, ChessPieces Chess[])
{
    //象斜行
    if (abs(x1 - x2) != abs(y1 - y2))
        return false;
    return CheckHavePiece(x1, y1, x2, y2, Chess);
}

bool ChessLogic::CanMove_Queue(int x1, int y1, int x2, int y2, ChessPieces Chess[])
{ 
    //王后可斜行可直行
    return (CanMove_Elephant(x1, y1, x2, y2, Chess) || CanMove_Chariot(x1, y1, x2, y2, Chess));
}

bool ChessLogic::CanMove_Chariot(int x1, int y1, int x2, int y2, ChessPieces Chess[])
{
    //车可直行
 /*   if (x1 == x2 && y1 != y2)
    {
        return CheckHavePiece(x1, y1, x2, y2, Chess);
    }
    else if(y1==y2&& x1!=x2 )
    { 
        return CheckHavePiece(x1, y1, x2, y2, Chess);
    }*/
    if (y1 != y2&&x2 != x1)
        return false;
    return CheckHavePiece(x1, y1, x2, y2, Chess);
}

bool ChessLogic::CheckHavePiece(int x1, int y1, int x2, int y2, ChessPieces Chess[])
{
    int j = 1;
    if (x2 != x1&& y2 != y1) //斜走
    {
        for (int i = x1 + y1*iChessCount ; i != x2 + y2*iChessCount; j++)
        {
            i = (x1 + ((x1 - x2) / abs(x1 - x2))*j) + (y1 + ((y1 - y2) / abs(y1 - y2))*j)*iChessCount;
            if (Chess[i].iType != 255) return false;
        }
    }
    ///////直走
    else if (x2 == x1)
    {
        for (int i = x1 + y1*iChessCount,j = 1; i != x2 + y2*iChessCount; j++)
        {
            i = x1 + (y1 + ((y1 - y2) / abs(y1 - y2))*j)*iChessCount;
            if (Chess[i].iType != 255) return false;
        }
    }
    else if (y2 == y1)
    {
        for (int i = x1 + y1*iChessCount,  j = 1; i != x2 + y2*iChessCount; j++)
        {
            i = (x1 + ((x1 - x2) / abs(x1 - x2))*j) + y1*iChessCount;
            if (Chess[i].iType != 255) return false;
        }
    }
    else;
    return true;
}

bool ChessLogic::CanMove_King(int x1, int y1, int x2, int y2, ChessPieces Chess[])
{
    if (abs(x2 - x1) > 1 && abs(y2 - y1) > 1)
        return false;
    return CanMove_Queue(x1, y1, x2, y2, Chess);
}

bool ChessLogic::CanMove_PawnFirst(int x1, int y1, int x2, int y2, ChessPieces Chess[])
{
    if (x2 != x1 && y2 != y1)
        return false;
    if (abs(x2 - x1) > 2 || abs(y1 - y2) > 2)
        return false;
    return CheckHavePiece(x1, y1, x2, y2, Chess);
}

bool ChessLogic::CanMove_Pawn(int x1, int y1, int x2, int y2, ChessPieces Chess[])
{
    if (x2 != x1 && y2 != y1)
        return false;
    if (abs(x2 - x1) > 1 || abs(y1 - y2) > 1)
        return false;
    return CheckHavePiece(x1, y1, x2, y2, Chess);
}
