
// hackDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "hack.h"
#include "hackDlg.h"
#include "afxdialogex.h"

#include"password.h"
#include"SYS.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//加载套接字
WSADATA wsadata;
SOCKET  ServerSocket ;  
SOCKADDR_IN  ServerAdd;  //地址族
int port=12345;
HTREEITEM SelectNode;
typedef struct 
{
	int ID;              //请求的磁盘ID
	BYTE lpram[2048];    //包含请求的一些附加信息
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

DWORD WINAPI CLisen(LPVOID lparam);

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// ChackDlg 对话框



ChackDlg::ChackDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ChackDlg::IDD, pParent)
	, str(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ChackDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_tree);
	DDX_Control(pDX, IDC_EDIT1, m_addr);
	DDX_Control(pDX, IDC_LIST1, m_list);
}

BEGIN_MESSAGE_MAP(ChackDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &ChackDlg::OnConnnect)
//	ON_NOTIFY(TVN_SELCHANGING, IDC_TREE1, &ChackDlg::OnSelchangingTree1)
ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &ChackDlg::OnSelchangedTree1)
END_MESSAGE_MAP()


// ChackDlg 消息处理程序

BOOL ChackDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	GetDlgItem (IDC_EDIT1)->SetWindowText("125.217.40.116");    //设置Ip

	//树列表框
	m_ImageList_tree.Create(32,32,ILC_COLOR32,20,40);

	//加载图标
	HICON hicon=NULL;

	//加载我的电脑
	hicon=(HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON2),IMAGE_ICON,16,16,NULL);
	m_ImageList_tree.Add(hicon);

	hicon=(HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,16,16,NULL);
	m_ImageList_tree.Add(hicon);

	hicon=(HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON3),IMAGE_ICON,16,16,NULL);
	m_ImageList_tree.Add(hicon);

	//m_tree是IDC_TREE的成员

	m_tree.SetImageList(&m_ImageList_tree,TVSIL_NORMAL);
	DWORD dwStyle=GetWindowLong(m_tree.m_hWnd,GWL_STYLE);
	dwStyle|= TVS_HASBUTTONS |TVS_HASLINES | TVS_LINESATROOT;   //+号和线 
	SetWindowLong (m_tree.m_hWnd,GWL_STYLE,dwStyle);
	m_hRoot=m_tree.InsertItem("我的电脑",0,0,0,0);     //得到远程电脑的句柄   
	//m_tree.InsertItem ("")
	//GetDlgItem(IDC_EDIT2)->SetWindowTextA ("无连接状态");

	//初始化套接字
	WORD ver=MAKEWORD (2,2);
	WSAStartup (ver,&wsadata);



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void ChackDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void ChackDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR ChackDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void ChackDlg::OnConnnect()
{
	// TODO: 在此添加控件通知处理程序代码

	//调用UpdateData()函数使得输入的IP地址变得可用
	UpdateData (true);
	if(m_addr.LineLength()==0)
	{
		MessageBox("NULL");
		return ;
	}
	//获取ip编辑框中输入的ip地址
	m_addr.GetWindowText(str);
	ServerSocket=socket(AF_INET,SOCK_STREAM,0);
	ServerAdd.sin_family=AF_INET;
	ServerAdd.sin_port =htons(port);
	//照教程中调用LockBuffer()不成功，提示说没有改函数，然后添加CString str变量保存输入的ip
	ServerAdd.sin_addr.S_un.S_addr=inet_addr(str.GetBuffer());
	//连接函数
	if(connect(ServerSocket,(SOCKADDR*)&ServerAdd,sizeof(ServerAdd))!=0)
	{
		MessageBox("连接失败！");
		GetDlgItem(IDC_EDIT2)->SetWindowTextA ("连接服务器失败");
		Sleep(1000);
		GetDlgItem(IDC_EDIT2)->SetWindowTextA ("无连接状态");
		return ;
	}
	else
	{
		GetDlgItem(IDC_EDIT2)->SetWindowTextA ("连接服务器成功！");
		//password dlg;
		//dlg.DoModal ();
		/*char buff[20];
		if(recv(ServerSocket,buff,sizeof(buff),0)!=SOCKET_ERROR)
		{
		if(strcmp(buff,"kjinziguiteer")==0)
		{
		password dlg;
		dlg.DoModal ();
		}
		}*/

		COMMAND command;
		memset ((char*)&command,0,sizeof(command));
		command.ID =GetDriver;  
		//请求得到磁盘的信息
		send(ServerSocket,(char*)&command,sizeof(command),0 );
		CreateThread (NULL,NULL,CLisen,this,NULL,NULL);

	}

}


