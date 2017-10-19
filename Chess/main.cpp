#include "UIchess.h"
#include "Message.h"
#include "ChessLogic.h"
#include "Client.h"
#include "Common.h"
#include <map>
#include <mutex>
#include <condition_variable>
#define IDC_BUTTON                        40004  
#define KEY_DOWM(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000)? 1:0)
using namespace Gdiplus;
int iArrIndex = 0;
int iClickIndex = 255;
HWND hWnd;

TCPSocketClient* SocketTcp;
ChessLogic * Logic;
UIChess* UI;
std::multimap<void*, int> m_SendMap;
CCriticalLock m_Critiacal_Send;

//C++11 锁
std::mutex mSendMutex;
std::condition_variable cSendCondition;
struct  tagWindows
{
	HWND hWnd;
	int i;
	TCPSocketClient* SocketTcp;
	ChessLogic* Logic;
	UIChess* UI;
	std::multimap<void*, int> m_SendMap;
	CCriticalLock m_Critiacal_Send;
};
void ShowCode(wchar_t* Msg)
{
		MessageBox(NULL, Msg, L"Notice", MB_OK);
}
void SendData(void* pData, int flag);
LRESULT CALLBACK textprom(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	PAINTSTRUCT ps;
	HDC hdc;
	int result;
	if (KEY_DOWM(VK_ESCAPE))
		PostMessage(hwnd, WM_DESTROY, 0, 0);
	switch (msg)
	{
	case WM_CREATE:
		return 0;
	case WM_CLOSE:
		result = MessageBox(hwnd, L"你确定离开游戏么?", L"离开游戏", MB_YESNO | MB_ICONQUESTION);
		if (IDYES == result)
		{
			SocketTcp->Stop();
			/*m_Critiacal_Send.release();
			m_SendMap.clear();*/
			return (DefWindowProc(hwnd, msg, wparam, lparam));

		}
		return 0;
	case WM_PAINT:
	{
					 hdc = BeginPaint(hwnd, &ps);
					 WCHAR *temp = new WCHAR[100];
					 memset(temp, '\0', sizeof(temp));
					 HDC hMemDC = CreateCompatibleDC(hdc);
					 RECT rtClient;
					 GetClientRect(hwnd, &rtClient);
					 HBITMAP hMemBitmap = CreateCompatibleBitmap(hdc, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top);
					 HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hMemBitmap);
					 HBRUSH hBgColor_2 = CreateSolidBrush(RGB(255, 255, 255));
					 ::FillRect(hMemDC, &rtClient, hBgColor_2);

					 HPEN hPen = CreatePen(PS_SOLID, 3, RGB(211, 112, 133));

					 HPEN hOldPen = (HPEN)SelectObject(hMemDC, hPen);
					 HBRUSH hBgColor = CreateSolidBrush(RGB(122, 221, 132));

					 Graphics graphics(hMemDC);
					 for (int i = 0; i < ChessBoard; i++)
					 {
						 if (UI->ChessArr[i]->bHover)
							 SelectObject(hMemDC, hPen);
						 else
							 SelectObject(hMemDC, hOldPen);

						 Rectangle(hMemDC, UI->ChessArr[i]->iX, UI->ChessArr[i]->iY, UI->ChessArr[i]->iWidth + UI->ChessArr[i]->iX, UI->ChessArr[i]->iHeight + UI->ChessArr[i]->iY);
						 if (UI->ChessArr[i]->bDrawBG)
						 {
							 SelectObject(hMemDC, hBgColor);
							 Rectangle(hMemDC, UI->ChessArr[i]->iX, UI->ChessArr[i]->iY, UI->ChessArr[i]->iWidth + UI->ChessArr[i]->iX, UI->ChessArr[i]->iHeight + UI->ChessArr[i]->iY);
							 SelectObject(hMemDC, hBgColor_2);
						 }


						 wsprintf(temp, TEXT("png//%s%d"), UI->ChessArr[i]->bFlag ? "black" : "white", UI->ChessArr[i]->iType);
						 graphics.DrawImage(UI->ChessArr[i]->IImage, UI->ChessArr[i]->iX + 50, UI->ChessArr[i]->iY + 30, UI->ChessArr[i]->iWidth - 100, UI->ChessArr[i]->iHeight - 50);
						 if (UI->ChessArr[i]->bClick)
						 {
							 SelectObject(hMemDC, hBgColor_2);
							 SelectObject(hMemDC, hOldPen);
							 Ellipse(hMemDC, UI->ChessArr[i]->iX + (iChessWidth / 2) - iEillpse, UI->ChessArr[i]->iY + (iChessHeight / 2) - iEillpse, UI->ChessArr[i]->iX + (iChessWidth / 2) + iEillpse, UI->ChessArr[i]->iY + (iChessHeight / 2) + iEillpse);
						 }
					 }

					 BitBlt(hdc, 0, 0, rtClient.right - rtClient.left, rtClient.bottom - rtClient.top, hMemDC, 0, 0, SRCCOPY);
					 SelectObject(hMemDC, hOldBitmap);
					 DeleteObject(hMemBitmap);
					 SelectObject(hMemDC, hBgColor_2);
					 DeleteObject(hBgColor);
					 SelectObject(hMemDC, hPen);
					 DeleteObject(hOldPen);
					 DeleteObject(hMemDC);
					 delete temp;
					 temp = NULL;
					 EndPaint(hwnd, &ps);

					 return 0;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_MOUSEMOVE:
	{
						 int  x = LOWORD(lparam); //取得鼠标x坐标值(低位字节的信息)
						 int  y = HIWORD(lparam); //取得鼠标y坐标值(高位字节的信息)
						 int iIndex = 0;
						 if (x < iChessWidth * 8 + iStartPosX && y < iChessHeight * 8 + iStartPosY && x >= iStartPosX&&y >= iStartPosY)
						 {
							 UI->ChessArr[iArrIndex]->bHover = false;
							 iIndex = ((y - iStartPosY) / iChessHeight) * 8 + (x - iStartPosX) / iChessWidth;
							 if (iArrIndex != iIndex)
							 {
								 iArrIndex = iIndex;
							 }

							 UI->ChessArr[iIndex]->bHover = true;
						 }
						 InvalidateRect(hwnd, NULL, false);
						 break;
	}
	case WM_LBUTTONDOWN:
	{
						   int  x = LOWORD(lparam); //取得鼠标x坐标值(低位字节的信息)
						   int  y = HIWORD(lparam); //取得鼠标y坐标值(高位字节的信息)
						   int iIndex = 0;
						//   SocketTcp->SendData();
						   if (x < iChessWidth * 8 + iStartPosX && y < iChessHeight * 8 + iStartPosY && x >= iStartPosX&&y >= iStartPosY)
						   {

							   if (UI->iOldX != 0 && UI->iOldY != 0)
							   {
								   if (Logic->CanMove((x - iStartPosX) / iChessCount, (y - iStartPosY) / iChessCount,
									   (UI->iOldX - iStartPosX) / iChessCount, (UI->iOldY - iStartPosY) / iChessCount, *UI->ChessArr))
								   {
									   S_C_MOVE* MoveData = new S_C_MOVE;
									   MoveData->x1 = (x - iStartPosX) / iChessWidth;
									   MoveData->y1 = (y - iStartPosY) / iChessHeight;
									   MoveData->x2 = (UI->iOldX - iStartPosX) / iChessWidth;
									   MoveData->y2 = (UI->iOldY - iStartPosY) / iChessHeight;
									   SendData(MoveData, MSG_MOVE_PIECE);
									   {
										   UI->iOldX = 0;
										   UI->iOldY = 0;
									   }
								   }
							   }
							   else
							   {
								   UI->iOldX = x;
								   UI->iOldY = y;
							   }
						   }
						   /* InvalidateRect(hwnd, NULL, false);*/

						   break;
	}
	case WM_CTLCOLORSTATIC:
	{
							  HDC hdc = (HDC)wparam;
							  SetBkColor(hdc, RGB(200, 200, 200));
	}
	case WM_COMMAND:
	{
	}
	case WM_SOCKET_MESSAGE:
	{
							  if (SocketTcp->OnSocketNoti(wparam, lparam))
							  {
								  InvalidateRect(hwnd, NULL, false);
							  }
	}
	default:break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);
}
void SendData(void* pData, int flag)
{
	/*m_Critiacal_Send.lock();
	m_SendMap.insert(std::make_pair(pData, flag));
	m_Critiacal_Send.unlock();
	m_Critiacal_Send.unwaite();*/
	std::lock_guard<std::mutex> grand(mSendMutex);
	m_SendMap.insert(std::make_pair(pData, flag));
	cSendCondition.notify_one();

}
unsigned int _stdcall ThreadProc(void* lParam)
{
	SocketTcp = new Client(UI);
	if (!SocketTcp->Start_Socket(hWnd))
	{
		ShowCode(L"ERROR");
	};
	return true;
}
int Send(void* pData, int flag)
{
	switch (flag)
	{
	case MSG_MOVE_PIECE:
		return SocketTcp->SendGameData((S_C_MOVE*)pData, MSG_MOVE_PIECE, sizeof(S_C_MOVE));
	default:
		break;
	}
	return 1;
}
unsigned int _stdcall SendThreadProc(void* lParam)
{
	//m_Critiacal_Send.lock();
	
	while (1)
	{
		std::unique_lock<std::mutex> uni(mSendMutex);
		if (m_SendMap.empty())
		{/*
			m_Critiacal_Send.unlock();
			m_Critiacal_Send.waite();*/
			cSendCondition.wait(uni);
		}
	//	m_Critiacal_Send.lock();
		int flag = m_SendMap.begin()->second;
		void* pData = m_SendMap.begin()->first;
		m_SendMap.erase(m_SendMap.begin());
	//	m_Critiacal_Send.unlock();
	//	m_Critiacal_Send.unwaite();
		 ;
		if (Send(pData, flag))
		{
			//ShowCode(L"Send Success");
		}
		else;
	}
	m_Critiacal_Send.unlock();
}

