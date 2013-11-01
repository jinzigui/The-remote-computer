#pragma once


// password 对话框

class password : public CDialogEx
{
	DECLARE_DYNAMIC(password)

public:
	password(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~password();

// 对话框数据
	enum { IDD = IDD_password };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
