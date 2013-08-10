#ifndef __PACKET_H__
#define __PACKET_H__

typedef struct packet{ // 키보드 키 값을 가지고 있는 패킷(UDP)
   unsigned int sendDev:4;			// 보내는 장치
   unsigned int recvDev:4;			// 받는 장치
   unsigned int inputDev:1;			// 입력 장치 구분 0: 키보드, 1: 마우스
   unsigned int relativeField:1;	// 상대값 여부
   unsigned int pad:22;				// 여분
} PACKET;

typedef struct keypacket{ // 키보드 키 값을 가지고 있는 패킷(UDP)
   unsigned int sendDev:4;			// 보내는 장치
   unsigned int recvDev:4;			// 받는 장치
   unsigned int inputDev:1;			// 입력 장치 구분 0: 키보드, 1: 마우스
   unsigned int relativeField:1;	// 상대값 여부
   unsigned int keyCode:14;			// 키코드
   unsigned int pad:8;				// 여분
} KPACKET;
 
typedef struct mousepacket{ // 마우스 위치 정보를 가지고 있는 패킷(UDP)
   unsigned int sendDev:4;			// 보내는 장치
   unsigned int recvDev:4;			// 받는 장치
   unsigned int inputDev:1;			// 입력 장치 구분 0: 키보드, 1: 마우스
   unsigned int relativeField:1;	// 상대값 여부
   unsigned int xCoord:7;			// x 좌표
   unsigned int yCoord:7;			// y 좌표
   unsigned int wheel:1;			// 휠 On/Off
   unsigned int rightmouse:1;		// 오른쪽 마우스 On/Off
   unsigned int leftmouse:1;		// 왼쪽 마우스 On/Off
   unsigned int pad:4;
} MPACKET;


typedef struct clientpacket{ // 맨 처음 Connect 될 때 클라이언트가 서버에게 보내는 패킷
	unsigned int ip:17;
	
	/* 더 추가할 것 있으면 말하3 */

} CPACKET;

typedef struct serverpacket{ // 맨 처음 Connect 될 때 서버가 클라이언트한테 보내는 패킷
	unsigned int position:4;

	/* 더 추가할 것 있으면 말하3 */

} SPACKET;

#endif