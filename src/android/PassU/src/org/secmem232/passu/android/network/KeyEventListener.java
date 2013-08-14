package org.secmem232.passu.android.network;

public interface KeyEventListener {
	/**
	 * Touch pointer has been set.
	 * @param xPosition x coordinate
	 * @param yPosition y coordinate
	 */
	public void onSetCoordinates(int xPosition, int yPosition);
	
	/**
	 * User clicked(touched) screen.
	 */
	public void onTouchDown();
	
	/**
	 * User removed finger from screen.
	 */
	public void onTouchUp();
	
	/**
	 * User pressed down key.
	 * @param keyCode A Keycode that user has pressed
	 */
	public void onKeyDown(int keyCode);
	
	/**
	 * User pressed up key.
	 * @param keyCode A Keycode that user has pressed up
	 */
	public void onKeyUp(int keyCode);
	
	public void onKeyStroke(int keyCode);
}
