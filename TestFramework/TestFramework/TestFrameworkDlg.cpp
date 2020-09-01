
// TestFrameworkDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "TestFramework.h"
#include "TestFrameworkDlg.h"
#include "afxdialogex.h"
#include <thread>
#include <string>
#include <conio.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Prototype for the test functions
typedef bool(__stdcall* testFunc)();

// Initailize static members
std::queue<TestInfo>CTestFrameworkDlg::m_testQueue;

// Message glue
#define WM_USER_LOG_MESSAGE  WM_USER + 1

// Maximum text buffer size
#define MAX_BUFFER 1024

// Max test engine threads
#define THREAD_POOL_MAX 5

CMutex m_mutex;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:

};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	
END_MESSAGE_MAP()


// CTestFrameworkDlg dialog



CTestFrameworkDlg::CTestFrameworkDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_TESTFRAMEWORK_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CTestFrameworkDlg::~CTestFrameworkDlg()
{

}

void CTestFrameworkDlg::OnClose()
{
	// Deallocate the test info
	for (int i = 0; i < m_testList.GetCount(); i++)
	{
		TestInfo* testInfo = (TestInfo*)m_testList.GetItemData(i);

		if (testInfo)
		{
			delete testInfo;
		}
	}

	// Clear lists
	m_testList.ResetContent();
	m_logList.ResetContent();

	// Deallocate the test engine console
	FreeConsole();

	// Call the base method OnClose
	__super::OnClose();
}

void CTestFrameworkDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_TESTS, m_testList);
	DDX_Control(pDX, IDC_LIST2, m_logList);
}

BEGIN_MESSAGE_MAP(CTestFrameworkDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CTestFrameworkDlg::OnBnClickedButtonBrowse)
	ON_BN_CLICKED(IDC_BUTTON_RESET, &CTestFrameworkDlg::OnBnClickedButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_RESETLOG, &CTestFrameworkDlg::OnBnClickedButtonResetlog)
	ON_BN_CLICKED(IDC_BUTTON_RUNTESTS, &CTestFrameworkDlg::OnBnClickedButtonRuntests)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT, &CTestFrameworkDlg::OnBnClickedButtonExport)
	ON_MESSAGE(WM_USER_LOG_MESSAGE, OnLogMessage) // custom message to handle log PostMessages
	ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_OPENTESTS, &CTestFrameworkDlg::OnFileOpentests)
	ON_COMMAND(ID_FILE_QUIT, &CTestFrameworkDlg::OnFileQuit)
	ON_COMMAND(ID_HELP_ABOUT, &CTestFrameworkDlg::OnHelpAbout)
	ON_COMMAND(ID_EDIT_RESETTESTLIST, &CTestFrameworkDlg::OnEditResettestlist)
	ON_COMMAND(ID_EDIT_RESETLOG, &CTestFrameworkDlg::OnEditResetLog)
	ON_COMMAND(ID_EDIT_RUNTESTS, &CTestFrameworkDlg::OnEditRunTests)
	ON_COMMAND(ID_FILE_EXPORTLOG, &CTestFrameworkDlg::OnFileExportLog)
END_MESSAGE_MAP()


// CTestFrameworkDlg message handlers

BOOL CTestFrameworkDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_testList.AddString("Please click the Browse for Tests button to select tests to run.");
	m_firstStart = true;

	for (int i = 0; i < THREAD_POOL_MAX; i++)
	{
		// start Test Engine
		std::thread{ CTestFrameworkDlg::TestEngine, std::ref(CTestFrameworkDlg::m_testQueue) }.detach();
	}


	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestFrameworkDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestFrameworkDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestFrameworkDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTestFrameworkDlg::OnBnClickedButtonBrowse()
{
	const TCHAR szFilter[] = _T("Test Binaries (*.dll;*.jar)|*.dll;*.jar|All Files (*.*)|*.*||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT | OFN_FILEMUSTEXIST, szFilter, this);

	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		CString fileName = dlg.GetFileName();

		if (!fileName.IsEmpty())
		{
			// single selection
			GetFuncNamesFromBinary(filePath, fileName);
		}
		else
		{
			// multiple selection
			char* str = dlg.m_ofn.lpstrFile;
			std::string directory = str;
			str += (directory.length() + 1);
			while (*str)
			{
				std::string filename = str;
				str += (filename.length() + 1);

				CString fullFilePath = filePath + "\\" + filename.c_str();
				GetFuncNamesFromBinary(fullFilePath, filename.c_str());
			}
		}
	}
}

