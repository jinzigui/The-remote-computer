
// hackDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// ChackDlg �Ի���
class ChackDlg : public CDialogEx
{
// ����
public:
	ChackDlg(CWnd* pParent = NULL);	// ��׼���캯��
	int GetDriverProc(BYTE*DATE);
	int GetDriInfoProc(BYTE*DATE);
// �Ի�������
	enum { IDD = IDD_HACK_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
	HTREEITEM m_hRoot;   //��������õ�Զ�̵��Ծ��
public:
//	afx_msg void OnSelchangingTree1(NMHDR *pNMHDR, LRESULT *pResult);
	//CListCtrl m_list1;
	afx_msg void OnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
};
