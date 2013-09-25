

#include <afxwin.h>
#include <Windows.h>

#define WM_KEYBOARD_FALSE		WM_USER + 1001
#define WM_MOUSE_FALSE			WM_USER + 1002
#define WM_KEYBOARD_TRUE		WM_USER + 1003
#define WM_MOUSE_TRUE			WM_USER + 1004
#define WM_BINDED_CLIENT		WM_USER + 1005
#define WM_UNBINDED_CLIENT		WM_USER + 1006
#define WM_ARRIVED_SCREEN_EDGE	WM_USER + 1007

HINSTANCE g_hInstance;		// Instance Handle
CRITICAL_SECTION cs;

HHOOK	g_hKeyboardHook;	// KeyBoard Hook Handle
HHOOK	g_hMouseHook;		// Mouse Hook Handle

// Shared DATA
#pragma data_seg(".shared")
HWND g_hWnd = NULL;		// Main hwnd. We will get this from the App
HWND g_hMsgWnd = NULL;
#pragma data_seg()

BOOL m_overFlag = FALSE; // 커서가 클라이언트 쪽으로 넘어간 것을 확인하는 변수 /TRUE : 넘어감

BOOL bLMouseDown = FALSE;
BOOL bRMouseDown = FALSE;
BOOL bMMouseDown = FALSE;
BOOL BLOCK = FALSE;

int btn_Bind[9]; // 각각의 index는 버튼 번호, 그 안의 값은 ClientID
int whereisPoint;

char *szMessageString(int ID);

typedef struct tagHEVENT{
	int type;
	int keyCode;
	int updown; // 0 : down, 1 : up
	WPARAM data;
	LPARAM lParam;
}HEVENT;

