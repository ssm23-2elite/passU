
#include <afxwin.h>
#include <Windows.h>

#define WM_KEYBOARD_FALSE	WM_USER + 1001
#define WM_MOUSE_FALSE	WM_USER + 1002
#define WM_CLIENT_TRUE	WM_USER + 1003
#define WM_SERVER_TRUE	WM_USER + 1004

HINSTANCE g_hInstance;		// Instance Handle
CRITICAL_SECTION cs;

HHOOK	g_hMouseHook;		// Mouse Hook Handle

// Shared DATA
#pragma data_seg(".shared")
HWND g_hWnd = NULL;		// Main hwnd. We will get this from the App
HWND g_hMsgWnd = NULL;
#pragma data_seg()

BOOL m_overFlag = FALSE; // 커서가 클라이언트 쪽으로 넘어간 것을 확인하는 변수 /TRUE : 넘어감

BOOL m_keyboard = TRUE;

char *szMessageString(int ID);

typedef struct mousepacket{ // 마우스 위치 정보를 가지고 있는 패킷(TCP)
	int msgType:32; // msgType : 2
	int sendDev:32; // server : 5, client : clnt_id
	int recvDev:32; // server : 5, client : clnt_id
	int deviceType:8; // mouse or keyboard
	int relativeField:8; // 상대적인 필드................
	int updownFlag:8; // 0 : up, 1 : down
	int leftRight:8; // 0 : left , 1 : right
	int wheelFlag:32; // 0 : wheel off, 1 : wheel btn down 2 : wheel btn up 3: wheel move
	int xCoord:32; // x좌표
	int yCoord:32; // y좌표
	int pad:32;
	//mouse move 일때의 상태를 만들어야함?.......
} MPACKET;
/* _______________________________________________________________________________ 

Get Window Handle
_______________________________________________________________________________  */

extern "C" __declspec(dllexport)
	void SetWindowHandleToDll(HWND hWnd)
{
	g_hWnd = hWnd;
}

/* _______________________________________________________________________________ 

Hook Procedure
_______________________________________________________________________________  */


extern "C" __declspec(dllexport)
	LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam){
		MPACKET tmp;
		COPYDATASTRUCT CDS;
		POINT pt = {0};
		GetCursorPos(&pt);
		int nWidth = GetSystemMetrics(SM_CXSCREEN);
		int nHeight = GetSystemMetrics(SM_CYSCREEN);

		HWND hwnd = FindWindow(NULL, TEXT("PassU - Pass Your USB via Network"));

		if(nCode < 0 )
			return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
		
		if(wParam == WM_LBUTTONDOWN){ // 왼쪽 버튼 DOWN


			CDS.dwData = 1; // mouse;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			tmp.updownFlag = 1; // down
			tmp.leftRight = 0; // left
			tmp.wheelFlag = 0;  // 0 : wheel off, 1 : wheel btn down 2 : wheel btn up 3: wheel move
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;

			//TRACE("x : %d, y : %d\n", pt.x, pt.y);

			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

			//TRACE("LBUTTONDOWN\n");


		} else if (wParam == WM_LBUTTONUP){ // 왼쪽 버튼 UP
			CDS.dwData = 1;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			tmp.updownFlag = 0; // up
			tmp.leftRight = 0; // left
			tmp.wheelFlag = 0; // 0 : wheel off, 1 : wheel btn down 2 : wheel btn up 3: wheel move
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;

			//TRACE("x : %d, y : %d\n", pt.x, pt.y);
			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
			//TRACE("LBUTTONUP\n");

			//	MessageBox(g_hWnd, "LBUTTONUP", "WM_LBUTTONUP", MB_OK);

		} else if (wParam == WM_RBUTTONDOWN){ // 오른쪽 버튼 DOWN
			CDS.dwData = 1;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			tmp.updownFlag = 1; // down
			tmp.leftRight = 1; // right
			tmp.wheelFlag = 0;
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;

			//TRACE("x : %d, y : %d\n", pt.x, pt.y);
			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
			//	TRACE("RBUTTONDOWN\n");

			//	MessageBox(g_hWnd, "RBUTTONDOWN", "WM_RBUTTONDOWN", MB_OK);

		} else if(wParam == WM_RBUTTONUP){ // 오른쪽 버튼 UP
			CDS.dwData = 1;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			tmp.updownFlag = 0; // up
			tmp.leftRight = 1; // right
			tmp.wheelFlag = 0;
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;
			//	TRACE("RBUTTONUP\n");

			//TRACE("x : %d, y : %d\n", pt.x, pt.y);
			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

			//	MessageBox(g_hWnd, "RBUTTONUP", "WM_RBUTTONUP", MB_OK);

		} else if(wParam == WM_MOUSEWHEEL){ // 휠 움직일 때
			CDS.dwData = 1;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			//tmp.updownFlag = 0; // up
			//tmp.leftRight = 1; // right
			tmp.wheelFlag = 3;  // 0 : wheel off, 1 : wheel btn down 2 : wheel btn up 3: wheel move
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;

			//	TRACE("MOUSEWHEEL\n");

		//	TRACE("x : %d, y : %d\n", pt.x, pt.y); 
			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

		} else if(wParam == WM_MBUTTONDOWN){ // 휠 버튼 DOWN
			CDS.dwData = 1;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			//tmp.updownFlag = 0; // up
			//tmp.leftRight = 1; // right
			tmp.wheelFlag = 1; // 0 : wheel off, 1 : wheel btn down 2 : wheel btn up 3: wheel move
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;


		//	TRACE("x : %d, y : %d\n", pt.x, pt.y);
			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

			//	TRACE("WHEELBUTTONDOWN\n");

		} else if(wParam == WM_MBUTTONUP){ // 휠 버튼 UP
			CDS.dwData = 1;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			//tmp.updownFlag = 0; // up
			//tmp.leftRight = 1; // right
			tmp.wheelFlag = 2;  // 0 : wheel off, 1 : wheel btn down 2 : wheel btn up 3: wheel move
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;

		//	TRACE("x : %d, y : %d\n", pt.x, pt.y);
		SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
			//	TRACE("WHEELBUTTONUP\n");
		} else if(wParam == WM_MOUSEMOVE){ // 마우스 이동

			//	TRACE("%d, %d\n", nWidth, nHeight);
			//	TRACE("%d, %d\n", pt.x, pt.y);
			//if(m_overFlag == TRUE){

			//	}
			CDS.dwData = 1;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;

			//TRACE("x : %d, y : %d\n", pt.x, pt.y);
			//if(m_overFlag)

		//	TRACE("x : %d, y : %d\n", pt.x, pt.y);
			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

		}
		//	} else
		//		return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);

		return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}

