#include "Server.h"
#include <condition_variable>
#define  IDC_LIST_BOX 1022
//#include<windows.h>
wchar_t* wcString = NULL;
HWND hListBox;
HWND hWnd;
IOCPClass* IOCPModel = NULL;
std::queue<void*> qRecvTask;
std::mutex m_Recv;
std::condition_variable m_cRecv;
void HandleRecv(IOCPClass iocp);
LRESULT CALLBACK textprom(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_CREATE:

		return 0;
	case WM_PAINT:
			UpdateWindow(hwnd);
		return 0;
	case WM_CLOSE:
		if (NULL != IOCPModel)
			IOCPModel->Stop();
		return (DefWindowProc(hwnd, msg, wparam, lparam));
	case WM_MOUSEMOVE:
	//	InvalidateRect(hwnd, NULL, false);
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProc(hwnd, msg, wparam, lparam);;
}
void SetMsg(char* msg,int iSize)
{
	swprintf_s(wcString, 200, (wchar_t*)msg, iSize);
	SendMessageA(GetDlgItem(hWnd, IDC_LIST_BOX), LB_ADDSTRING, NULL, (LPARAM)wcString);
}
int APIENTRY WinMain(HINSTANCE hCurrentInst, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int iCmdShow)
{
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
	wndclass.lpszClassName = L"´°Ìו";

	RegisterClass(&wndclass);

	 hWnd = CreateWindow(L"´°Ìו", L"111", WS_DLGFRAME | WS_MINIMIZEBOX | WS_SYSMENU,
		50, 50,
		500, 500,
		NULL, NULL,
		hCurrentInst, NULL);
	ShowWindow(hWnd, SW_SHOWNORMAL);
	UpdateWindow(hWnd);
	hListBox = CreateWindow(L"LISTBOX",
		NULL,
		WS_CHILD | WS_VSCROLL| WS_BORDER  ,
		100, 20, 300, 300,
		hWnd, (HMENU)(IDC_LIST_BOX),
		(HINSTANCE)GetWindowLong(hWnd, GWL_HINSTANCE),
		NULL);

	ShowWindow(hListBox, SW_SHOWNORMAL);
    IOCPModel = new IOCPClass;
	IOCPModel->SetHwnd(hWnd);
	IOCPModel->SetRecvTask(qRecvTask, m_Recv, m_cRecv);
    IOCPModel->Start();
    while (GetMessage(&msg, NULL, 0, 0))
	{
		
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 0;

}
