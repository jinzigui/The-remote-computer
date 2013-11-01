
// hackDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// ChackDlg 对话框
class ChackDlg : public CDialogEx
{
// 构造
public:
	ChackDlg(CWnd* pParent = NULL);	// 标准构造函数
	int GetDriverProc(BYTE*DATE);
	int GetDriInfoProc(BYTE*DATE);
// 对话框数据
	enum { IDD = IDD_HACK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnEnChangeEdit1();
protected:
	CImageList m_ImageList_tree;
	CImageList m_ImageList;
public:
	CTreeCtrl m_tree;
	CListCtrl m_list;
	afx_msg void OnConnnect();
	CEdit m_addr;
	CString str;
protected:
	HTREEITEM m_hRoot;   //定义变量得到远程电脑句柄
public:
//	afx_msg void OnSelchangingTree1(NMHDR *pNMHDR, LRESULT *pResult);
	//CListCtrl m_list1;
	afx_msg void OnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
};