/* _______________________________________________________________________________ 

Install Method (Hook)
_______________________________________________________________________________  */

// WH_MOUSE_LL 마우스 후킹
extern "C" __declspec(dllexport)
	HHOOK InstallMouseHook(){
		g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, g_hInstance, 0);


		return g_hMouseHook;
}

/* _______________________________________________________________________________ 

Uninstall Method (Hook)
_______________________________________________________________________________  */

// Keyboard의 훅 프로시져를 해제한다.
extern "C" __declspec(dllexport)
	void UnInstallKeyboardHook()
{
	UnhookWindowsHookEx(g_hKeyboardHook);
}

// Mouse 후킹 제거
extern "C" __declspec(dllexport)
	void UnInstallMouseHook()
{
	UnhookWindowsHookEx(g_hMouseHook);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam);

BOOL CreateMsgWnd(HINSTANCE hInst, HWND *phWnd){
	WNDCLASS stWndClass;

	stWndClass.cbClsExtra = 0;
	stWndClass.cbWndExtra = 0;
	stWndClass.cbClsExtra = 0;
	stWndClass.cbWndExtra = 0;
	stWndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	stWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	stWndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	stWndClass.hInstance = hInst;
	stWndClass.lpfnWndProc = (WNDPROC)WndProc;
	stWndClass.lpszClassName = "MsgWnd";
	stWndClass.lpszMenuName = NULL;
	stWndClass.style = CS_HREDRAW | CS_VREDRAW;
	RegisterClass(&stWndClass);

	*phWnd = CreateWindow("MsgWnd", NULL, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
		NULL, (HMENU)NULL, hInst, NULL);
	if(*phWnd == NULL){
		return FALSE;
	}

	return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uiMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uiMsg){
	case WM_KEYBOARD_FALSE: // 키보드 무시 이벤트 
		// 여기서 메세지 처리

		m_keyboard = FALSE;

		break;
	case WM_MOUSE_FALSE:
		// 여기서 메세지 처리

		m_mouse = FALSE;

		break;
	case WM_CLIENT_TRUE:
		// 여기서 메세지 처리

		m_keyboard = TRUE;
		break;
	case WM_SERVER_TRUE:

		m_mouse = TRUE;
		break;
	}

	return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}
/* _______________________________________________________________________________ 

Dll Main
_______________________________________________________________________________  */

BOOL WINAPI DllMain(HINSTANCE hInst,DWORD fdwReason,LPVOID lpRes)
{
	switch(fdwReason){
	case DLL_PROCESS_ATTACH:
		CreateMsgWnd(hInst, &g_hMsgWnd);
		break;
	case DLL_PROCESS_DETACH:
		if(g_hMsgWnd != NULL){
			DestroyWindow(g_hMsgWnd);
			g_hMsgWnd = NULL;
		}
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;

	}
	return TRUE;
}