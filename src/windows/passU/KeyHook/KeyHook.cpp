#include <Windows.h>

HINSTANCE g_hInstance;		// Instance Handle

HHOOK	g_hKeyboardHook;	// KeyBoard Hook Handle
HHOOK	g_hMouseHook;		// Mouse Hook Handle

// Shared DATA
#pragma data_seg(".shared")
HWND g_hWnd = NULL;		// Main hwnd. We will get this from the App
#pragma data_seg()

char *szMessageString(int ID);

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
									
	KBDLLHOOKSTRUCT *pKey = (KBDLLHOOKSTRUCT *)lParam;

		//MessageBox(g_hWnd, "keyboardHook", "vkCode : ", MB_OK);
	


	if(nCode<0)
		return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);

	
	if(wParam == WM_KEYDOWN){
		
	//	MessageBox(g_hWnd, "hellokey",0,0);
		
		//SendMessage(g_hWnd, (WM_USER + 1001) , wParam, lParam); // keyboard message

	}

	// MessageBox(NULL, "Keyboard", "Hook", MB_OK);  <-- 짜증졸라 남..

//	return 1;		// 다음 훅체인에 메시지 전달 안함--> 자기 스스로도 키보드 메세지를 못 받는다?
	return CallNextHookEx(g_hKeyboardHook, nCode, wParam, lParam);
}

extern "C" __declspec(dllexport)
LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam){
	if(nCode < 0 )
		return CallNextHookEx(g_hMouseHook, nCode, wParam, lParam);

	if(wParam == WM_LBUTTONDOWN){ // 왼쪽 버튼 DOWN
	//	MessageBox(g_hWnd, "LBUTTONDOWN", "WM_LBUTTONDOWN", MB_OK);
	
	} else if (wParam == WM_LBUTTONUP){ // 왼쪽 버튼 UP
	
	//	MessageBox(g_hWnd, "LBUTTONUP", "WM_LBUTTONUP", MB_OK);
	
	} else if (wParam == WM_RBUTTONDOWN){ // 오른쪽 버튼 DOWN
		
	//	MessageBox(g_hWnd, "RBUTTONDOWN", "WM_RBUTTONDOWN", MB_OK);
	
	} else if(wParam == WM_RBUTTONUP){ // 오른쪽 버튼 UP
		
	//	MessageBox(g_hWnd, "RBUTTONUP", "WM_RBUTTONUP", MB_OK);
	
	} else if(wParam == WM_MOUSEWHEEL){ // 휠 움직일 때
		
	//	MessageBox(g_hWnd, "MOUSEWHEEL", "WM_MOUSEWHEEL", MB_OK);
	
	//} else if(wParam == WM_MOUSEMOVE){ // 마우스 움직일 때
	//		
	//	//MessageBox(g_hWnd, "MOUSEMOVE", "WM_MOUSEMOVE", MB_OK);
	//
	} else if(wParam == WM_MBUTTONDOWN){ // 휠 버튼 DOWN
			
		MessageBox(g_hWnd, "MBUTTONDOWN", "WM_MBUTTONDOWN", MB_OK);
	
	} else if(wParam == WM_MBUTTONUP){ // 휠 버튼 UP
			
		MessageBox(g_hWnd, "MBUTTONUP", "WM_MBUTTONUP", MB_OK);
	
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