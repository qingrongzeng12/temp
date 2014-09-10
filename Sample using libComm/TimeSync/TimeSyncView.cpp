
// TimeSyncView.cpp : implementation of the CTimeSyncView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "TimeSync.h"
#endif

#include "TimeSyncDoc.h"
#include "TimeSyncView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


#define		WM_SOCKET_EVENT				(WM_USER + 3)


// CTimeSyncView

IMPLEMENT_DYNCREATE(CTimeSyncView, CFormView)

BEGIN_MESSAGE_MAP(CTimeSyncView, CFormView)
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_SOCKET_EVENT, &CTimeSyncView::OnSocketEvent)
	ON_BN_CLICKED(IDC_BTN_UPDATE, &CTimeSyncView::OnClickedBtnUpdate)
END_MESSAGE_MAP()

// CTimeSyncView construction/destruction

CTimeSyncView::CTimeSyncView()
	: CFormView(CTimeSyncView::IDD)
{
	// TODO: add construction code here

}

CTimeSyncView::~CTimeSyncView()
{
}

void CTimeSyncView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_cmbServer);
}

BOOL CTimeSyncView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.style &= ~WS_BORDER;
	return CFormView::PreCreateWindow(cs);
}

void CTimeSyncView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();

	GetParentFrame()->CenterWindow();

	m_cmbServer.SetCurSel(0);
	m_usToServer.SetWndMsgProc(GetSafeHwnd(), WM_SOCKET_EVENT, NULL, 0);
}

// CTimeSyncView diagnostics

#ifdef _DEBUG
void CTimeSyncView::AssertValid() const
{
	CFormView::AssertValid();
}

void CTimeSyncView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}

CTimeSyncDoc* CTimeSyncView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTimeSyncDoc)));
	return (CTimeSyncDoc*)m_pDocument;
}
#endif //_DEBUG


// CTimeSyncView message handlers


HBRUSH CTimeSyncView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
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


int CTimeSyncView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	m_hbruBkgnd = ::CreateSolidBrush(RGB(164, 195, 235));
	return 0;
}


void CTimeSyncView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: Add your message handler code here
	::DeleteObject(m_hbruBkgnd);
	m_usToServer.Disbind();
}


void CTimeSyncView::OnClickedBtnUpdate()
{
	// TODO: Add your control notification handler code here
	CString str;
	m_cmbServer.GetWindowText(str);

	TCHAR szIPAddr[NET_HOST_MAX_IPV6];
	BOOL bRet;

	bRet = GetIPAddrByDomainName(str, szIPAddr);
	if (bRet)
	{
		bRet = m_usToServer.Bind(37);
		BYTE btDateTime[4] = {0};
		bRet = m_usToServer.SendTo(szIPAddr, btDateTime, sizeof(btDateTime));
	}
}


LRESULT	CTimeSyncView::OnSocketEvent(WPARAM wParam, LPARAM lParam)
{
	if (WSAGETSELECTERROR(lParam) != 0)
		return 1;
	
	int nEvent = WSAGETSELECTEVENT(lParam);
	switch (nEvent)
	{
	case FD_READ: OnSocketRead(wParam); break;
	default: break;
	}
	
	return 1;
}


void CTimeSyncView::OnSocketRead(WPARAM wParam)
{
	if (m_usToServer.GetSocket() == (SOCKET)wParam)
	{
		BYTE btDateTime[4] = {0};
		TCHAR szIPAddr[NET_HOST_MAX_IPV6];
		m_usToServer.RecvFrom(szIPAddr, btDateTime, sizeof(btDateTime));
		m_usToServer.Disbind();

		DWORD dwDateTime = 0;
		dwDateTime += btDateTime[0] << 24;
		dwDateTime += btDateTime[1] << 16;
		dwDateTime += btDateTime[2] << 8;
		dwDateTime += btDateTime[3];
		
		COleDateTime odtGMT(1900, 1, 1, 0, 0, 0);	// 1900.1.1 00:00:00 
		COleDateTime odtWGMT(1970, 1, 1, 0, 0, 0);	// 1970.1.1 00:00:00 
		COleDateTimeSpan odtsDelt = odtWGMT - odtGMT; 
		DWORD dwSpan = (DWORD)odtsDelt.GetTotalSeconds(); 
		ASSERT(dwSpan == 2208988800L); 
		
		dwDateTime -= dwSpan;		
		CTime tmNetTime = (CTime)dwDateTime;
		SYSTEMTIME st = {0};
		tmNetTime.GetAsSystemTime(st);
		::SetLocalTime(&st);

		CString str;
		m_cmbServer.GetWindowText(str);
		CString strOutput;
		strOutput.Format(_T("与 %s 同步时间成功,在 %04d-%02d-%02d 在 %02d:%02d:%02d."), str,
							st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
		SetDlgItemText(IDC_STA_OUTPUT, strOutput);
	}
}