void game_main(HWND hWnd)
{
	
}

int APIENTRY WinMain(HINSTANCE hCurrentInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{
	ULONG_PTR gdiplusStartupToken = 111;
	GdiplusStartupInput gdiInput;
	GdiplusStartup(&gdiplusStartupToken, &gdiInput, NULL);
	WNDCLASS wndclass;

	MSG msg;
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = textprom;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.hInstance = hCurrentInst;
	wndclass.hIcon = LoadIcon(NULL, MAKEINTRESOURCE(100));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = L"窗体";

	RegisterClass(&wndclass);

	hWnd = CreateWindow(L"窗体", L"111", WS_DLGFRAME | WS_MINIMIZEBOX | WS_SYSMENU,
		iStartPosX, iStartPosY,
		1600, 2600,
		NULL, NULL,
		hCurrentInst, NULL);
	if (hWnd == NULL) return 0;
    UI = new UIChess();
    Logic = new ChessLogic();
	m_SendMap.clear();
	m_Critiacal_Send.init();
	UI->initChess();
	//SetWindowLong(hWnd, GWL_USERDATA, (long)win);
	//ShowWindow(hWnd, iCmdShow);
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);
	HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, ThreadProc, NULL, NULL, NULL);
	HANDLE handle1 = (HANDLE)_beginthreadex(NULL, 0, SendThreadProc, NULL, NULL, NULL);
	while (GetMessageW(&msg, NULL, 0, 0))
	{
		game_main(hWnd);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	GdiplusShutdown(gdiplusStartupToken);
	return 0;

}






