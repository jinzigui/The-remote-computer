// password.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "hack.h"
#include "password.h"
#include "afxdialogex.h"


// password �Ի���

IMPLEMENT_DYNAMIC(password, CDialogEx)

password::password(CWnd* pParent /*=NULL*/)
	: CDialogEx(password::IDD, pParent)
{

}

password::~password()
{
}

void password::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(password, CDialogEx)
END_MESSAGE_MAP()


// password ��Ϣ�������
