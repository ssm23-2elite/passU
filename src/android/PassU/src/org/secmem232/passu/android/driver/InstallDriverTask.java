package org.secmem232.passu.android.driver;

import java.io.IOException;

import org.secmem232.passu.android.util.CommandLine;

import android.content.Context;
import android.os.AsyncTask;


/***
 * 
 * @author Jake Yoon
 *
 */
public class InstallDriverTask extends AsyncTask<Void, Void, Integer>{
	private Context context;
	private InstallDriverListener litener;
	public final static int ERR_IO = -1;
	public final static int ERR_SECURITY = -2;
	public final static int ERR_UNKNOWN = -3;
	
	public InstallDriverTask(Context context, InstallDriverListener litener) {
		this.context = context;
		this.litener = litener;
	}
	
	@Override
	protected void onPreExecute() {
		super.onPreExecute();
	}

	@Override
	protected Integer doInBackground(Void... params) {
		try {
			CommandLine.copyInputDrivers(context);
		} catch (SecurityException e) {
			e.printStackTrace();
			return ERR_SECURITY;
			
		} catch (IOException e) {
			e.printStackTrace();
			return ERR_IO;
		}
		return 0;
	}
	
	@Override
	protected void onPostExecute(Integer result) {
		super.onPostExecute(result);
		switch(result){
		case ERR_SECURITY:
			// Root access denied
			litener.onError(ERR_SECURITY);
			break;
		case ERR_IO:
			litener.onError(ERR_IO);
			break;
		default: // Driver installation succeed
			litener.onSucceed();
			break;
		}	
	}
}