typedef struct mousepacket{ // 마우스 위치 정보를 가지고 있는 패킷(TCP)
	int msgType; // msgType : 2
	int sendDev; // server : 5, client : clnt_id
	int recvDev; // server : 5, client : clnt_id
	int deviceType; // mouse or keyboard
	int relativeField; // 
	int updownFlag; // 0 : up, 1 : down
	int leftRight; // 0 : left , 1 : right
	int wheelFlag; // 0 : wheel off, 1 : wheel btn down 2 : wheel btn up 3: wheel move
	int xCoord; // x좌표
	int yCoord; // y좌표
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

// Keyboard Hook Procedure
extern "C" __declspec(dllexport)
	LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{				
	HWND hwnd = FindWindow(NULL, TEXT("PassU - Pass Your USB via Network"));
	KBDLLHOOKSTRUCT *pKey = (KBDLLHOOKSTRUCT *)lParam;

	if(nCode<0)
		return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);

	if(wParam == WM_KEYDOWN ){
		COPYDATASTRUCT CDS;
		HEVENT Event;

		CDS.dwData = 0; // keyboard
		CDS.cbData = sizeof(Event);
		CDS.lpData = &Event;

		Event.type = 1; // WM_KEY
		Event.keyCode = pKey->vkCode;
		Event.updown = 0;
		Event.data = wParam;
		Event.lParam = lParam;

		SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
		if(BLOCK){
			return TRUE;
		}

	} else if(wParam == WM_KEYUP){

		COPYDATASTRUCT CDS;
		HEVENT Event;

		CDS.dwData = 0; // keyboard
		CDS.cbData = sizeof(Event);
		CDS.lpData = &Event;

		Event.type = 1; // WM_KEY
		Event.keyCode = pKey->vkCode;
		Event.updown = 1;
		Event.data = wParam;
		Event.lParam = lParam;

		SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
		if(BLOCK){
			return TRUE;
		}
	}
	return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport)
	LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam){
		MPACKET tmp;
		COPYDATASTRUCT CDS;
		POINT pt = {0};
		GetCursorPos(&pt);
		PMSLLHOOKSTRUCT ms;
		ms = (PMSLLHOOKSTRUCT) lParam;

		int nWidth = GetSystemMetrics(SM_CXSCREEN);
		int nHeight = GetSystemMetrics(SM_CYSCREEN);

		HWND hwnd = FindWindow(NULL, TEXT("PassU - Pass Your USB via Network"));

		if(nCode < 0 )
			return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);

		if(wParam == WM_LBUTTONDOWN){ // 왼쪽 버튼 DOWN

			bLMouseDown = TRUE;
			CDS.dwData = 1; // mouse;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			tmp.updownFlag = (bLMouseDown == TRUE)?1:0; // down
			tmp.leftRight = 0; // left
			tmp.wheelFlag = 0;  // 0 : wheel off, 1 : wheel btn down 2 : wheel btn up 3: wheel move
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;

			tmp.deviceType = 0;
			tmp.recvDev = 0;
			tmp.relativeField = 0;
			tmp.sendDev = 0;

			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
			if(BLOCK){
				return TRUE;
			}
		} else if (wParam == WM_LBUTTONUP){ // 왼쪽 버튼 UP
			bLMouseDown = FALSE;
			CDS.dwData = 1;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			tmp.updownFlag = (bLMouseDown == TRUE)?1:0; // up
			tmp.leftRight = 0; // left
			tmp.wheelFlag = 0; // 0 : wheel off, 1 : wheel btn down 2 : wheel btn up 3: wheel move
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;

			tmp.deviceType = 0;
			tmp.recvDev = 0;
			tmp.relativeField = 0;
			tmp.sendDev = 0;

			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
			if(BLOCK){
				return TRUE;
			}
		} else if (wParam == WM_RBUTTONDOWN){ // 오른쪽 버튼 DOWN
			bRMouseDown = TRUE;
			CDS.dwData = 1;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			tmp.updownFlag = (bRMouseDown == TRUE)?1:0; // down
			tmp.leftRight = 1; // right
			tmp.wheelFlag = 0;
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;

			tmp.deviceType = 0;
			tmp.recvDev = 0;
			tmp.relativeField = 0;
			tmp.sendDev = 0;

			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

			if(BLOCK){
				return TRUE;
			}
		} else if(wParam == WM_RBUTTONUP){ // 오른쪽 버튼 UP
			bRMouseDown = FALSE;
			CDS.dwData = 1;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			tmp.updownFlag = (bRMouseDown == TRUE)?1:0; // up
			tmp.leftRight = 1; // right
			tmp.wheelFlag = 0;
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;

			tmp.deviceType = 0;
			tmp.recvDev = 0;
			tmp.relativeField = 0;
			tmp.sendDev = 0;

			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
			if(BLOCK){
				return TRUE;
			}
		} else if(wParam == WM_MOUSEWHEEL){ // 휠 움직일 때
			CDS.dwData = 1;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			tmp.updownFlag = 2; // up
			tmp.leftRight = 2; // right
			int wheel = ms->mouseData >> 31;
			if(wheel == 0){
				tmp.wheelFlag = 3;  // 0 : wheel off, 1 : wheel btn down 2 : wheel btn up 3: wheel move up 4 : wheel move down
			} else if(wheel == 1){
				tmp.wheelFlag = 4;
			}
			
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;

			tmp.deviceType = 0;
			tmp.recvDev = 0;
			tmp.relativeField = 0;
			tmp.sendDev = 0;

			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

			if(BLOCK){
				return TRUE;
			}
		} else if(wParam == WM_MBUTTONDOWN){ // 휠 버튼 DOWN
			bMMouseDown = TRUE;
			CDS.dwData = 1;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			tmp.updownFlag = 2; // up
			tmp.leftRight = 3; // right
			tmp.wheelFlag = 1; // 0 : wheel off, 1 : wheel btn down 2 : wheel btn up 3: wheel move
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;
			tmp.deviceType = 0;
			tmp.recvDev = 0;
			tmp.relativeField = 0;
			tmp.sendDev = 0;

			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

			if(BLOCK){
				return TRUE;
			}
		} else if(wParam == WM_MBUTTONUP){ // 휠 버튼 UP
			bMMouseDown = FALSE;
			CDS.dwData = 1;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			tmp.leftRight = 3; // right
			tmp.wheelFlag = 2;  // 0 : wheel off, 1 : wheel btn down 2 : wheel btn up 3: wheel move
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;

			tmp.updownFlag = 4; // up
			tmp.deviceType = 0;
			tmp.recvDev = 0;
			tmp.relativeField = 0;
			tmp.sendDev = 0;
			//	TRACE("x : %d, y : %d\n", pt.x, pt.y);
			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
			if(BLOCK){
				return TRUE;
			}

		} else if(wParam == WM_MOUSEMOVE){ // 마우스 이동
			//TRACE("MOUSE MOVE EVENT");
			CDS.dwData = 1;
			CDS.cbData = sizeof(tmp);
			CDS.lpData = &tmp;

			tmp.msgType = 2; // mouse
			tmp.xCoord = pt.x;
			tmp.yCoord = pt.y;

			tmp.updownFlag = 2;
			tmp.leftRight = 2;
			tmp.wheelFlag = 0;
			tmp.deviceType = 0;
			tmp.recvDev = 0;
			tmp.relativeField = 0;
			tmp.sendDev = 0;

			int nWidth =  GetSystemMetrics(SM_CXSCREEN);
			int nHeight =  GetSystemMetrics(SM_CYSCREEN);
			if(pt.x <= 2 ) {
				if((whereisPoint == 5) && (btn_Bind[3] != 0)) { // 왼쪽
					::SetCursorPos(nWidth - 3, pt.y);
					BLOCK = TRUE;
					whereisPoint = 4;
					SendMessage(hwnd, WM_ARRIVED_SCREEN_EDGE, TRUE, whereisPoint);
					return TRUE;
				} else if((whereisPoint == 6) && (btn_Bind[5] != 0)) {
					::SetCursorPos(nWidth - 3, pt.y);
					BLOCK = FALSE;
					whereisPoint = 5;
					SendMessage(hwnd, WM_ARRIVED_SCREEN_EDGE, FALSE, whereisPoint);
					return TRUE;
				}
			} else if( pt.y <= 2 ) {
				if((whereisPoint == 5) && (btn_Bind[1] != 0)) { // 위쪽
					::SetCursorPos(pt.x, nHeight - 3);
					BLOCK = TRUE;
					whereisPoint = 2;
					SendMessage(hwnd, WM_ARRIVED_SCREEN_EDGE, TRUE, whereisPoint);
					return TRUE;
				} else if((whereisPoint == 8) && (btn_Bind[7] != 0)) {
					::SetCursorPos(pt.x, nHeight - 3);
					BLOCK = FALSE;
					whereisPoint = 5;
					SendMessage(hwnd, WM_ARRIVED_SCREEN_EDGE, FALSE, whereisPoint);
					return TRUE;
				}
			} else if( pt.x >= nWidth - 2 ) {
				if((whereisPoint == 5) && (btn_Bind[5] != 0)) { // 오른쪽
					::SetCursorPos(3, pt.y);
					BLOCK = TRUE;
					whereisPoint = 6;
					SendMessage(hwnd, WM_ARRIVED_SCREEN_EDGE, TRUE, whereisPoint);
					return TRUE;
				} else if((whereisPoint == 4) && (btn_Bind[3] != 0)) {
					::SetCursorPos(3, pt.y);
					BLOCK = FALSE;
					whereisPoint = 5;
					SendMessage(hwnd, WM_ARRIVED_SCREEN_EDGE, FALSE, whereisPoint);
					return TRUE;
				}
			} else if( pt.y >= nHeight - 2 ) {
				if((whereisPoint == 5) && (btn_Bind[7] != 0)) { // 아래쪽
					::SetCursorPos(pt.x, 3);
					BLOCK = TRUE;
					whereisPoint = 8;
					SendMessage(hwnd, WM_ARRIVED_SCREEN_EDGE, TRUE, whereisPoint);
					return TRUE;
				} else if((whereisPoint == 2) && (btn_Bind[1] != 0)) {
					::SetCursorPos(pt.x, 3);
					BLOCK = FALSE;
					whereisPoint = 5;
					SendMessage(hwnd, WM_ARRIVED_SCREEN_EDGE, FALSE, whereisPoint);
					return TRUE;
				}
			}
			SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

		}
		return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);
}



/* _______________________________________________________________________________ 

Install Method (Hook)
_______________________________________________________________________________  */

// WH_KEYBOARD : 키보드 후킹
extern "C" __declspec(dllexport)
	HHOOK InstallKeyboardHook()
{
	g_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, g_hInstance, 0);

	return g_hKeyboardHook;
}

// WH_MOUSE_LL 마우스 후킹
extern "C" __declspec(dllexport)
	HHOOK InstallMouseHook(){
		g_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, g_hInstance, 0);

	for(int i = 0 ; i < 9 ; i ++){
		btn_Bind[i] = 0;
	}
	whereisPoint = 5;

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
	case WM_BINDED_CLIENT:
		btn_Bind[wParam] = lParam;
		break;
	case WM_UNBINDED_CLIENT:
		btn_Bind[wParam] = 0;
		break;
	}

	return DefWindowProc(hWnd, uiMsg, wParam, lParam);
}
/* _______________________________________________________________________________ 

Dll Main
_______________________________________________________________________________  */

BOOL WINAPI DllMain(HINSTANCE hInst,
					DWORD fdwReason,
					LPVOID lpRes)
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