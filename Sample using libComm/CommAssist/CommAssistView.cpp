
// CommAssistView.cpp : implementation of the CCommAssistView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "CommAssist.h"
#endif

#include "CommAssistDoc.h"
#include "CommAssistView.h"
#include "MainFrm.h"


#include <WinIoCtl.h>
#include <strsafe.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define		WM_SERIAL_EVENT				(WM_USER + 2)

#define		TIMER_ID_AUTO_SEND			1

#define		DEF_COMM_OUT_QUEUE			4096


// CCommAssistView

IMPLEMENT_DYNCREATE(CCommAssistView, CFormView)

BEGIN_MESSAGE_MAP(CCommAssistView, CFormView)
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_TIMER()
	ON_MESSAGE(WM_SERIAL_EVENT, &CCommAssistView::OnCommEvent)
	ON_BN_CLICKED(IDC_BTN_EMPTY_RECV_EDIT, &CCommAssistView::OnClickedBtnEmptyRecvEdit)
	ON_BN_CLICKED(IDC_BTN_EMPTY_SEND_EDIT, &CCommAssistView::OnClickedBtnEmptySendEdit)
	ON_BN_CLICKED(IDC_BTN_SEL_FILE, &CCommAssistView::OnClickedBtnSelFile)
	ON_BN_CLICKED(IDC_BTN_SEND, &CCommAssistView::OnClickedBtnSend)
	ON_BN_CLICKED(IDC_BTN_SEND_FILE, &CCommAssistView::OnClickedBtnSendFile)
	ON_BN_CLICKED(IDC_BTN_SAVE_RECV_EDIT, &CCommAssistView::OnClickedBtnSaveRecvEdit)
	ON_BN_CLICKED(IDC_CHK_AUTO_SEND, &CCommAssistView::OnClickedChkAutoSend)
	ON_BN_CLICKED(IDCANCEL, &CCommAssistView::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BTN_OPEN_COMMPORT, &CCommAssistView::OnClickedBtnOpenCommport)
	ON_BN_CLICKED(IDC_BTN_EMPTY_COUNT, &CCommAssistView::OnClickedBtnEmptyCount)
END_MESSAGE_MAP()

// CCommAssistView construction/destruction

CCommAssistView::CCommAssistView()
	: CFormView(CCommAssistView::IDD)
{
	// TODO: add construction code here

}

CCommAssistView::~CCommAssistView()
{
}

void CCommAssistView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cmbPort);
	DDX_Control(pDX, IDC_COMBO2, m_cmbBaudRate);
	DDX_Control(pDX, IDC_COMBO3, m_cmbParity);
	DDX_Control(pDX, IDC_COMBO4, m_cmbByteSize);
	DDX_Control(pDX, IDC_COMBO5, m_cmbStopBits);
	DDX_Control(pDX, IDC_EDIT_CYCLE, m_edtCycle);
	DDX_Control(pDX, IDC_EDIT_RECV, m_editRecv);
}

BOOL CCommAssistView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style &= ~WS_BORDER;
	return CFormView::PreCreateWindow(cs);
}

void CCommAssistView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();
		
	GetParentFrame()->CenterWindow();

	TCHAR szPath[MAX_PATH];
	GUID guid = GUID_DEVINTERFACE_COMPORT;
	CxDeviceMapFind dev_map_find;
	BOOL bRet = dev_map_find.FindDeviceMap(&guid);
	while (bRet)
	{			
		dev_map_find.GetDevicePath(szPath/*, MAX_PATH*/);
		m_cmbPort.AddString(szPath);

		bRet = dev_map_find.FindNextDeviceMap();
	}
	dev_map_find.Close();
	m_cmbPort.SetCurSel(0);

	CString str;
	int nBund[12] = {1200, 1800, 2400, 4800, 7200, 9600,
						14400, 19200, 38400, 57600, 115200, 128000};
	int i, nCount = sizeof(nBund) / sizeof(nBund[0]);
	for (i=0; i<nCount; i++)
	{
		str.Format(_T("%d"), nBund[i]);
		m_cmbBaudRate.AddString(str);
	}
	m_cmbBaudRate.SetCurSel(5);

	m_cmbParity.AddString(_T("无"));
	m_cmbParity.AddString(_T("奇"));
	m_cmbParity.AddString(_T("偶"));
	m_cmbParity.AddString(_T("标志"));
	m_cmbParity.AddString(_T("空格"));
	m_cmbParity.SetItemData(0, NOPARITY);
	m_cmbParity.SetItemData(1, ODDPARITY);
	m_cmbParity.SetItemData(2, EVENPARITY);
	m_cmbParity.SetItemData(3, MARKPARITY);
	m_cmbParity.SetItemData(4, SPACEPARITY);
	m_cmbParity.SetCurSel(0);

	m_cmbByteSize.AddString(_T("4"));
	m_cmbByteSize.AddString(_T("5"));
	m_cmbByteSize.AddString(_T("6"));
	m_cmbByteSize.AddString(_T("7"));
	m_cmbByteSize.AddString(_T("8"));
	m_cmbByteSize.SetCurSel(4);

	m_cmbStopBits.AddString(_T("1"));
	m_cmbStopBits.AddString(_T("1.5"));
	m_cmbStopBits.AddString(_T("2"));
	m_cmbStopBits.SetItemData(0, ONESTOPBIT);
	m_cmbStopBits.SetItemData(1, ONE5STOPBITS);
	m_cmbStopBits.SetItemData(2, TWOSTOPBITS);
	m_cmbStopBits.SetCurSel(0);

	m_edtCycle.SetLimitText(6);

	OnClickedBtnEmptyCount();
}

