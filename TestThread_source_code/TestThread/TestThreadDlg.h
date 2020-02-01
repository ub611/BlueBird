
// TestThreadDlg.h : header file
//

#pragma once
#include "afxwin.h"

// CTestThreadDlg dialog
class CTestThreadDlg : public CDialog
{
// Construction
public:
	CTestThreadDlg(CWnd* pParent = NULL);	// standard constructor
	DWORD m_idThread;

// Dialog Data
	enum { IDD = IDD_TESTTHREAD_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP();
public:
	afx_msg void OnBnClickedButtonCreatethread();
	afx_msg void OnBnClickedButtonThreadHello();
	afx_msg void OnBnClickedButtonQuitthread();
	afx_msg LRESULT CTestThreadDlg::OnMainTray(WPARAM wParam, LPARAM lParam);
	afx_msg void CreateTray();
	afx_msg LONG OnTrayNotify(WPARAM wParam, LPARAM lParam);
	CStatic m_stCaption;
	LRESULT OnThreadQuitFunction(WPARAM wParam, LPARAM lParam);
protected:
	afx_msg LRESULT OnThreadCheckFunction(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnEnChangeEdit1();
	afx_msg void OnPopOpen();
	afx_msg void OnPopDown();


	
	afx_msg void OnStnClickedStaticCaption();
};
LRESULT CALLBACK Disablekeys(int nCode, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK DisableMouse(int nCode, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK Wndproc(HWND, UINT, WPARAM, LPARAM);
