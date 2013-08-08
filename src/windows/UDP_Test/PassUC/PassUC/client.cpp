#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include "packet.h"
#pragma comment(lib, "ws2_32.lib")

#define BUFSIZE 512

void err_msg(const char *msg, bool option){
	LPVOID lpMsgBuf;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER|
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);


	if(option){
		MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
		LocalFree(lpMsgBuf);
		exit(-1);
	} else{
		printf("[%s] %s", msg, lpMsgBuf);
		LocalFree(lpMsgBuf);
	}
}

int main(void){

	int retval = 0;
	char serverIP[25];
	int serverPort = 0;
	KPACKET kp;

	// 윈속 초기화
	WSADATA wsa;
	if(WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;


	// socket(UDP)
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if( sock == INVALID_SOCKET) err_msg("socket()", TRUE);

	// ip 주소 입력받기
	printf("Server IP 주소 : ");
	scanf("%s\n", serverIP);

	// 소켓 주소 구조체 초기화

	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9501);
	serveraddr.sin_addr.s_addr = inet_addr(serverIP); // 보낼 IP 주소a

	//connect()
	retval = connect(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if( retval == SOCKET_ERROR ) err_msg("connect()", TRUE);

	// 데이터 통신에 사용할 변수
	SOCKADDR_IN peeraddr;
	int addrlen;
	char buf[BUFSIZE + 1];
	char msg[BUFSIZE + 1];
	int len;


	ZeroMemory(buf, sizeof(buf));




	// 클라이언트와 데이터 통신

	while(1){

		// 보낼 데이터 입력
		printf("\n[보낼 데이터] ");
		if(fgets(msg, BUFSIZE + 1 , stdin) == NULL)
			break;

		// \n 문자 제거
		len = strlen(msg);
		if(msg[len - 1] == '\n')
			msg[len - 1] = '\0';
		if(strlen(msg) == 0)
			break;
		kp.inputDev = 0;
		kp.keyCode = 1231;
		kp.recvDev = 5;
		kp.sendDev = 1;
		kp.relativeField = 0;

		strcpy(buf, (char *)&kp);

		// 데이터 보내기
		retval = sendto(sock, buf, strlen(buf), 0, (sockaddr *)&serveraddr, sizeof(serveraddr));
		if(retval == SOCKET_ERROR){
			err_msg("sendto() error!", false);
			continue;
		}

		// 데이터 받기
		addrlen = sizeof(peeraddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0, (sockaddr *)&peeraddr, &addrlen);

		if(retval == SOCKET_ERROR){
			err_msg("recvfrom()", FALSE);
			continue;
		}

		// 송신자 IP 체크
		if(memcmp(&peeraddr, &serveraddr, sizeof(peeraddr))){
			printf("[오류] 잘못된 데이터입니다!\n");
			continue;
		}

		kp = *(KPACKET *)buf;

		//받은 데이터 출력
		buf[retval] = '\0';
		printf("[UDP 클라이언트] %d 바이트를 받았습니다. \n", retval);
		printf("%d, %d, %d, %d, %d\n", kp.inputDev, kp.keyCode, kp.recvDev, kp.relativeField, kp.sendDev);

		//printf("[받은 데이터] %s\n", buf);


	}

	closesocket(sock);

	WSACleanup();

	return 0;

}






