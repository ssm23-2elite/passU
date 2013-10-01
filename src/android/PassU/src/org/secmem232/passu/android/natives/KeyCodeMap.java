package org.secmem232.passu.android.natives;

import java.util.HashMap;

import android.os.Build;
import android.util.Log;

public class KeyCodeMap {
	public static HashMap<Integer, Integer> M = new HashMap<Integer, Integer>();
	
	public static void init() {
		Log.w("KeyCodeMap", "Init");
		M.put(WindowsKeyCode.KEY_0, NativeKeyCode.KEY_0);
		M.put(WindowsKeyCode.KEY_1, NativeKeyCode.KEY_1);
		M.put(WindowsKeyCode.KEY_2, NativeKeyCode.KEY_2);
		M.put(WindowsKeyCode.KEY_3, NativeKeyCode.KEY_3);
		M.put(WindowsKeyCode.KEY_4, NativeKeyCode.KEY_4);
		M.put(WindowsKeyCode.KEY_5, NativeKeyCode.KEY_5);
		M.put(WindowsKeyCode.KEY_6, NativeKeyCode.KEY_6);
		M.put(WindowsKeyCode.KEY_7, NativeKeyCode.KEY_7);
		M.put(WindowsKeyCode.KEY_8, NativeKeyCode.KEY_8);
		M.put(WindowsKeyCode.KEY_9, NativeKeyCode.KEY_9);
		M.put(WindowsKeyCode.VK_MINUS, NativeKeyCode.KEY_MINUS);
		M.put(WindowsKeyCode.VK_PLUS, NativeKeyCode.KEY_EQUAL);
		M.put(WindowsKeyCode.KEY_TAB, NativeKeyCode.KEY_TAB);
		M.put(WindowsKeyCode.KEY_A, NativeKeyCode.KEY_A);
		M.put(WindowsKeyCode.KEY_B, NativeKeyCode.KEY_B);
		M.put(WindowsKeyCode.KEY_C, NativeKeyCode.KEY_C);
		M.put(WindowsKeyCode.KEY_D, NativeKeyCode.KEY_D);
		M.put(WindowsKeyCode.KEY_E, NativeKeyCode.KEY_E);
		M.put(WindowsKeyCode.KEY_F, NativeKeyCode.KEY_F);
		M.put(WindowsKeyCode.KEY_G, NativeKeyCode.KEY_G);
		M.put(WindowsKeyCode.KEY_H, NativeKeyCode.KEY_H);
		M.put(WindowsKeyCode.KEY_I, NativeKeyCode.KEY_I);
		M.put(WindowsKeyCode.KEY_J, NativeKeyCode.KEY_J);
		M.put(WindowsKeyCode.KEY_K, NativeKeyCode.KEY_K);
		M.put(WindowsKeyCode.KEY_L, NativeKeyCode.KEY_L);
		M.put(WindowsKeyCode.KEY_M, NativeKeyCode.KEY_M);
		M.put(WindowsKeyCode.KEY_N, NativeKeyCode.KEY_N);
		M.put(WindowsKeyCode.KEY_O, NativeKeyCode.KEY_O);
		M.put(WindowsKeyCode.KEY_P, NativeKeyCode.KEY_P);
		M.put(WindowsKeyCode.KEY_Q, NativeKeyCode.KEY_Q);
		M.put(WindowsKeyCode.KEY_R, NativeKeyCode.KEY_R);
		M.put(WindowsKeyCode.KEY_S, NativeKeyCode.KEY_S);
		M.put(WindowsKeyCode.KEY_T, NativeKeyCode.KEY_T);
		M.put(WindowsKeyCode.KEY_U, NativeKeyCode.KEY_U);
		M.put(WindowsKeyCode.KEY_V, NativeKeyCode.KEY_V);
		M.put(WindowsKeyCode.KEY_W, NativeKeyCode.KEY_W);
		M.put(WindowsKeyCode.KEY_X, NativeKeyCode.KEY_X);
		M.put(WindowsKeyCode.KEY_Y, NativeKeyCode.KEY_Y);
		M.put(WindowsKeyCode.KEY_Z, NativeKeyCode.KEY_Z);
		M.put(WindowsKeyCode.VK_SQUARE_LEFT_BRACKET, NativeKeyCode.KEY_LEFTBRACE);
		M.put(WindowsKeyCode.VK_SQUARE_RIGHT_BRACKET, NativeKeyCode.KEY_RIGHTBRACE);
		M.put(WindowsKeyCode.KEY_BACK, NativeKeyCode.KEY_DEL);
		if(Build.VERSION.SDK_INT <= Build.VERSION_CODES.GINGERBREAD_MR1)
			M.put(WindowsKeyCode.KEY_F1, NativeKeyCode.KEY_MOVE_HOME);
		else
			M.put(WindowsKeyCode.KEY_F1, NativeKeyCode.KEY_HOME);
		M.put(WindowsKeyCode.KEY_F2, NativeKeyCode.KEY_BACK);
		M.put(WindowsKeyCode.KEY_F3, NativeKeyCode.KEY_VOLUMEUP);
		M.put(WindowsKeyCode.KEY_F4, NativeKeyCode.KEY_VOLUMEDOWN);
		M.put(WindowsKeyCode.KEY_F5, NativeKeyCode.KEY_MENU);
		M.put(WindowsKeyCode.KEY_F6, NativeKeyCode.KEY_POWER);
		M.put(WindowsKeyCode.VK_OEM_COMMA, NativeKeyCode.KEY_COMMA);
		M.put(WindowsKeyCode.KEY_LCONTROL, NativeKeyCode.KEY_LCTRL);
		M.put(WindowsKeyCode.KEY_RCONTROL, NativeKeyCode.KEY_RCTRL);
		M.put(WindowsKeyCode.KEY_DELETE, NativeKeyCode.KEY_DEL);
		M.put(WindowsKeyCode.KEY_RETURN, NativeKeyCode.KEY_ENTER);
		M.put(WindowsKeyCode.KEY_INSERT, NativeKeyCode.KEY_INSERT);
		M.put(WindowsKeyCode.KEY_SPACE, NativeKeyCode.KEY_SPACE);
		M.put(WindowsKeyCode.VK_OEM_102, NativeKeyCode.KEY_BACKSLASH);		
		M.put(WindowsKeyCode.KEY_HANGUEL, NativeKeyCode.KEY_HANGEUL);
		M.put(WindowsKeyCode.KEY_KANA, NativeKeyCode.KEY_HANGEUL);
		M.put(WindowsKeyCode.KEY_SPACE, NativeKeyCode.KEY_SPACE);
		M.put(WindowsKeyCode.VK_DOT, NativeKeyCode.KEY_DOT);
		M.put(WindowsKeyCode.VK_SLASH, NativeKeyCode.KEY_SLASH);
		M.put(WindowsKeyCode.VK_DOT, NativeKeyCode.KEY_DOT);
		M.put(WindowsKeyCode.VK_SEMICOLON, NativeKeyCode.KEY_SEMICOLON);
		M.put(WindowsKeyCode.VK_GRAVE, NativeKeyCode.KEY_GRAVE);
		M.put(WindowsKeyCode.VK_APOSTROPHE, NativeKeyCode.KEY_APOSTROPHE);
		M.put(WindowsKeyCode.VK_LEFT_SHIFT, NativeKeyCode.KEY_LEFTSHIFT);
	}
}
