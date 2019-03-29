#pragma once
#ifndef __SERVER_H__
#define __SERVER_H__
#pragma comment(lib,"ws2_32.lib")
#include <WinSock2.h>
#include <stdio.h>
#include "thread"



#define WM_MY_MESSAGE (WM_USER+100)

struct SockPackage
{
	int p_operate;
};

struct pParam
{
	SOCKET sockClient;
	HWND hwnd;
	
};



enum MyEnum
{
	CONNECT,
	LIGHT,
	SMOKE,
	HEART,
	ADVER,
	CAMERA
};

class SocketServer
{
public:
	SocketServer();
	~SocketServer();
	bool ServerOpen();
	SOCKET cSocket;			
	
private:
	HANDLE rThread;
	DWORD rThreadID;
	
};



#endif