void CTestFrameworkDlg::GetFuncNamesFromBinary(CString filePath, CString fileName)
{
	// get a handle to the test dll
	HINSTANCE hTestDll = LoadLibrary(filePath);
	if (hTestDll != NULL)
	{
		for (int i = 1; i < 10; i++)
		{
			CString funcName;
			funcName.Format(_T("Test%d"), i);

			// convert CString to LPCTR
			CStringA tempCString(funcName);
			LPCSTR lpctrFuncName = tempCString;

			// get a pointer to the function
			testFunc pTestFunc = (testFunc)GetProcAddress(hTestDll, lpctrFuncName);
			if (NULL != pTestFunc)
			{
				// if this is the first run, reset the list contents
				if (m_firstStart)
				{
					m_testList.ResetContent();
					m_firstStart = false;
				}

				// add the function name to the available tests
				CString tmp;
				tmp.Format(_T("%s --> %s()"), fileName, funcName);
				int currItemIndex = m_testList.AddString(tmp);

				// save test data
				TestInfo* testInfo = new TestInfo();
				testInfo->setFilePath(filePath);
				testInfo->setFileName(fileName);
				testInfo->setTestName(funcName);

				m_testList.SetItemData(currItemIndex, (DWORD_PTR)testInfo);
				
			}
			else
			{
				//MessageBox(_T("Coudn't find test function"), _T("Woops!"), MB_OK);
			}
		}
		FreeLibrary(hTestDll);
	}
	else
	{
		MessageBox(_T("Coudn't load DLL"), _T("Yikes!"), MB_OK);
	}

	// set to first item
	if (m_testList.GetCount() > 0)
	{
		m_testList.SetCurSel(0);
	}
}

void CTestFrameworkDlg::OnBnClickedButtonReset()
{
	m_testList.ResetContent();
}


void CTestFrameworkDlg::OnBnClickedButtonResetlog()
{
	m_logList.ResetContent();
}


void CTestFrameworkDlg::OnBnClickedButtonRuntests()
{
	if (m_testList.GetCount() > 0)
	{
		for (int i = 0; i < m_testList.GetCount(); i++)
		{
			TestInfo* testInfo = (TestInfo*) m_testList.GetItemData(i);
			if (testInfo)
			{
				CTestFrameworkDlg::m_testQueue.push(*testInfo);
			}
		}
	}
}

void CTestFrameworkDlg::PostLogMessage(CString *str)
{
	CWnd* pMainWnd = AfxGetApp()->GetMainWnd();
	if (pMainWnd)
	{
		pMainWnd->PostMessage(WM_USER_LOG_MESSAGE, NULL, (LPARAM) str);
	}
}

void CTestFrameworkDlg::ConsoleOut(char *str)
{
	m_mutex.Lock();
	_cprintf(str);
	m_mutex.Unlock();
}

