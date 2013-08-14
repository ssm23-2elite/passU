package org.secmem232.passu.android.network;


/**
 * This class combine eventCode, xPosition, yPosition, keyCode from packet
 * @author Hyomin Oh
 */
public class EventPacket {
	
	public static final int SETCOORDINATES = 0;
	public static final int TOUCHDOWN = 1;
	public static final int TOUCHUP = 2;
	public static final int KEYDOWN = 3;
	public static final int KEYUP = 4;
	public static final int BACK = 5;
	public static final int HOME = 6;
	public static final int MENU = 7;
	public static final int VOLUMEDOWN = 8;
	public static final int VOLUMEUP = 9;
	public static final int POWER = 10;
	
	public static final int EVENTCODE_SIZE = 2;
	public static final int DATA_SIZE = 4;
	public static final int XPOSITION_SIZE = DATA_SIZE;
	public static final int YPOSITION_SIZE = DATA_SIZE;
	public static final int KEYCODE_SIZE = DATA_SIZE;	
	
	private int eventCode;
	private int xPosition;
	private int yPosition;
	private int keyCode;
	
	
	
	private static byte[] eventValueBuf = new byte[DATA_SIZE];
	private static byte[] bEventCode = new byte[EVENTCODE_SIZE];
		
	private static int ByteToInt(byte [] data){
		int result = 0;
		for(int i=0; i<data.length; i++){
			if(data[i] == ' ')
				continue;
			result = result * 10 + (data[i]-'0');
		}
		return result;
	}
	
	protected EventPacket(){			
	}
	
	public void SetEventCode(int eventCode){
		this.eventCode = eventCode;
	}
	
	public void SetXPosition(int xPosition){
		this.xPosition = xPosition;
	}
	
	public void SetYPosition(int yPosition){
		this.yPosition = yPosition;
	}
	
	public void SetKeyCode(int keyCode){
		this.keyCode = keyCode;
	}
	
	public int GetEventCode(){
		return eventCode;
	}
	
	public int GetXPosition(){
		return xPosition;
	}
	
	public int GetYPosition(){
		return yPosition;
	}
	
	public int GetKeyCode(){
		return keyCode;
	}	
	
	//Parsing EventPacket from packet
	public static EventPacket parse(Packet packet){
		EventPacket eventPacket = new EventPacket();		
		
		byte[] payload = packet.getPayload();	
		
		
		//Event data are written in ASKII Code
		System.arraycopy(payload, 0, bEventCode, 0, EVENTCODE_SIZE);
		eventPacket.SetEventCode(ByteToInt(bEventCode));
		
		
		//X,Y position or Keycode set when eventcode is SETCOORDINATES,TOUCHDOWN,KEYDOWN,KEYUP
		//others are only eventcode
		switch(eventPacket.GetEventCode()){		
		case SETCOORDINATES:		
		case TOUCHDOWN:
			System.arraycopy(payload, EVENTCODE_SIZE, eventValueBuf, 0, XPOSITION_SIZE);
			eventPacket.SetXPosition(ByteToInt(eventValueBuf));
			System.arraycopy(payload, EVENTCODE_SIZE+XPOSITION_SIZE, eventValueBuf, 0, YPOSITION_SIZE);
			eventPacket.SetYPosition(ByteToInt(eventValueBuf));
			break;		
		case KEYDOWN:
		case KEYUP:			
			System.arraycopy(payload, EVENTCODE_SIZE, eventValueBuf, 0, KEYCODE_SIZE);
			eventPacket.SetKeyCode(ByteToInt(eventValueBuf));
			break;				
		}						
		return eventPacket;
	}
	
	@Override
	public String toString(){
		return "EventPacket x="+xPosition+" y="+yPosition;
	}
}
