#ifndef __PACKET_H__
#define __PACKET_H__

#pragma pack(1)
/*
#define STATUS_EMPTY	0
#define STATUS_PC		1
#define STATUS_MOBILE	2
*/

typedef struct packet{
	int msgType; // msgType : 1
	int sendDev; // id
	int recvDev; // first
	int deviceType;
	int relativeField;
	int updownFlag;
	int keyCode;
	int pad1;
	int pad2;
	int pad3;
	int pad4;
} PACKET;
typedef struct keypacket{ // 키보드 키 값을 가지고 있는 패킷(TCP)
	int msgType; // msgType : 1
	int sendDev; // id
	int recvDev; // first
	int deviceType;
	int relativeField;
	int updownFlag;
	int keyCode;
	int pad1;
	int pad2;
	int pad3;
	int pad4;
} KPACKET;

typedef struct mousepacket{ // 마우스 위치 정보를 가지고 있는 패킷(TCP)
	int msgType; // msgType : 2
	int sendDev;
	int recvDev;
	int deviceType;
	int relativeField;
	int updownFlag; // 0 : up, 1 : down
	int leftRight; // 0 : left , 1 : right
	int wheelFlag; // 0 : wheel off, 1 : wheel on 2 : wheel move
	int xCoord;
	int yCoord;
} MPACKET;

typedef struct c_packet{ // 클라이언트에서 쓸 패킷
	int msgType; // msgType : 3
	int c_id; // 클라이언트 ID
	int pad3; // 어떤 device인지. Computer : 1 // Android : 2
	int hello; // Hello 패킷
	int bye; // bye 패킷
	int ipFirst;
	int ipSecond;
	int ipThird;
	int ipForth;
} CPACKET;

typedef struct dataPacket{ // 데이터 전송 때 쓸 패킷
	int msgType; // msgtype : 4
	int len; // data length
	unsigned char data[1024]; // data
} DPACKET;

#endif 