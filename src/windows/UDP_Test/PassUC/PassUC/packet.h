typedef struct keypacket{ // 키보드 키 값을 가지고 있는 패킷(UDP)
	unsigned int sendDev:4;
	unsigned int recvDev:4;
	unsigned int inputDev:1;
	unsigned int relativeField:1;
	unsigned int keyCode:14;
} KPACKET;

typedef struct mousepacket{ // 마우스 위치 정보를 가지고 있는 패킷(UDP)
	unsigned int sendDev:4;
	unsigned int recvDev:4;
	unsigned int inputDev:1;
	unsigned int relativeField:1;
	unsigned int xCoord:7;
	unsigned int yCoord:7;
} MPACKET;

typedef struct clientpacket{ // 맨 처음 Connect 될 때 클라이언트가 서버에게 보내는 패킷
	unsigned int ip:17;
	
	/* 더 추가할 것 있으면 말하3 */

} CPACKET;

typedef struct serverpacket{ // 맨 처음 Connect 될 때 서버가 클라이언트한테 보내는 패킷
	unsigned int position:4;

	/* 더 추가할 것 있으면 말하3 */

} SPACKET;