// CCommAssistView diagnostics

#ifdef _DEBUG
void CCommAssistView::AssertValid() const
{
	CFormView::AssertValid();
}

void CCommAssistView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CCommAssistDoc* CCommAssistView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CCommAssistDoc)));
	return (CCommAssistDoc*)m_pDocument;
}
#endif //_DEBUG


// CCommAssistView message handlers


HBRUSH CCommAssistView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  Change any attributes of the DC here

	// TODO:  Return a different brush if the default is not desired
	BOOL bOutputWnd = FALSE;
	if (nCtlColor == CTLCOLOR_DLG ||
		nCtlColor == CTLCOLOR_STATIC)
		bOutputWnd = TRUE;
	if (nCtlColor == CTLCOLOR_STATIC)
		pDC->SetBkMode(TRANSPARENT);

	return  bOutputWnd ? m_hbruBkgnd : hbr;
}


int CCommAssistView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_hbruBkgnd = ::CreateSolidBrush(RGB(164, 195, 235));
	return 0;
}


void CCommAssistView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: Add your message handler code here
	::DeleteObject(m_hbruBkgnd);
	KillTimer(TIMER_ID_AUTO_SEND);
	m_xComm.Close();
}


void CCommAssistView::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	m_xComm.Close();

	GetParentFrame()->SendMessage(WM_CLOSE);
}


void CCommAssistView::OnClickedBtnOpenCommport()
{
	// TODO: Add your control notification handler code here
	if (m_xComm.GetSafeHandle() != NULL)
	{
		BOOL bRet = m_xComm.Close();
		OnCommPortOpenClosed(bRet);
		return;
	}

	int nRet;
	CString str, strPort;
	DCB dcbCustom = {0};

	m_cmbPort.GetWindowText(strPort);
	m_cmbBaudRate.GetWindowText(str);
	dcbCustom.BaudRate = _ttoi(str);

	nRet = m_cmbParity.GetCurSel();
	if (nRet == -1)
		nRet = 0;
	dcbCustom.Parity = (BYTE)m_cmbParity.GetItemData(nRet);

	m_cmbByteSize.GetWindowText(str);
	dcbCustom.ByteSize = _ttoi(str);

	nRet = m_cmbStopBits.GetCurSel();
	if (nRet == -1)
		nRet = 0;
	dcbCustom.StopBits = (BYTE)m_cmbStopBits.GetItemData(nRet);

	m_xComm.SetWndMsgProc(GetSafeHwnd(), WM_SERIAL_EVENT, NULL, 0);
	BOOL bRet = m_xComm.Open(strPort);
	if (bRet)
	{
		DCB dcb = {0};
		m_xComm.GetSettings(&dcb);
		dcb.BaudRate = dcbCustom.BaudRate;
		dcb.ByteSize = dcbCustom.ByteSize;
		dcb.Parity   = dcbCustom.Parity;
		dcb.StopBits = dcbCustom.StopBits;
		m_xComm.SetSettings(&dcb);
//		m_xComm.SetBufferSize(DEF_COMM_IN_QUEUE, DEF_COMM_OUT_QUEUE);
//		m_xComm.ClearBuffer(TRUE);
//		m_xComm.ClearBuffer(FALSE);
//		m_xComm.SetRThreshold(1);
		

		
	}
	else
		str.Format(_T("打开 %s 失败."), strPort);

	CMainFrame* pFrame = (CMainFrame*)GetParentFrame();
	pFrame->SetStatusText(0, str);

	OnCommPortOpenClosed(!bRet);
}


void CCommAssistView::OnCommPortOpenClosed(BOOL bClosed)
{
	SetDlgItemText(IDC_BTN_OPEN_COMMPORT, bClosed ? _T("打开串口") : _T("关闭串口" ));
	m_cmbPort.EnableWindow(bClosed);
	m_cmbBaudRate.EnableWindow(bClosed);
	m_cmbParity.EnableWindow(bClosed);
	m_cmbByteSize.EnableWindow(bClosed);
	m_cmbStopBits.EnableWindow(bClosed);

	OnClickedBtnEmptyCount();
	m_strFilePath.Empty();
}


