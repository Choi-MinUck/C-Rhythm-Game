#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "networking.h"

#define BUFSIZE 512

#pragma comment(lib,"ws2_32.lib")

int send_score(char* name, int score, int map_number, char* play_data)
{
	int retval;
	char buf[BUFSIZE];

	//���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return -1;

	//socket()
	SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == INVALID_SOCKET) return 0;

	HOSTENT* remoteHost = gethostbyname("rhythm_rinmin.kys");//���ּҸ� ip�� �ٲٱ� ����
	//connect()
	SOCKADDR_IN serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(80);
	serveraddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*) * remoteHost->h_addr_list));
	retval = connect(sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr));

	if (retval == SOCKET_ERROR) return 0;

	char msg[500] = {NULL};
	sprintf(msg, "GET /Handle.php?name=%s&score=%d&map=%d&p_data=%s HTTP/1.1\r\n",name,score,map_number,play_data);
	strcat(msg, "Host: rhythm_rinmin.kys:80\r\n\r\n");//�������� ���� ���

	send(sock, msg, strlen(msg), 0);//�������� ������

	while (1)
	{
		if ((retval = recv(sock, buf, BUFSIZE, 0)) >= 0) break;//�������� ���� �ޱ�

		printf("������ ������..");
		//printf("%s", buf);//ȭ�鿡 ���(�ʹ� ���Ƽ� ©���� ����)

		memset(buf, 0, BUFSIZE);//���� �����
	}
	closesocket(sock);
	//���� ����
	WSACleanup();
	return 0;
}