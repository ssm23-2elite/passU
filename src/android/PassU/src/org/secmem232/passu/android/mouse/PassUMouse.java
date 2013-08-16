package org.secmem232.passu.android.mouse;

import org.secmem232.passu.android.D;
import org.secmem232.passu.android.R;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.util.Log;
import android.view.MotionEvent;
import android.view.ViewGroup;

public class PassUMouse extends ViewGroup {
	private final String LOG = "PassUMouse";
	private Paint mLoadPaint;
	boolean mShowCursor;

	Bitmap	cursor;
	public int x = 0,y = 0;

	public void Update(int nx, int ny) {
		if(D.D) Log.w(LOG, "Update");
		x = nx; y = ny;
	}

	public void ShowCursor() {
		if(D.D) Log.w(LOG, "ShowCursor");
		mShowCursor = true;
	}

	public void HideCursor() {
		if(D.D) Log.w(LOG, "HideCursor");
		mShowCursor = false;
	}
	public boolean isCursorShown() {
		if(D.D) Log.w(LOG, "isCursorShown");
		return mShowCursor;
	}

	public PassUMouse(Context context) {
		super(context);
		if(D.D) Log.w(LOG, "PassUMouse");
		cursor = BitmapFactory.decodeResource(context.getResources(), R.drawable.cursor);

		mLoadPaint = new Paint();
		mLoadPaint.setAntiAlias(true);
		mLoadPaint.setTextSize(10);
		mLoadPaint.setARGB(255, 255, 0, 0);
	}

	@Override
	protected void onDraw(Canvas canvas) {
		super.onDraw(canvas);
		if(D.D) Log.w(LOG, "onDraw");
		if (mShowCursor) 
			canvas.drawBitmap(cursor, x, y, null);
	}

	@Override
	protected void onLayout(boolean arg0, int arg1, int arg2, int arg3, int arg4) {
	}

	@Override
	public boolean onTouchEvent(MotionEvent event) {
		return true;
	}
}
