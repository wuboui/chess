#include <windows.h>
#include<gdiplus.h>
#pragma comment(lib,"gdiplus.lib")
#include <thread>
#include <queue>
#include <mutex>
#include <condition_variable>
const int ErStartError = 0; //1表示为成功
const int ErVersionError = 2;
const int ErConnectError = 3;
const int ErSendError = 4;
const int ErSelectError = 5;
const int SuSendSuccess = 6;

//棋盘用
const int ChessBoard = 64;
//const int iChessWidth = 150;

const int iStartPosX = 100;
const int iStartPosY = 100;
const int iEillpse = 10;

const int iCellWidth = 50;
const int iCellHeigt = 50;
const int iChessHeight = 100;
const int iChessWidth = 150;
const int iChessCount = 8;
const int Horse = 1;
////消息协议
const int MSG_MOVE_PIECE = 20;  //落子
const int MSG_GAMEFINSH = 21; //游戏结束
const int MSG_GAMEBEGIN = 22;//游戏开始

struct C_S_MOVE
{
	int x1;
	int y1;
	int x2;
	int y2;
	bool flag;  //哪一方落子
	C_S_MOVE()
	{
		memset(this, 0, sizeof(C_S_MOVE));
	}
};
struct S_C_MOVE
{
	int x1;
	int y1;
	int x2;
	int y2;
	bool flag;  //哪一方落子
	S_C_MOVE()
	{
		memset(this, 0, sizeof(S_C_MOVE));
	}
};
struct NetHead
{
	int iMsgSize;
	int iHead;
};
struct ChessPieces
{

	Gdiplus::Image* IImage;
	int iWidth;
	int iHeight;
	int iX;
	int iY;
	bool bHover;
	bool bClick;
	bool bDrawBG;
	int  iType;
	bool bFlag;
	ChessPieces(WCHAR * filename, int width, int height, int x, int y)
	{
		IImage = new Gdiplus::Bitmap(filename, true);
		iWidth = width;
		iHeight = height;
		iX = x;
		iY = y;
		bHover = false;
		bClick = false;
		bDrawBG = false;
		iType = 255;
	}
	ChessPieces(int Type, bool Flag, int width, int height, int x, int y)
	{

		bFlag = Flag;
		iWidth = width;
		iHeight = height;
		iX = x;
		iY = y;
		bHover = false;
		bClick = false;
		bDrawBG = false;
		iType = Type;
	}
};