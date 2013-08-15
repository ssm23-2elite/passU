package org.secmem232.passu.android.mouse;

import org.secmem232.passu.android.R;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.view.MotionEvent;
import android.view.ViewGroup;

public class PassUMouse extends ViewGroup {
		private Paint mLoadPaint;
		boolean mShowCursor;

		Bitmap	cursor;
		public int x = 0,y = 0;

		public void Update(int nx, int ny) {
			x = nx; y = ny;
		}
		
		public void ShowCursor() {
			mShowCursor = true;
		}
		
		public void HideCursor() {
			mShowCursor = false;
		}
		public boolean isCursorShown() {
			return mShowCursor;
		}

		public PassUMouse(Context context) {
			super(context);
			cursor = BitmapFactory.decodeResource(context.getResources(), R.drawable.cursor);

			mLoadPaint = new Paint();
			mLoadPaint.setAntiAlias(true);
			mLoadPaint.setTextSize(10);
			mLoadPaint.setARGB(255, 255, 0, 0);
		}

		@Override
		protected void onDraw(Canvas canvas) {
			super.onDraw(canvas);
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