LRESULT	CCommAssistView::OnCommEvent(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = 0;
	switch (lParam)
	{
	case EV_RXCHAR:
		lRet = OnCommRead(wParam, lParam);
		break;
	default: break;
	}
	
	return lRet;
}


LRESULT CALLBACK CCommAssistView::CommProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = 0;
	CCommAssistView* pThis = (CCommAssistView*)hwnd;
	if (pThis != NULL)
		lRet = pThis->OnCommEvent(wParam, lParam);

	return lRet;
}


LRESULT	CCommAssistView::OnCommRead(WPARAM wParam, LPARAM lParam)
{
	LRESULT lRet = 0;
	CString str, strText;

	if ((HANDLE)wParam == m_xComm.GetSafeHandle())
	{
		DWORD dwInQue, dwOutQue;
		while (m_xComm.GetBufferCount(&dwInQue, &dwOutQue))
		{
			if (dwInQue == 0)
				break;

			LPBYTE lpbtData = (LPBYTE)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwInQue);
			m_xComm.Recv(lpbtData, dwInQue);
			if (::IsDlgButtonChecked(GetSafeHwnd(), IDC_CHK_RECV_HEX))
			{
				int nMaxCount = HexToString(lpbtData, dwInQue, NULL, 0);
				LPTSTR lpszData = (LPTSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nMaxCount*sizeof(TCHAR));
				HexToString(lpbtData, dwInQue, lpszData, nMaxCount);
				str = lpszData;
				::HeapFree(GetProcessHeap(), 0, lpszData);
			}
			else
			{
				int nMaxSize = dwInQue;
				LPTSTR lpszData = (LPTSTR)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nMaxSize+sizeof(TCHAR));
				memcpy(lpszData, lpbtData, dwInQue);
				str = lpszData;
				::HeapFree(GetProcessHeap(), 0, lpszData);
			}

			m_editRecv.GetWindowText(strText);
			if (strText.GetLength() + str.GetLength() > 30000)
				m_editRecv.SetWindowText(NULL);
			m_editRecv.SetSel(-1, -1);
			m_editRecv.ReplaceSel(str);
			::HeapFree(GetProcessHeap(), 0, lpbtData);

			CMainFrame* pFrame = (CMainFrame*)GetParentFrame();
			m_nRecvCount += dwInQue;
			str.Format(_T("Recv %d"), m_nRecvCount);
			pFrame->SetStatusText(1, str);
		}
			
		lRet = 1;
	}

	return lRet;
}

void CCommAssistView::OnClickedBtnEmptyRecvEdit()
{
	// TODO: Add your control notification handler code here
	SetDlgItemText(IDC_EDIT_RECV, NULL);
}


void CCommAssistView::OnClickedBtnEmptySendEdit()
{
	// TODO: Add your control notification handler code here
	SetDlgItemText(IDC_EDIT_SEND, NULL);
}


void CCommAssistView::OnClickedBtnSelFile()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE);
	int nRet = dlg.DoModal();

	if (nRet == IDOK)
	{
		m_strFilePath = dlg.GetPathName();
		CMainFrame* pFrame = (CMainFrame*)GetParentFrame();
		pFrame->SetStatusText(0, m_strFilePath);
	}
}


void CCommAssistView::OnClickedBtnSend()
{
	// TODO: Add your control notification handler code here
	CString str;
	GetDlgItemText(IDC_EDIT_SEND, str);
	if (str.GetLength() == 0)
		return;

	LPBYTE lpbtData;
	int nMaxCount;

	if (::IsDlgButtonChecked(GetSafeHwnd(), IDC_CHK_SEND_HEX))
	{
		nMaxCount = HexFromString(NULL, str);
		lpbtData = (LPBYTE)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nMaxCount);
		HexFromString(lpbtData, str);
	}
	else
	{
		nMaxCount = str.GetLength() * sizeof(TCHAR);
		lpbtData = (LPBYTE)::HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, nMaxCount);
		memcpy(lpbtData, str.GetBuffer(), nMaxCount);
		str.ReleaseBuffer();
	}

	BOOL bRet = m_xComm.Send(lpbtData, nMaxCount);
	::HeapFree(GetProcessHeap(), 0, lpbtData);

	if (bRet)
	{
		CMainFrame* pFrame = (CMainFrame*)GetParentFrame();
		m_nSendCount += nMaxCount;
		str.Format(_T("Send %d"), m_nSendCount);
		pFrame->SetStatusText(2, str);
	}
	else
		AfxMessageBox(_T("发送数据失败.请检查串口是否已打开."));
}


