#pragma once
#include "Common.h"
#include<math.h>
class ChessLogic 
{
public:
    //�ж��ܷ�����
    /// @param x1,y1 ���ӵĳ�ʼ����
    /// @param x2,y2 ���ӵ�����
    /// @param Chess  ����
    /// @return �Ƿ�����
    bool CanMove(int x1, int y1, int x2, int y2, ChessPieces Chess[]);
    //�ж����ܷ��ƶ�
    /// @param x1,y1 ���ӵĳ�ʼ����
    /// @param x2,y2 ���ӵ�����
    /// @return �Ƿ�����
    bool CanMove_Horse(int x1, int y1, int x2, int y2);
    //�ж����ܷ��ƶ�
    /// @param x1,y1 ���ӵĳ�ʼ����
    /// @param x2,y2 ���ӵ�����
    /// @param Chess  ����
    /// @return �Ƿ�����
    bool CanMove_Elephant(int x1, int y1, int x2, int y2, ChessPieces Chess[]);
    //�ж������ܷ��ƶ�
    /// @param x1,y1 ���ӵĳ�ʼ����
    /// @param x2,y2 ���ӵ�����
    /// @param Chess  ����
    /// @return �Ƿ�����
    bool CanMove_Queue(int x1, int y1, int x2, int y2, ChessPieces Chess[]);
    //�жϳ��ܷ��ƶ�
    /// @param x1,y1 ���ӵĳ�ʼ����
    /// @param x2,y2 ���ӵ�����
    /// @param Chess  ����
    /// @return �Ƿ�����
    bool CanMove_Chariot(int x1, int y1, int x2, int y2, ChessPieces Chess[]);
    //����ƶ���;�Ƿ�������
    /// @param x1,y1 ���ӵĳ�ʼ����
    /// @param x2,y2 ���ӵ�����
    /// @param Chess  ����
    /// @return �Ƿ�������
    bool CheckHavePiece(int x1, int y1, int x2, int y2, ChessPieces Chess[]);
    //�жϹ����ܷ��ƶ�
    /// @param x1,y1 ���ӵĳ�ʼ����
    /// @param x2,y2 ���ӵ�����
    /// @param Chess  ����
    /// @return �Ƿ�����
    bool CanMove_King(int x1, int y1, int x2, int y2, ChessPieces Chess[]);

    //�ж�С���ܷ��ƶ�
    /// @param x1,y1 ���ӵĳ�ʼ����
    /// @param x2,y2 ���ӵ�����
    /// @param Chess  ����
    /// @return �Ƿ�����
    bool CanMove_PawnFirst(int x1, int y1, int x2, int y2, ChessPieces Chess[]);
    //�ж�С���ܷ��ƶ�
    /// @param x1,y1 ���ӵĳ�ʼ����
    /// @param x2,y2 ���ӵ�����
    /// @param Chess  ����
    /// @return �Ƿ�����
    bool CanMove_Pawn(int x1, int y1, int x2, int y2, ChessPieces Chess[]);
};

