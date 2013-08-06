package org.ssm232elite.passu.android;

import org.ssm232elite.passu.android.network.ServerConnectionListener;

import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.PixelFormat;
import android.os.IBinder;
import android.util.Log;
import android.view.Gravity;
import android.view.MotionEvent;
import android.view.ViewGroup;
import android.view.WindowManager;

public class CursorService extends Service implements ServerConnectionListener {
    private OverlayView mView;
    
    public void Update(final int x, final int y, final boolean autoenable) {
    	mView.Update(x,y);
    	if ((x!=0 || y!= 0) && autoenable && !mView.isCursorShown() ) 
    		ShowCursor(true); //will also post invalidate
    	else
    		mView.postInvalidate();
    }
    
    public void ShowCursor(boolean status) {
    	mView.ShowCursor(status);
    	mView.postInvalidate();
    }
    
    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }
    
    public int getX() {
    	return mView.x;
    }

    public int getY() {
    	return mView.y;
    }
    
    @Override
    public void onCreate() {
        super.onCreate();
        Singleton.getInstance().m_CurService = this;

		Log.d("CursorService", "Service created");
		
        mView = new OverlayView(this);
        WindowManager.LayoutParams params = new WindowManager.LayoutParams(
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.WRAP_CONTENT,
                WindowManager.LayoutParams.TYPE_SYSTEM_OVERLAY,//TYPE_SYSTEM_ALERT,//TYPE_SYSTEM_OVERLAY,
                WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE | WindowManager.LayoutParams.FLAG_NOT_TOUCHABLE |
                WindowManager.LayoutParams.FLAG_LAYOUT_IN_SCREEN, //will cover status bar as well!!!
                PixelFormat.TRANSLUCENT);
        params.gravity = Gravity.LEFT | Gravity.TOP;
       //params.x = 100;
        //params.y = 100;
        params.setTitle("Cursor");
        WindowManager wm = (WindowManager) getSystemService(WINDOW_SERVICE);
        wm.addView(mView, params);
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Singleton.getInstance().m_CurService = null;
        if(mView != null) {
            ((WindowManager) getSystemService(WINDOW_SERVICE)).removeView(mView);
            mView = null;
        }
    }

	@Override
	public void onServerConnected(String ipAddress) {
		
	}
	
	@Override
	public void onServerConnectionFailed() {
		
	}
	
	@Override
	public void onServerConnectionInterrupted() {
		
	}
	
	@Override
	public void onServerDisconnected() {
		
	}
}

class OverlayView extends ViewGroup {
    private Paint mLoadPaint;
    boolean mShowCursor;
    
    Bitmap	cursor;
    public int x = 0,y = 0;
    
    public void Update(int nx, int ny) {
    	x += nx; y += ny;
    }
    public void ShowCursor(boolean status) {
    	mShowCursor = status;
	}
    public boolean isCursorShown() {
    	return mShowCursor;
    }
    
	public OverlayView(Context context) {
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
        	canvas.drawBitmap(cursor,x,y,null);
    }

    @Override
    protected void onLayout(boolean arg0, int arg1, int arg2, int arg3, int arg4) {
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        return true;
    }
}
