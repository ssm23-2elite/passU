
#include <afxwin.h>
#include <Windows.h>
HINSTANCE g_hInstance;		// Instance Handle

HHOOK	g_hKeyboardHook;	// KeyBoard Hook Handle
HHOOK	g_hMouseHook;		// Mouse Hook Handle

// Shared DATA
#pragma data_seg(".shared")
HWND g_hWnd = NULL;		// Main hwnd. We will get this from the App
#pragma data_seg()

char *szMessageString(int ID);

typedef struct tagHEVENT{
	int type;
	int keyCode;
	WPARAM data;
	LPARAM lParam;
}HEVENT;

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

// Keyboard Hook Procedure
extern "C" __declspec(dllexport)
LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	//TCHAR s[256];
						

	HWND hwnd = FindWindow(NULL, TEXT("PassU - Server"));
	KBDLLHOOKSTRUCT *pKey = (KBDLLHOOKSTRUCT *)lParam;

		//MessageBox(g_hWnd, "keyboardHook", "vkCode : ", MB_OK);

	
	if(nCode<0)
		return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);

	if(wParam == WM_KEYDOWN){
		COPYDATASTRUCT CDS;
		HEVENT Event;

		CDS.dwData = 0; // keyboard
		CDS.cbData = sizeof(Event);
		CDS.lpData = &Event;
		
		Event.type = 1; // WM_KEY
		Event.keyCode = pKey->vkCode;
		Event.data = wParam;
		Event.lParam = lParam;
		

		SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

		TRACE("key Code : %d\n", pKey->vkCode);
	//	TRACE("SENDMESSAGE...과연 copyData에서 받을까?\n");

	}
	// MessageBox(NULL, "Keyboard", "Hook", MB_OK);  <-- 짜증졸라 남..

//	return 1;		// 다음 훅체인에 메시지 전달 안함--> 자기 스스로도 키보드 메세지를 못 받는다?
	return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport)
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam){
	MPACKET tmp;
	COPYDATASTRUCT CDS;
	POINT pt = {0};
	GetCursorPos(&pt);
	

	HWND hwnd = FindWindow(NULL, TEXT("PassU - Server"));

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

		TRACE("x : %d, y : %d\n", pt.x, pt.y);

		int a = ::SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);

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
		
		TRACE("x : %d, y : %d\n", pt.x, pt.y);
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
		
		TRACE("x : %d, y : %d\n", pt.x, pt.y);
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
		
		TRACE("x : %d, y : %d\n", pt.x, pt.y);
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
	
		TRACE("x : %d, y : %d\n", pt.x, pt.y);
		SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
	//	MessageBox(g_hWnd, "MOUSEWHEEL", "WM_MOUSEWHEEL", MB_OK);
	
	//} else if(wParam == WM_MOUSEMOVE){ // 마우스 움직일 때
	//		
	//	//MessageBox(g_hWnd, "MOUSEMOVE", "WM_MOUSEMOVE", MB_OK);
	//
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

		
		TRACE("x : %d, y : %d\n", pt.x, pt.y);
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
		
		TRACE("x : %d, y : %d\n", pt.x, pt.y);
		SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(VOID *)&CDS);
	//	TRACE("WHEELBUTTONUP\n");
	} else if(wParam == WM_MOUSEMOVE){ // 마우스 이동
		CDS.dwData = 1;
		CDS.cbData = sizeof(tmp);
		CDS.lpData = &tmp;

		tmp.msgType = 2; // mouse
		tmp.xCoord = pt.x;
		tmp.yCoord = pt.y;
		
		//TRACE("x : %d, y : %d\n", pt.x, pt.y);
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

/* _______________________________________________________________________________ 

		Dll Main
_______________________________________________________________________________  */

BOOL WINAPI DllMain(HINSTANCE hInst,DWORD fdwReason,LPVOID lpRes)
{
	g_hInstance = hInst;
	return TRUE;
}