void CCommAssistView::OnClickedBtnSendFile()
{
	// TODO: Add your control notification handler code here
	if (m_strFilePath.IsEmpty())
		return;

	BOOL bRet = FALSE;
	CFile f;
	if (f.Open(m_strFilePath, CFile::modeRead|CFile::typeBinary))
	{
		BYTE btData[DEF_COMM_OUT_QUEUE] = {0};
		UINT uRead = f.Read(btData, DEF_COMM_OUT_QUEUE);
		while (uRead != 0)
		{
			bRet = m_xComm.Send(btData, uRead);
			if (bRet)
				m_nSendCount += uRead;
			else
				break;

			ZeroMemory(btData, sizeof(btData));
			uRead = f.Read(btData, DEF_COMM_OUT_QUEUE);
		}

		f.Close();
	}

	CMainFrame* pFrame = (CMainFrame*)GetParentFrame();
	CString str;
	str.Format(_T("Send %d"), m_nSendCount);
	pFrame->SetStatusText(2, str);

	if (bRet)
	{
		m_strFilePath.Empty();
		str.LoadString(AFX_IDS_IDLEMESSAGE);
		pFrame->SetStatusText(0, str);
	}
	else
		AfxMessageBox(_T("发送数据失败.请检查串口是否已打开."));
}


void CCommAssistView::OnClickedBtnSaveRecvEdit()
{
	// TODO: Add your control notification handler code here
	LPCTSTR  szFilter = _T("文本文档(*.txt)|*.txt|所有文件|*.*||");
	CFileDialog dlg(FALSE, _T("txt"), _T("*.txt"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	int nRet = dlg.DoModal();

	if (nRet == IDOK)
	{
		CFile f;
		if (f.Open(dlg.GetPathName(), CFile::modeWrite|CFile::modeCreate|CFile::typeBinary))
		{
			CString str;
			GetDlgItemText(IDC_EDIT_RECV, str);
			LPTSTR lpszText = str.GetBuffer();
			int nSize = str.GetLength() * sizeof(TCHAR);
			f.Write(lpszText, nSize);
			f.Close();
		}
	}
}

int CCommAssistView::HexToString(LPBYTE lpbtData, int nSize, LPTSTR lpszHex, int nMaxCount)
{
	if (lpszHex != NULL)
		lpszHex[0] = 0;

	const int nMaxHexStr = 4;
	int nStrSize = 1;
	TCHAR szHex[nMaxHexStr] = {0};
	for (int i=0; i<nSize; i++)
	{
		StringCchPrintf(szHex, nMaxHexStr, _T("%02X "), lpbtData[i]);
		nStrSize += lstrlen(szHex);

		if (lpszHex != NULL)
			StringCchCat(lpszHex, nMaxCount, szHex);
	}
	
	if (lpszHex != NULL)
	{
		nSize = lstrlen(lpszHex);
		lpszHex[nSize] = 0;
	}

	return nStrSize;
}

int CCommAssistView::HexFromString(LPBYTE lpbtData, LPCTSTR lpszHex)
{
	const int nMaxHexStr = 3;
	int nData;
	TCHAR szData[nMaxHexStr] = {0};
	int i, j, nLen = lstrlen(lpszHex);
	
	for (i=0, j=0; i<nLen; i+=nMaxHexStr,j++)
	{
		StringCchCopyN(szData, nMaxHexStr, &lpszHex[i], nMaxHexStr);
		_stscanf_s(szData, _T("%02X"), &nData);
		
		if (lpbtData != NULL)
			lpbtData[j] = (BYTE)nData;
	}
	
	return j;
}


void CCommAssistView::OnClickedChkAutoSend()
{
	// TODO: Add your control notification handler code here
	if (::IsDlgButtonChecked(GetSafeHwnd(), IDC_CHK_AUTO_SEND))
	{
		int nElapse = GetDlgItemInt(IDC_EDIT_CYCLE);
		if (nElapse > 0)
			SetTimer(TIMER_ID_AUTO_SEND, nElapse, NULL);	
	}
	else
		KillTimer(TIMER_ID_AUTO_SEND);
}


void CCommAssistView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == TIMER_ID_AUTO_SEND)
	{
		OnClickedBtnSend();
	}

	CFormView::OnTimer(nIDEvent);
}


void CCommAssistView::OnClickedBtnEmptyCount()
{
	// TODO: Add your control notification handler code here
	CString str;
	CMainFrame* pFrame = (CMainFrame*)GetParentFrame();
	m_nRecvCount = m_nSendCount = 0;
	str.Format(_T("Recv %d"), m_nRecvCount);
	pFrame->SetStatusText(1, str);
	str.Format(_T("Send %d"), m_nSendCount);
	pFrame->SetStatusText(2, str);
}
