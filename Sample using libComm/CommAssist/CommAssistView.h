
// CommAssistView.h : interface of the CCommAssistView class
//

#pragma once

#include "resource.h"


class CCommAssistView : public CFormView
{
protected: // create from serialization only
	CCommAssistView();
	DECLARE_DYNCREATE(CCommAssistView)

public:
	enum{ IDD = IDD_COMMASSIST_FORM };

// Attributes
public:
	CCommAssistDoc* GetDocument() const;

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
	virtual ~CCommAssistView();
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
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	LRESULT	OnCommEvent(WPARAM, LPARAM);
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClickedBtnOpenCommport();
	afx_msg void OnClickedBtnEmptyRecvEdit();
	afx_msg void OnClickedBtnEmptySendEdit();
	afx_msg void OnClickedBtnSelFile();
	afx_msg void OnClickedBtnSend();
	afx_msg void OnClickedBtnSendFile();
	afx_msg void OnClickedBtnSaveRecvEdit();
	afx_msg void OnClickedBtnEmptyCount();
	afx_msg void OnClickedChkAutoSend();
	DECLARE_MESSAGE_MAP()

	static LRESULT CALLBACK CommProc(HWND, UINT, WPARAM, LPARAM);
	LRESULT	OnCommRead(WPARAM, LPARAM);

	void OnCommPortOpenClosed(BOOL bClosed = TRUE);
	int HexToString(LPBYTE lpbtData, int nSize, LPTSTR lpszHex, int nMaxCount);
	int HexFromString(LPBYTE lpbtData, LPCTSTR lpszHex);

private:
	CxSerial m_xComm;
		
	CComboBox m_cmbPort;
	CComboBox m_cmbBaudRate;
	CComboBox m_cmbParity;
	CComboBox m_cmbByteSize;
	CComboBox m_cmbStopBits;
	CEdit m_edtCycle;
	CEdit m_editRecv;

	CString m_strFilePath;
	int m_nRecvCount, m_nSendCount;
};

#ifndef _DEBUG  // debug version in CommAssistView.cpp
inline CCommAssistDoc* CCommAssistView::GetDocument() const
   { return reinterpret_cast<CCommAssistDoc*>(m_pDocument); }
#endif

