#ifndef __PACKET_H__
#define __PACKET_H__

typedef struct keypacket{ // 키보드 키 값을 가지고 있는 패킷(TCP)
	int msgType:32; // msgType : 1
	int sendDev:32; // id
	int recvDev:32; // first
	int deviceType:8;
	int relativeField:8;
	int updownFlag:8;
	int pad1:8;
	int keyCode:32;
	int pad2:32;
	int pad3:32;
} KPACKET;

typedef struct mousepacket{ // 마우스 위치 정보를 가지고 있는 패킷(TCP)
	int msgType:32; // msgType : 2
	int sendDev:32;
	int recvDev:32;
	int deviceType:8;
	int relativeField:8;
	int updownFlag:8; // 0 : up, 1 : down
	int leftRight:8; // 0 : left , 1 : right
	int wheelFlag:32; // 0 : wheel off, 1 : wheel on 2 : wheel move
	int xCoord:32;
	int yCoord:32;
} MPACKET;

typedef struct c_packet{ // 클라이언트에서 쓸 패킷
	int msgType:32; // msgType : 3
	int c_id:32; // 클라이언트 ID
	int pad3:32;
	int hello:8; // Hello 패킷
	int bye:8; // bye 패킷
	int pad4:8;
	int pad5:8;
	int pad6:32;
	int pad7:32;
	int pad8:32;
} CPACKET;

typedef struct dataPacket{ // 데이터 전송 때 쓸 패킷
	int msgType:32; // msgtype : 4
	int len:32; // data length
	unsigned char data[1024]; // data
} DPACKET;

#endif 