void CTestFrameworkDlg::TestEngine(std::queue<TestInfo> &testQueue)
{
	TestInfo testInfo;
	char buf[MAX_BUFFER];

	// Send startup to test engine console)
	snprintf(buf, MAX_BUFFER, "Test Engine running on thread id: %d\n", GetCurrentThreadId());
	ConsoleOut(buf);

	// Send startup message to logger window
	PostLogMessage(new CString(buf));
	
	// Loop forever
	while (true)
	{
		// Check if there are any tests
		m_mutex.Lock();
		if (!testQueue.empty())
		{
			testInfo = testQueue.front();
			testQueue.pop();
			m_mutex.Unlock();
		}
		else
		{
			m_mutex.Unlock();
			Sleep(1000);
			continue;
		}
		
		// Declare string variables for holding DLL and Function Name
		std::string testDllName = testInfo.getFilePath();
		std::string testDllFuncName = testInfo.getTestName();
		std::string testDllFileName = testInfo.getFileName();

		// Get a handle to the test dll
		HINSTANCE hTestDll = LoadLibraryA(testDllName.c_str());
		if (hTestDll != NULL)
		{
			// Get the test function
			testFunc pTestFunc = (testFunc)GetProcAddress(hTestDll, testDllFuncName.c_str());
			if (NULL != pTestFunc)
			{
				snprintf(buf, MAX_BUFFER, "%d: %s %s started\n", GetCurrentThreadId(), testDllFileName.c_str(), testDllFuncName.c_str());
				ConsoleOut(buf);

				try
				{
					// Call the test function
					bool ret = pTestFunc();

					// Log message
					(ret) ? snprintf(buf, MAX_BUFFER, "%d: INFO %s %s test function returned %s", GetCurrentThreadId(), testDllFileName.c_str(), testDllFuncName.c_str(), "true (pass).") :
						snprintf(buf, MAX_BUFFER, "%d: INFO %s %s test function returned %s", GetCurrentThreadId(), testDllFileName.c_str(), testDllFuncName.c_str(), "false (failed).");
					PostLogMessage(new CString(buf));
				}
				catch (int code) 
				{
					// Log message
					snprintf(buf, MAX_BUFFER, "%d: ERROR %s %s %s 0x%X %s", GetCurrentThreadId(), testDllFileName.c_str(), testDllFuncName.c_str(), "threw exception", code, ", exception handled.");
					PostLogMessage(new CString(buf));
				}
				catch (...)
				{
					// Log message
					snprintf(buf, MAX_BUFFER, "%d: ERROR %s %s %s", GetCurrentThreadId(), testDllFileName.c_str(), testDllFuncName.c_str(), "test function threw a generic exception, exception handled.");
					PostLogMessage(new CString(buf));
				}

				// Log message
				snprintf(buf, MAX_BUFFER, "%d: %s %s ended\n", GetCurrentThreadId(), testDllFileName.c_str(), testDllFuncName.c_str());
				ConsoleOut(buf);
			}
			else
			{
				// Log message
				snprintf(buf, MAX_BUFFER, "%d: ERROR %s %s %s, %s.", GetCurrentThreadId(), testDllFileName.c_str(), testDllFuncName.c_str(), "couldn't find test function", testDllFuncName);
				PostLogMessage(new CString(buf));
			}

			//Free the shared object
			FreeLibrary(hTestDll);

		}
		else
		{
			// Build string to log
			snprintf(buf, MAX_BUFFER, "%d: ERROR %s %s %s, %s", GetCurrentThreadId(), testDllFileName.c_str(), testDllFuncName.c_str(), "coudn't find dll", testDllName);
			PostLogMessage(new CString(buf));
		}
	}
}

// Log message handler
afx_msg LRESULT CTestFrameworkDlg::OnLogMessage(WPARAM wParam, LPARAM lParam)
{
	CString* cslParam = (CString*)lParam;
	CString friendlyMonth[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };

	if (cslParam)
	{
		CTime t = CTime::GetCurrentTime();

		CString formattedString;
		formattedString.Format("%s %02d %02d:%02d:%02d %s", friendlyMonth[t.GetMonth() - 1], t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond(), *cslParam);
		m_logList.AddString(formattedString);
		delete cslParam;
	}

	return 0;
}

void CTestFrameworkDlg::OnBnClickedButtonExport()
{
	const TCHAR szFilter[] = _T("Log File (*.log)|*.log||");
	CFileDialog dlg(FALSE, _T(".log"), NULL, OFN_HIDEREADONLY, szFilter, this);

	if (dlg.DoModal() == IDOK)
	{
		CString filePath = dlg.GetPathName();
		CString fileName = dlg.GetFileName();

		if (!fileName.IsEmpty())
		{
			CStdioFile logFile;
			if (logFile.Open(filePath, CFile::modeCreate | CFile::modeWrite))
			{
				CString line;
				for (int i = 0; i < (int)m_logList.GetCount(); i++) {
					m_logList.GetText(i, line);
					logFile.WriteString(line);
				}
			}
			//MessageBox(filePath, fileName, MB_OK);
		}
	}
}

void CTestFrameworkDlg::OnFileOpentests()
{
	OnBnClickedButtonBrowse();
}


void CTestFrameworkDlg::OnFileQuit()
{
	EndDialog(IDOK);
}


void CTestFrameworkDlg::OnHelpAbout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


void CTestFrameworkDlg::OnEditResettestlist()
{
	OnBnClickedButtonReset();
}


void CTestFrameworkDlg::OnEditResetLog()
{
	OnBnClickedButtonResetlog();
}


void CTestFrameworkDlg::OnEditRunTests()
{
	OnBnClickedButtonRuntests();
}


void CTestFrameworkDlg::OnFileExportLog()
{
	OnBnClickedButtonExport();
}
