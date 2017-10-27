#pragma once
#include "Common.h"
#include<math.h>
class ChessLogic 
{
public:
    //判断能否走棋
    /// @param x1,y1 棋子的初始坐标
    /// @param x2,y2 落子的坐标
    /// @param Chess  棋盘
    /// @return 是否能落
    bool CanMove(int x1, int y1, int x2, int y2, ChessPieces Chess[]);
    //判断马能否移动
    /// @param x1,y1 棋子的初始坐标
    /// @param x2,y2 落子的坐标
    /// @return 是否能落
    bool CanMove_Horse(int x1, int y1, int x2, int y2);
    //判断象能否移动
    /// @param x1,y1 棋子的初始坐标
    /// @param x2,y2 落子的坐标
    /// @param Chess  棋盘
    /// @return 是否能落
    bool CanMove_Elephant(int x1, int y1, int x2, int y2, ChessPieces Chess[]);
    //判断王后能否移动
    /// @param x1,y1 棋子的初始坐标
    /// @param x2,y2 落子的坐标
    /// @param Chess  棋盘
    /// @return 是否能落
    bool CanMove_Queue(int x1, int y1, int x2, int y2, ChessPieces Chess[]);
    //判断车能否移动
    /// @param x1,y1 棋子的初始坐标
    /// @param x2,y2 落子的坐标
    /// @param Chess  棋盘
    /// @return 是否能落
    bool CanMove_Chariot(int x1, int y1, int x2, int y2, ChessPieces Chess[]);
    //检查移动中途是否有棋子
    /// @param x1,y1 棋子的初始坐标
    /// @param x2,y2 落子的坐标
    /// @param Chess  棋盘
    /// @return 是否有棋子
    bool CheckHavePiece(int x1, int y1, int x2, int y2, ChessPieces Chess[]);
    //判断国王能否移动
    /// @param x1,y1 棋子的初始坐标
    /// @param x2,y2 落子的坐标
    /// @param Chess  棋盘
    /// @return 是否能落
    bool CanMove_King(int x1, int y1, int x2, int y2, ChessPieces Chess[]);

    //判断小兵能否移动
    /// @param x1,y1 棋子的初始坐标
    /// @param x2,y2 落子的坐标
    /// @param Chess  棋盘
    /// @return 是否能落
    bool CanMove_PawnFirst(int x1, int y1, int x2, int y2, ChessPieces Chess[]);
    //判断小兵能否移动
    /// @param x1,y1 棋子的初始坐标
    /// @param x2,y2 落子的坐标
    /// @param Chess  棋盘
    /// @return 是否能落
    bool CanMove_Pawn(int x1, int y1, int x2, int y2, ChessPieces Chess[]);
};

