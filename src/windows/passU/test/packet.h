#ifndef __PACKET_H__
#define __PACKET_H__

typedef struct keypacket{ // 키보드 키 값을 가지고 있는 패킷(UDP)
   int sendDev:32;
   int recvDev:32;
   int deviceType:8;
   int relativeField:8;
   int updownFlag:8;
   int keyCode:32;
   int pad1:32;
   int pad2:32;
   int pad3:6;
} KPACKET;
 
typedef struct mousepacket{ // 마우스 위치 정보를 가지고 있는 패킷(UDP)
   int sendDev:32;
   int recvDev:32;
   int deviceType:8;
   int relativeField:8;
   int updownFlag:8;
   int wheelFlag:8;
   int xCoord:32;
   int yCoord:32;
   int pad:32;
} MPACKET;

#endif 