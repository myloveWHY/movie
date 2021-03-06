// movie.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <time.h>  
#include "server.h"
#include <opencv2\highgui\highgui.hpp>
#include <fstream>

using namespace cv;
using namespace std;


int flag;
HANDLE hSemaphore;

struct mParam
{
	int i;
	bool vidio;
	int timeCam;
	VideoCapture *adv;
	VideoCapture cam;
	char buffer[64];
//	HWND hwnd;
};

//DWORD WINAPI TimerThread(LPVOID lParam);	//计时器线程

DWORD WINAPI MovieThread(LPVOID lParam);
DWORD WINAPI RecvThread(LPVOID lParam);
//设置程序开机启动
BOOL AutoBootSet();
BOOL IsAutoBoot();

int main()
{
#if 0
	time_t nowTime;
	
	nowTime = time(NULL);
	struct tm *sysTime = localtime(&nowTime);
	/*
	  美国国家标准协会规定 C99(C++11)
	  tm_year:从1900年开始计算，所以如果直接打印tm_year，将得到1900至当前年份已过去的年数,如现在是2015年，则打印115,所以要加1900
	  tm_mon:是由下标为0开始计算的，即0代表1月份，所以要加1，比如，当前是三月份，如果不加1，则显示是2.
	  其它略，可参考C99或C++11标准规定。
	*/
	//printf("系统时间：%d年%d月%d日%d时%d分\n", sysTime->tm_year + 1900, sysTime->tm_mon + 1, sysTime->tm_mday, sysTime->tm_hour, sysTime->tm_min);
#endif
	if (IsAutoBoot())
	{
		Sleep(10000);
	}
	

	BOOL autoOpen = AutoBootSet();
	if (autoOpen == false)
	{
		cout << "设置开机自动开启失败" << endl;

	}


	hSemaphore = CreateSemaphore(NULL, 0, 2, NULL);
	//打开视频文件
	VideoCapture adv[10];
	int i = 0;
	string buff;
	for (i = 0; i < 10; i++) 
	{
		buff = "adv" + to_string(i) + ".mp4";
		adv[i].open(buff.data());
		if (!adv[i].isOpened())
		{
			cout << "不能打开视频文件" + buff << endl;
			i--;
			break;
		}
		//从0ms开始播放视频
		adv[i].set(CV_CAP_PROP_POS_MSEC, 0);
	}


//	namedWindow("MyAdv", CV_WINDOW_AUTOSIZE);
//	int width = GetSystemMetrics(SM_CXSCREEN);
//	int height = GetSystemMetrics(SM_CYSCREEN);
//	adv.set(CV_CAP_PROP_FRAME_WIDTH, width);
//	adv.set(CV_CAP_PROP_FRAME_HEIGHT, height);

	// 打开第1个摄像头
	VideoCapture camA(1);
	if (!camA.isOpened())
	{
		cout << "不能打开摄像头1！" << endl;
		while (1);
	}
	VideoCapture camB(0);
	if (!camA.isOpened())
	{
		cout << "不能打开摄像头0！" << endl;
		while (1);
	}

	//获取视频帧的宽度
//	double dWidth = cam.get(CV_CAP_PROP_FRAME_WIDTH);
	//获取视频帧的高度
//	double dHeight = cam.get(CV_CAP_PROP_FRAME_HEIGHT);
	double width =  GetSystemMetrics(SM_CXSCREEN);
	double height =  GetSystemMetrics(SM_CYSCREEN);

	char nameA[16] = "MyVideo1";
	
	camA.set(CV_CAP_PROP_FRAME_WIDTH, width / 2);
	camA.set(CV_CAP_PROP_FRAME_HEIGHT, height);
	cout << "视频帧尺寸 : " << width << " x " << height << endl;


	char nameB[16] = "MyVideo2";
	camB.set(CV_CAP_PROP_FRAME_WIDTH, width / 2);
	camB.set(CV_CAP_PROP_FRAME_HEIGHT, height);
//	cout << "视频帧尺寸 : " << width << " x " << height << endl;
/*	namedWindow(nameB, CV_WINDOW_NORMAL);
//	setWindowProperty(nameB, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	cvMoveWindow(nameB, width / 2, 0);
*/
	bool vid = true;
	if (height > width)
	{
		vid = false;
	}
//	namedWindow("MyCam", CV_WINDOW_AUTOSIZE);
	//设置摄像头的播放时间
	std::ifstream ifile("Time.txt");

	if (!ifile.is_open())
	{
		cout << "不能打开Time.txt！" << endl;
		while (1);
	}

	char buffer[128];
	ifile.getline(buffer, 100);
	char* buf = &buffer[12];
	int timeCam = atoi(buf);
	
	mParam* ppa = (mParam*)malloc(sizeof(mParam));
	memset(ppa, 0, sizeof(mParam));
	ppa->adv = adv;
	ppa->cam = camA;
	ppa->i = i;
	ppa->timeCam = timeCam*100 + 1;
	ppa->vidio = vid;
	memcpy(ppa->buffer, nameA, 16);
	

	mParam* ppb = (mParam*)malloc(sizeof(mParam));
	memset(ppb, 0, sizeof(mParam));
	ppb->adv = adv;
	ppb->cam = camB;
	ppb->i = i;
	ppb->timeCam = timeCam * 100 + 1;
	ppb->vidio = vid;
	memcpy(ppb->buffer, nameB, 15);
	ifile.close();

#if 0
	//连接服务器
	SocketServer* server = new SocketServer;
	if (server->ServerOpen == false)
	{
		cout << "连接服务器失败" << endl;
		return -1;
	}


	pParam* pp = (pParam*)malloc(sizeof(pParam));
	memset(pp, 0, sizeof(pParam));
	/*
		CWnd *wnd = AfxGetMainWnd();
		HWND hwnd = wnd->GetSafeHwnd();
		pp->hwnd = hwnd;
	*/

	pp->sockClient = cSocket;
	rThread = CreateThread(NULL, 0, RecvThread, (LPVOID)pp, 0, NULL);
	
	HANDLE RecvThread = CreateThread(NULL, 0, RecvThread, NULL, 0, NULL);
#endif
	HANDLE movieThreadA = CreateThread(NULL, 0, MovieThread, (LPVOID)ppa, 0, NULL);
	HANDLE movieThreadB = CreateThread(NULL, 0, MovieThread, (LPVOID)ppb, 0, NULL);

//	CloseHandle(movieThread);	
	WaitForSingleObject(hSemaphore, INFINITE);
//	WaitForSingleObject(hSemaphore, INFINITE);

	CloseHandle(hSemaphore);
	CloseHandle(movieThreadA);
	CloseHandle(movieThreadB);
	delete ppa;
	delete ppb;

	return 0;
}



