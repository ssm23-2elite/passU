package org.secmem232.passu.android.driver;


/***
 * get status of busybox installation
 * @author Jake Yoon
 *
 */
public interface InstallDriverListener {	
	public void onSucceed();
	public void onError(int ERR_CODE);
}
