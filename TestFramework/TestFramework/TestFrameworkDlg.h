
// TestFrameworkDlg.h : header file
//

#pragma once
#include <queue>
#include "TestInfo.h"

// CTestFrameworkDlg dialog
class CTestFrameworkDlg : public CDialogEx
{
// Construction
public:
	CTestFrameworkDlg(CWnd* pParent = nullptr);	// standard constructor
	~CTestFrameworkDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_TESTFRAMEWORK_DIALOG };
#endif

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
	afx_msg void OnClose();
	DECLARE_MESSAGE_MAP()
public:
	CListBox m_testList;
	afx_msg void OnBnClickedButtonBrowse();
private:
	void GetFuncNamesFromBinary(CString filePath, CString fileName);
	std::queue<TestInfo> m_logQueue;
	static std::queue<TestInfo> m_testQueue;
	static void TestEngine(std::queue<TestInfo>& testQueue);
	static void PostLogMessage(CString* str);
	static void ConsoleOut(char* str);

public:
	afx_msg void OnBnClickedButtonReset();
	afx_msg void OnBnClickedButtonResetlog();
	CListBox m_logList;
	afx_msg void OnBnClickedButtonRuntests();
	afx_msg void OnBnClickedButtonExport();
	afx_msg LRESULT OnLogMessage(WPARAM wParam, LPARAM lParam);
	afx_msg void OnFileOpentests();
	afx_msg void OnFileQuit();
	afx_msg void OnHelpAbout();
	afx_msg void OnEditResettestlist();
	afx_msg void OnEditResetLog();
	afx_msg void OnEditRunTests();
	afx_msg void OnFileExportLog();
};