DWORD WINAPI CLisen (LPVOID lparam)
{
	ChackDlg *dlg=(ChackDlg *)lparam;    //转换为对话框变量
	COMMAND command;                    //储存服务端返回的信息
	memset ((char*)&command,0,sizeof(command));
	while(true)
	{
		memset ((char*)&command,0,sizeof(command));
		if(recv(ServerSocket ,(char*)&command,sizeof(command),0)==SOCKET_ERROR )
		{
			//GetDlgItem(IDC_EDIT2)->SetWindowTextA ("连接服务器失败");
			break;

		}
		else
		{
			switch(command.ID)
			{
			case GetDriver :
				dlg->GetDriverProc(command.lpram);
				break;
			case GetDriInfo:
				dlg->GetDriInfoProc(command.lpram);
				break;
			}
		}
	}
	closesocket(ServerSocket);
	return 0;
}

int ChackDlg::GetDriverProc(BYTE *DATE)
{
	char buff[1024];
	strcpy(buff,(char*)DATE);  //得到磁盘信息
	m_tree .InsertItem (buff,1,1,m_hRoot ,0);   //将服务端得到的信息插进远程电脑列表下端
	m_tree.Expand (m_hRoot,TVE_EXPAND );
	return 0;
}

int ChackDlg::GetDriInfoProc(BYTE *DATE)
{
	FILEINFO *fi=(FILEINFO*)DATE;
	//memset((char*)&fi,0,sizeof(fi));
	if(fi->IsDir )
	{
		m_tree.InsertItem(fi->FileName,2,2,SelectNode,0);
		m_tree.Expand(SelectNode,TVE_EXPAND);
	}
	else
	{
		SHFILEINFO info;
		memset((char*)&info,0,sizeof(info));
		SHGetFileInfo(fi->FileName ,0,&info,sizeof(info),SHGFI_ICON | SHGFI_USEFILEATTRIBUTES);
		int i;
		i=m_ImageList_tree.Add(info.hIcon);  //shit 疯狂的bug！！
		//i=m_ImageList .Add(info.hIcon );
		m_list.InsertItem(i,fi->FileName,i);
	}
	return 0;

}



//void ChackDlg::OnSelchangingTree1(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
//	// TODO: 在此添加控件通知处理程序代码
//	TVITEM item=pNMTreeView->itemNew;
//	m_list.DeleteAllItems();
//	SelectNode=item.hItem;     //逼图比如点击c盘就保存c盘
//	if(item.hItem==m_hRoot)
//	{
//		return ;
//	}
//	HTREEITEM CurrentNode;
//	CurrentNode=item.hItem;
//	while(m_tree.GetChildItem(CurrentNode)!=NULL)
//	{
//		//该函数功能是点击C-->windows-->windows子项，再点击c,就清除windows下的所有列表
//		m_tree.DeleteItem(m_tree.GetChildItem(CurrentNode));
//	}
//
//	CString Temp;
//	CString FullPath;
//	while(CurrentNode!=m_hRoot)
//	{
//		Temp=m_tree.GetItemText(CurrentNode);
//		if(Temp!="\\")
//		{
//			Temp+="\\";
//		}
//		FullPath+=Temp;
//		CurrentNode=m_tree.GetParentItem(CurrentNode);    //此时CurrentNode=m_hRoot,跳出循环
//	}
//	COMMAND command;
//	command.ID=GetDriInfo;
//	strcpy((char*)command.lpram,FullPath);
//	if(send(ServerSocket,(char*)&command,sizeof(command),0)==SOCKET_ERROR)
//	{
//		AfxMessageBox ("发送磁盘信息错误！");
//	}
//	*pResult = 0;
//}


void ChackDlg::OnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	TVITEM item=pNMTreeView->itemNew;
	m_list.DeleteAllItems();
	SelectNode=item.hItem;     //逼图比如点击c盘就保存c盘
	if(item.hItem==m_hRoot)
	{
		return ;
	}
	HTREEITEM CurrentNode;
	CurrentNode=item.hItem;
	while(m_tree.GetChildItem(CurrentNode)!=NULL)
	{
		//该函数功能是点击C-->windows-->windows子项，再点击c,就清除windows下的所有列表
		m_tree.DeleteItem(m_tree.GetChildItem(CurrentNode));
	}

	CString Temp;
	CString FullPath;
	while(CurrentNode!=m_hRoot)
	{
		Temp=m_tree.GetItemText(CurrentNode);
		if(Temp!="\\")
		{
			Temp+="\\";
		}
		FullPath=Temp+FullPath;
		CurrentNode=m_tree.GetParentItem(CurrentNode);    //此时CurrentNode=m_hRoot,跳出循环
	}
	COMMAND command;
	memset((char*)&command,0,sizeof(command));//一定要重置啊！！！！
	command.ID=GetDriInfo;
	strcpy((char*)command.lpram,FullPath);
	if(send(ServerSocket,(char*)&command,sizeof(command),0)==SOCKET_ERROR)
	{
		AfxMessageBox ("发送磁盘信息错误！");
	}
	*pResult = 0;
}
