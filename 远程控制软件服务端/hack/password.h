#pragma once


// password �Ի���

class password : public CDialogEx
{
	DECLARE_DYNAMIC(password)

public:
	password(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~password();

// �Ի�������
	enum { IDD = IDD_password };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
