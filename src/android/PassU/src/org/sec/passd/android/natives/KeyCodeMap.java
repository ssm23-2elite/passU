package org.sec.passd.android.natives;

import java.util.HashMap;

import android.os.Build;
import android.util.Log;
import android.view.KeyEvent;

public class KeyCodeMap {
	public static HashMap<Integer, Integer> M;
	
	public static void init() {
		Log.w("KeyCodeMap", "Init");
		M = new HashMap<Integer, Integer>();
		M.put(WindowsKeyCode.KEY_0, KeyEvent.KEYCODE_0);
		M.put(WindowsKeyCode.KEY_1, KeyEvent.KEYCODE_1);
		M.put(WindowsKeyCode.KEY_2, KeyEvent.KEYCODE_2);
		M.put(WindowsKeyCode.KEY_3, KeyEvent.KEYCODE_3);
		M.put(WindowsKeyCode.KEY_4, KeyEvent.KEYCODE_4);
		M.put(WindowsKeyCode.KEY_5, KeyEvent.KEYCODE_5);
		M.put(WindowsKeyCode.KEY_6, KeyEvent.KEYCODE_6);
		M.put(WindowsKeyCode.KEY_7, KeyEvent.KEYCODE_7);
		M.put(WindowsKeyCode.KEY_8, KeyEvent.KEYCODE_8);
		M.put(WindowsKeyCode.KEY_9, KeyEvent.KEYCODE_9);
		M.put(WindowsKeyCode.VK_MINUS, KeyEvent.KEYCODE_MINUS);
		M.put(WindowsKeyCode.VK_PLUS, KeyEvent.KEYCODE_EQUALS);
		M.put(WindowsKeyCode.KEY_TAB, KeyEvent.KEYCODE_TAB);
		M.put(WindowsKeyCode.KEY_A, KeyEvent.KEYCODE_A);
		M.put(WindowsKeyCode.KEY_B, KeyEvent.KEYCODE_B);
		M.put(WindowsKeyCode.KEY_C, KeyEvent.KEYCODE_C);
		M.put(WindowsKeyCode.KEY_D, KeyEvent.KEYCODE_D);
		M.put(WindowsKeyCode.KEY_E, KeyEvent.KEYCODE_E);
		M.put(WindowsKeyCode.KEY_F, KeyEvent.KEYCODE_F);
		M.put(WindowsKeyCode.KEY_G, KeyEvent.KEYCODE_G);
		M.put(WindowsKeyCode.KEY_H, KeyEvent.KEYCODE_H);
		M.put(WindowsKeyCode.KEY_I, KeyEvent.KEYCODE_I);
		M.put(WindowsKeyCode.KEY_J, KeyEvent.KEYCODE_J);
		M.put(WindowsKeyCode.KEY_K, KeyEvent.KEYCODE_K);
		M.put(WindowsKeyCode.KEY_L, KeyEvent.KEYCODE_L);
		M.put(WindowsKeyCode.KEY_M, KeyEvent.KEYCODE_M);
		M.put(WindowsKeyCode.KEY_N, KeyEvent.KEYCODE_N);
		M.put(WindowsKeyCode.KEY_O, KeyEvent.KEYCODE_O);
		M.put(WindowsKeyCode.KEY_P, KeyEvent.KEYCODE_P);
		M.put(WindowsKeyCode.KEY_Q, KeyEvent.KEYCODE_Q);
		M.put(WindowsKeyCode.KEY_R, KeyEvent.KEYCODE_R);
		M.put(WindowsKeyCode.KEY_S, KeyEvent.KEYCODE_S);
		M.put(WindowsKeyCode.KEY_T, KeyEvent.KEYCODE_T);
		M.put(WindowsKeyCode.KEY_U, KeyEvent.KEYCODE_U);
		M.put(WindowsKeyCode.KEY_V, KeyEvent.KEYCODE_V);
		M.put(WindowsKeyCode.KEY_W, KeyEvent.KEYCODE_W);
		M.put(WindowsKeyCode.KEY_X, KeyEvent.KEYCODE_X);
		M.put(WindowsKeyCode.KEY_Y, KeyEvent.KEYCODE_Y);
		M.put(WindowsKeyCode.KEY_Z, KeyEvent.KEYCODE_Z);
		M.put(WindowsKeyCode.VK_SQUARE_LEFT_BRACKET, KeyEvent.KEYCODE_LEFT_BRACKET);
		M.put(WindowsKeyCode.VK_SQUARE_RIGHT_BRACKET, KeyEvent.KEYCODE_RIGHT_BRACKET);
		M.put(WindowsKeyCode.KEY_BACK, KeyEvent.KEYCODE_DEL);
		if(Build.VERSION.SDK_INT <= Build.VERSION_CODES.GINGERBREAD_MR1)
			M.put(WindowsKeyCode.KEY_F1, KeyEvent.KEYCODE_MOVE_HOME);
		else
			M.put(WindowsKeyCode.KEY_F1, KeyEvent.KEYCODE_HOME);
		M.put(WindowsKeyCode.KEY_F2, KeyEvent.KEYCODE_BACK);
		M.put(WindowsKeyCode.KEY_F3, KeyEvent.KEYCODE_VOLUME_UP);
		M.put(WindowsKeyCode.KEY_F4, KeyEvent.KEYCODE_VOLUME_DOWN);
		M.put(WindowsKeyCode.KEY_F5, KeyEvent.KEYCODE_MENU);
		M.put(WindowsKeyCode.KEY_F6, KeyEvent.KEYCODE_POWER);
		M.put(WindowsKeyCode.VK_OEM_COMMA, KeyEvent.KEYCODE_COMMA);
		M.put(WindowsKeyCode.KEY_LCONTROL, KeyEvent.KEYCODE_CTRL_LEFT);
		M.put(WindowsKeyCode.KEY_RCONTROL, KeyEvent.KEYCODE_CTRL_RIGHT);
		M.put(WindowsKeyCode.KEY_DELETE, KeyEvent.KEYCODE_DEL);
		M.put(WindowsKeyCode.KEY_RETURN, KeyEvent.KEYCODE_ENTER);
		M.put(WindowsKeyCode.KEY_INSERT, KeyEvent.KEYCODE_INSERT);
		M.put(WindowsKeyCode.KEY_SPACE, KeyEvent.KEYCODE_SPACE);
		M.put(WindowsKeyCode.VK_OEM_102, KeyEvent.KEYCODE_BACKSLASH);		
		M.put(WindowsKeyCode.KEY_HANGUEL, KeyEvent.KEYCODE_LANGUAGE_SWITCH);
		M.put(WindowsKeyCode.KEY_KANA, KeyEvent.KEYCODE_LANGUAGE_SWITCH);
		M.put(WindowsKeyCode.KEY_SPACE, KeyEvent.KEYCODE_SPACE);
		M.put(WindowsKeyCode.VK_DOT, KeyEvent.KEYCODE_PERIOD);
		M.put(WindowsKeyCode.VK_SLASH, KeyEvent.KEYCODE_SLASH);
		M.put(WindowsKeyCode.VK_SEMICOLON, KeyEvent.KEYCODE_SEMICOLON);
		M.put(WindowsKeyCode.VK_GRAVE, KeyEvent.KEYCODE_GRAVE);
		M.put(WindowsKeyCode.VK_APOSTROPHE, KeyEvent.KEYCODE_APOSTROPHE);
		M.put(WindowsKeyCode.VK_LEFT_SHIFT, KeyEvent.KEYCODE_SHIFT_LEFT);
	}
}
