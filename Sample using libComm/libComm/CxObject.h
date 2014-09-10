#pragma once

#ifndef	_XOBJECT_H_
#define _XOBJECT_H_


#ifdef LIBCOMM_EXPORTS
#define XOBJECT_API __declspec(dllexport)
#else
#define XOBJECT_API __declspec(dllimport)
#endif

#define XOBJECT_CLASS	XOBJECT_API





class XOBJECT_CLASS CxThread  
{
public:
	CxThread();
	virtual ~CxThread();

public:
	BOOL CreateThread(LPSECURITY_ATTRIBUTES lpThreadAttributes,
						DWORD dwStackSize, LPTHREAD_START_ROUTINE lpStartAddress,
						LPVOID lpParameter, DWORD dwCreationFlags,
						DWORD dwMillisec = IGNORE);
	BOOL IsThreadRunning();
	HANDLE GetSafeHandle() { return m_hThread; }

	void CloseThread(DWORD dwMillisec = INFINITE);
	BOOL ToExitThread() { return m_bExitThread; }
	LPVOID GetParamPtr() { return m_lpParameter; }

	void SetThreadStartedEvent();
	void SetWndMsg(HWND hWnd, UINT uMsg)
	{
		m_hWnd = hWnd;
		m_uMsg = uMsg;
	}

protected:
	static DWORD WINAPI ThreadStartRoutine(LPVOID lpParam);
	DWORD OnThreadStartRoutine();
		
	void PostQuitMessage();

private:
	BOOL m_bExitThread;
	HANDLE m_hThread, m_hEventThreadStarted;
	LPVOID m_lpParameter;
	DWORD m_dwThreadID;

	HWND m_hWnd;
	UINT m_uMsg;

	LPTHREAD_START_ROUTINE m_lpStartAddress;
};


typedef LRESULT (CALLBACK* COMMPROC)(LPVOID, WPARAM, LPARAM);

class XOBJECT_CLASS CxComm
{
public:
	CxComm();
	virtual ~CxComm();
	
public:
	virtual HANDLE GetSafeHandle() { return NULL; }
	virtual BOOL Send(LPBYTE lpbtData, DWORD dwSize, DWORD dwMillisec = INFINITE) { return FALSE; }
	virtual BOOL Recv(LPBYTE lpbtData, DWORD dwSize, DWORD dwMillisec = INFINITE) { return FALSE; }
	
public:
	void SetWndMsgProc(HWND hWnd, UINT uMsg, COMMPROC commproc = NULL, LPVOID lpParam = NULL)
	{
		m_hWnd = hWnd;
		m_uMsg = uMsg;
		m_commproc = commproc;
		m_lpParam = lpParam;
	}
	
protected:
	COMMPROC m_commproc;
	LPVOID m_lpParam;
	HWND m_hWnd;
	UINT m_uMsg;
};




#endif // _XOBJECT_H_