DWORD WINAPI MovieThread(LPVOID lParam)
{
	cout << "线程创建成功" << endl;
	mParam* pp = (mParam *)lParam;
	VideoCapture cap;
	double width = GetSystemMetrics(SM_CXSCREEN);
	double height = GetSystemMetrics(SM_CYSCREEN);
//	namedWindow(nameA, CV_WINDOW_AUTOSIZE);
//	namedWindow(pp->buffer, CV_WINDOW_NORMAL);
	namedWindow(pp->buffer, CV_WINDOW_NORMAL);
	resizeWindow(pp->buffer, width / 2, height);
//	setWindowProperty(pp->buffer, CV_WND_PROP_FULLSCREEN, CV_WINDOW_FULLSCREEN);
	if (strcmp(pp->buffer,"MyVideo1") == 0)
	{
		cvMoveWindow(pp->buffer, 0, 0);
	}
	else
	{
		cvMoveWindow(pp->buffer, width / 2, 0);
	}
	
	HWND hwnd = (HWND)cvGetWindowHandle(pp->buffer);
	cout << "1" << endl;
	HWND phwnd = ::GetParent(hwnd);
	bool bRet = ::SetWindowPos(phwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	if (!bRet)
	{
		cout << "set error" << endl;
		exit(0);
	}
//	bool flag = false;
	flag = 0;
	int temp = 0;
	int MyTime = 0;
	double fps;
	int i = 0;
	cout << "1" << endl;
	while (1)
	{
		Mat frame;
		Mat frame1;
		Mat frame2;
		if (temp == 1 && flag == 1)
		{
			cap = *((pp->adv) + i);
			cap.set(CV_CAP_PROP_POS_MSEC, 0);
			fps = cap.get(CV_CAP_PROP_FPS);
			fps = 1000 / (fps + 6);
			flag--;
			i++;
			temp = 2;
			MyTime = 2;
			if (i > pp->i) 
			{
				i = 0;
			}
		}
		if (temp == 0 && flag == 0)
		{
			cap = pp->cam;
			fps = 10;
			fps = 1000 / fps;
			temp = 1;
			MyTime = 1;
		}
		//从视频中读取一个帧
		bool bSuccess = cap.read(frame);
		if (!bSuccess)
		{
			cout << "不能从视频文件读取帧" << endl;
			break;
		}
		if (pp->vidio)
		{
			flip(frame, frame2, 1);
		}
		else
		{
			transpose(frame, frame1);
			flip(frame1, frame2,-1);
		}

		//在MyVideo窗口上显示当前帧
		if (temp == 1) 
		{
			imshow(pp->buffer, frame2);
		}
		else 
		{
			imshow(pp->buffer, frame);
		}
		
		//如果30ms没有按下键，就返回-1，当按下ESC键返回27
		if (waitKey(fps) == 27)
		{
			cout << "按下ESC键" << endl;
			break;
		}
		MyTime += 10;
		if (MyTime == pp->timeCam)
		{
			temp = 1;
			MyTime = 1;
			flag++;
		}
		if (temp == 2 && cap.get(CV_CAP_PROP_POS_FRAMES) >= cap.get(CV_CAP_PROP_FRAME_COUNT))
		{
			temp = 0;
			MyTime = 0;
		}
		/*
		if (temp == 2002)
		{
			temp = 0;
			MyTime = 0;
		}
		*/
	}
	ReleaseSemaphore(hSemaphore, 1, NULL);

	return 0;
}

DWORD WINAPI RecvThread(LPVOID lParam)
{
	char buff[1024];
	memset(buff, 0, sizeof(buff));
	pParam* pp = (pParam *)lParam;
	while (1)
	{
		int result = recv(pp->sockClient, buff, sizeof(buff), 0);
		if (result < 1)
		{
			break;
		}
		struct SockPackage* pack = (struct SockPackage*)buff;
		//		int * pi = (int*)buff;
		switch (pack->p_operate)
		{
		case ADVER:
		{
			flag++;
			break;
		}
		case SMOKE:

			break;
		case HEART:
			//心跳
//			send(pp->sockClient, (const char*)pack, sizeof(pack), 0);            //发送心跳信息
			break;
		default:
			break;
		}
		memset(buff, 0, sizeof(buff));
	}

	return 0;
}

//设置程序开机启动
BOOL AutoBootSet()
{
	LPCTSTR lpSubKey = _T("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run");

	HKEY hKey;

	REGSAM flag = KEY_WOW64_64KEY;//当前系统为win7 64位，访问的是64位的注册表，如果访问32位，则改为KEY_WOW64_32KEY

	LONG lRet = RegOpenKeyEx(HKEY_LOCAL_MACHINE, lpSubKey, 0, KEY_ALL_ACCESS|flag,&hKey);

	if (ERROR_SUCCESS != lRet)
	{
		cout << "RegOpenKeyEx fail!" << endl;
		return false;
	}
 

	LPCTSTR pchrName = _T("D:\\Program Files (x86)\\test.exe");
	lRet = RegSetValueEx(hKey, TEXT("TEST"), 0, REG_SZ, (LPBYTE)pchrName, wcslen(pchrName) * sizeof(TCHAR) + 1);

	if (ERROR_SUCCESS != lRet)
	{
		cout << "RegSetValueEx fail!" << endl;
		return false;
	}

	RegCloseKey(hKey);

	return true;
		
}


BOOL IsAutoBoot()
{
	LPWSTR *szArglist = NULL;
	int nArgs = -1;
	BOOL bRet = FALSE;
	szArglist = CommandLineToArgvW(GetCommandLine(), &nArgs);
	if ((NULL == szArglist) || (nArgs == 2))
	{
		if (_tcsicmp(szArglist[1], _T("-auto")) == 0)
		{
			bRet = TRUE;			//开机启动
		}
		else
		{
			bRet = FALSE;
		}
	}
	else
	{
		bRet = FALSE;
	}
	LocalFree(szArglist);
	return bRet;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门提示: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
