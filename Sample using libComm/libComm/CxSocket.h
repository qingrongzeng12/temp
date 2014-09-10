#pragma once

#ifndef	_XSOCKET_H_
#define	_XSOCKET_H_


#ifdef LIBCOMM_EXPORTS
#define XSOCKET_API __declspec(dllexport)
#else
#define XSOCKET_API __declspec(dllimport)
#endif

#define XSOCKET_CLASS XSOCKET_API


#include <winsock2.h>


#include "CxObject.h"
#include "xCommon.h"




class XSOCKET_CLASS CxSocket : public CxComm
{
public:
	CxSocket();
	virtual ~CxSocket();

public:
	virtual HANDLE GetSafeHandle();

public:
	void Attach(SOCKET s) { m_socket = s; }
	SOCKET Detach();
	SOCKET GetSocket() { return m_socket; }
	BOOL SelectEvent(long lEvent);
	BOOL CloseSocket();

protected:
	BOOL ListenEvent();
	void DisListenEvent() { m_xThreadListen.CloseThread(); }
	BOOL IsListenningEvent() { return m_xThreadListen.IsThreadRunning(); }
	static DWORD WINAPI ListenEventProc(LPVOID lpParam);
	void OnListenEvent(CxThread* pxThdObj);

protected:
	SOCKET m_socket;
	CxThread m_xThreadListen;
};

class XSOCKET_CLASS CxUdpSocket : public CxSocket
{
public:
	CxUdpSocket();
	virtual ~CxUdpSocket();

public:
	virtual BOOL Send(LPBYTE lpbtData, DWORD dwSize, DWORD dwMillisec = INFINITE);
	virtual BOOL Recv(LPBYTE lpbtData, DWORD dwSize, DWORD dwMillisec = INFINITE);

public:
	BOOL Bind(int nPort, BOOL bListen = FALSE);
	BOOL Disbind();
	BOOL IsBinded();
	void GetPeerAddress(LPTSTR lpszAddr) { lstrcpy(lpszAddr, m_szPeerAddr); }
	void SetPeerAddress(LPCTSTR lpszAddr) { lstrcpy(m_szPeerAddr, lpszAddr); }

	BOOL SendTo(LPCTSTR lpszIPAddr, LPBYTE lpbtData, DWORD dwSize);
	BOOL RecvFrom(LPTSTR lpszIPAddr, LPBYTE lpbtData, DWORD dwSize);

protected:
	int m_nPort;
	TCHAR m_szPeerAddr[NET_HOST_MAX_IPV6];
};

class XSOCKET_CLASS CxClientSocket : public CxSocket
{
public:
	CxClientSocket();
	virtual ~CxClientSocket();

public:
	virtual BOOL Send(LPBYTE lpbtData, DWORD dwSize, DWORD dwMillisec = INFINITE);
	virtual BOOL Recv(LPBYTE lpbtData, DWORD dwSize, DWORD dwMillisec = INFINITE);

public:
	BOOL Connect(LPCTSTR lpszIPAddr, int nPort, BOOL bListen = FALSE);
	BOOL DisConnect();
	virtual BOOL IsConnected();
	virtual BOOL GetPeerAddress(LPTSTR lpszAddr);	// IP
	virtual BOOL GetSockAddress(LPTSTR lpszAddr);	// IP
};

class XSOCKET_CLASS CxServerSocket : public CxSocket
{
public:
	CxServerSocket();
	virtual ~CxServerSocket();

public:
	BOOL ListenConnection(LPCTSTR lpszIPAddr, int nPort);
	BOOL DisListenConnection();
	BOOL IsListenningConnection();
	SOCKET AcceptConnection();
};


class XSOCKET_CLASS CxBthClientSocket : public CxClientSocket
{
public:
	CxBthClientSocket();
	virtual ~CxBthClientSocket();
	
public:
	BOOL Connect(LPCTSTR lpszMacAddr, LPGUID lpguidService = NULL, ULONG nPort = 1, BOOL bListen = FALSE);

public:
	virtual BOOL IsConnected();
	virtual BOOL GetPeerAddress(LPTSTR lpszAddr);	// Mac
	virtual BOOL GetSockAddress(LPTSTR lpszAddr);	// Mac
};

class XSOCKET_CLASS CxBthServerSocket : public CxServerSocket
{
public:
	CxBthServerSocket();
	virtual ~CxBthServerSocket();
	
public:
	BOOL ListenConnection(LPCTSTR lpszMacAddr, LPGUID lpguidService = NULL, ULONG nPort = 1);
};



XSOCKET_API BOOL SocketInit();
XSOCKET_API BOOL SocketUnInit();
XSOCKET_API DWORD SocketGetLastError();


#endif // _XSOCKET_H_