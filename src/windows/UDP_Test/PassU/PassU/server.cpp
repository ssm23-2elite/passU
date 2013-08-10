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

	int retval;

	//윈속 초기화
	
	WSADATA wsa;
	KPACKET kp;

	if( WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	// socket 생성
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);

	// SOCK_STREAM : 데이터가 보낸 순서대로 도착
	// SOCK_DGRAM : 데이터가 보낸 순서대로 도착하지 않을 수도 있음
	if( sock == INVALID_SOCKET) err_msg("socket() error", TRUE);

	// BIND
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(9501);
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);

	retval = bind(sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	if( retval == SOCKET_ERROR ) err_msg("connect() error", TRUE);

	SOCKADDR_IN clientaddr;
	int addrlen;
	char buf[BUFSIZE + 1];

	// 통신

	while(1){
		addrlen = sizeof(clientaddr);
		retval = recvfrom(sock, buf, BUFSIZE, 0, (SOCKADDR *)&clientaddr, &addrlen);
		if(retval == SOCKET_ERROR){
			err_msg("recvfrom()", FALSE);
			continue;
		}



		buf[retval] = '\0';
		
		kp = *(KPACKET *)buf;

		printf("%d %d %d %d %d\n", kp.inputDev, kp.keyCode, kp.recvDev, kp.relativeField, kp.sendDev);


		kp.inputDev = 0;
		kp.keyCode = 1;
		kp.recvDev = 2;
		kp.relativeField = 0;
		kp.sendDev = 5;
		
		//printf("[UDP\%s:%d] %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), buf);

		strcpy(buf, (char *)&kp);

		retval = sendto(sock, buf, retval, 0, (SOCKADDR *)&clientaddr, sizeof(clientaddr));
		if(retval == SOCKET_ERROR){
			err_msg("sentto()", FALSE);
			continue;
		}
	}
	
	closesocket(sock);


	WSACleanup();


	return 0;
}