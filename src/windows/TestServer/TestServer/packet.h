#ifndef __PACKET_H__
#define __PACKET_H__

typedef struct keypacket{ // 키보드 키 값을 가지고 있는 패킷(UDP)
   int sendDev:32;			// 보내는 디바이스 위치
   int recvDev:32;			// 받는 디바이스 위치
   int deviceType:8;		// 0: 키보드 1: 마우스
   int relativeField:8;		// 처음 보내는 신호 구분
   int updownFlag:8;		// 업 1 다운 0
   int keyCode:32;			// 키 코드
   int pad1:32;
   int pad2:32;
   int pad3:6;
} KPACKET;
 
typedef struct mousepacket{ // 마우스 위치 정보를 가지고 있는 패킷(UDP)
   int sendDev:32;			// 보내는 디바이스 위치
   int recvDev:32;			// 받는 디바이스 위치
   int deviceType:8;		// 0: 키보드 1: 마우스
   int relativeField:8;		// 처음 보내는 신호 구분
   int updownFlag:8;		// 업 1 다운 0
   int wheelFlag:8;			// 휠 구분
   int xCoord:32;			// x 위치
   int yCoord:32;			// y 위치
   int pad:32;
} MPACKET;

#endif	/* ifndef __PACKET_H__ */