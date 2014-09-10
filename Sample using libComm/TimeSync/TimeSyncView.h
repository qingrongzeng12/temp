
// TimeSyncView.h : interface of the CTimeSyncView class
//

#pragma once

#include "resource.h"


class CTimeSyncView : public CFormView
{
protected: // create from serialization only
	CTimeSyncView();
	DECLARE_DYNCREATE(CTimeSyncView)

public:
	enum{ IDD = IDD_TIMESYNC_FORM };

// Attributes
public:
	CTimeSyncDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void OnInitialUpdate(); // called first time after construct

// Implementation
public:
	virtual ~CTimeSyncView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	HBRUSH m_hbruBkgnd;
// Generated message map functions
protected:
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	LRESULT	OnSocketEvent(WPARAM, LPARAM);
	afx_msg void OnClickedBtnUpdate();
	DECLARE_MESSAGE_MAP()

	void OnSocketRead(WPARAM);

private:
	CComboBox m_cmbServer;
	CxUdpSocket m_usToServer;
	//CString m_strServer;
};

#ifndef _DEBUG  // debug version in TimeSyncView.cpp
inline CTimeSyncDoc* CTimeSyncView::GetDocument() const
   { return reinterpret_cast<CTimeSyncDoc*>(m_pDocument); }
#endif

