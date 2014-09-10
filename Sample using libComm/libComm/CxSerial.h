#pragma once

#ifndef	_XSERIAL_H_
#define	_XSERIAL_H_


#ifdef LIBCOMM_EXPORTS
#define XSERIAL_API __declspec(dllexport)
#else
#define XSERIAL_API __declspec(dllimport)
#endif

#define XSERIAL_CLASS XSERIAL_API


#include "CxObject.h"




#define		EV_DEVICE_NOT_CONNECTED		(EV_EVENT2 + 1)

class XSERIAL_CLASS CxSerial : public CxComm
{
public:
	CxSerial();
	virtual ~CxSerial();
	
public:
	virtual HANDLE GetSafeHandle() { return m_hComm; }
	virtual BOOL Send(LPBYTE lpbtData, DWORD dwSize, DWORD dwMillisec = INFINITE);
	virtual BOOL Recv(LPBYTE lpbtData, DWORD dwSize, DWORD dwMillisec = INFINITE);
	
public:
	BOOL Open(LPCTSTR lpszDevPath, WORD wRThreshold = 1);
	BOOL Close();
	
	void SelectEvent(long lEvent) { m_dwEvent = lEvent; }
	void GetDevPath(LPTSTR lpszDevPath) { lstrcpy(lpszDevPath, m_szDevicePath); }
	
	void SetRThreshold(WORD btRThreshold) { m_wRThreshold = btRThreshold; }
	WORD GetRThreshold() { return m_wRThreshold; }
	
	BOOL GetSettings(LPDCB lpdcb);
	BOOL SetSettings(LPDCB lpdcb);
	
	BOOL GetBufferSize(LPDWORD lpdwInSize, LPDWORD lpdwOutSize);
	BOOL SetBufferSize(DWORD dwInSize, DWORD dwOutSize);
	BOOL GetBufferCount(LPDWORD lpdwInSize, LPDWORD lpdwOutSize);
	BOOL ClearBuffer(BOOL bInput);
	
protected:
	BOOL ListenEvent();
	void DisListenEvent();
	BOOL IsListenningEvent() { return m_xThreadListen.IsThreadRunning(); }
	static DWORD WINAPI ListenEventProc(LPVOID lpParam);
	void OnListenEvent(CxThread* pxThdObj);
	
private:
	TCHAR m_szDevicePath[MAX_PATH];
	HANDLE m_hComm;
	HANDLE m_hEventListen, m_hEventSend, m_hEventRecv;
	WORD m_wRThreshold;
	DWORD m_dwEvent;
	CxThread m_xThreadListen;
};



#endif // _XSERIAL_H_