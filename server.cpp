#include "pch.h"
#include "server.h"
#include <iostream>
#include <stdlib.h>
#include <string>
#include <fstream>

SocketServer::SocketServer()
{
	rThread = 0;
	
}

SocketServer::~SocketServer()
{
}

bool SocketServer::ServerOpen()
{
	WSADATA wsaData;
	char buff[1024];
	memset(buff, 0, sizeof(buff));

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		//		printf("Failed to load Winsock");
		return false;
	}
	std::ifstream ifile("C:/connect.txt");
	if (!ifile.is_open())
	{
		return false;
	}
	char buffer[256];
	ifile.getline(buffer, 100);
	char* buf = &buffer[3];
	
	SOCKADDR_IN addrSrv;
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(7000);
	addrSrv.sin_addr.S_un.S_addr = inet_addr(buf);//htonl(INADDR_ANY);////////////////////////////////

												  
	cSocket = socket(AF_INET, SOCK_STREAM, 0);//´´½¨Ì×½Ó×Ö  
	if (SOCKET_ERROR == cSocket) {
		printf("Socket() error:%d", WSAGetLastError());
		return false;
	}

	if (connect(cSocket, (struct  sockaddr*)&addrSrv, sizeof(addrSrv)) == INVALID_SOCKET) {
		printf("Connect failed:%d", WSAGetLastError());
		return false;
	}

	ifile.close();

	return true;
}



