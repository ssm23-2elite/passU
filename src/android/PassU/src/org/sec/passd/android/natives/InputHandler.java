package org.sec.passd.android.natives;

import android.app.Instrumentation;
import android.content.Context;
import android.os.SystemClock;
import android.view.KeyEvent;
import android.view.MotionEvent;

public class InputHandler {
	private final String LOG = "InputHandler";
	private Instrumentation inst;
	private float x, y;
	
	public InputHandler(Context context){
		inst = new Instrumentation();
	}
	
	public void keyDown(int keyCode) {
		inst.sendKeySync(new KeyEvent(KeyEvent.ACTION_DOWN, keyCode));
	}

	public void keyUp(int keyCode) {
		inst.sendKeySync(new KeyEvent(KeyEvent.ACTION_UP, keyCode));
	}
	
	public void keyStroke(int keyCode) {
		inst.sendKeySync(new KeyEvent(KeyEvent.ACTION_MULTIPLE, keyCode));
	}
	
	public synchronized void touchDown() {
		long eventTime, downTime;
		eventTime = downTime = SystemClock.uptimeMillis();
		inst.sendPointerSync(MotionEvent.obtain(downTime, eventTime, MotionEvent.ACTION_DOWN, x, y, 0));
	}
	
	public synchronized void touchUp() {
		long eventTime, downTime;
		eventTime = downTime = SystemClock.uptimeMillis();
		inst.sendPointerSync(MotionEvent.obtain(downTime, eventTime, MotionEvent.ACTION_UP, x, y, 0));
	}
	
	public synchronized void touchSetPtr(int x, int y) {
		long eventTime, downTime;
		eventTime = downTime = SystemClock.uptimeMillis();
		this.x = x;
		this.y = y;
		inst.sendPointerSync(MotionEvent.obtain(downTime, eventTime, MotionEvent.ACTION_MOVE, x, y, 0));
	}
	
	public void touchOnce(int x, int y){
		touchSetPtr(x, y);
		touchDown();
		touchUp();
	}
}
