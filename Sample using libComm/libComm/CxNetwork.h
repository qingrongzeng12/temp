#pragma once

#ifndef	_XNETWORK_H_
#define	_XNETWORK_H_


#ifdef LIBCOMM_EXPORTS
#define XNET_API __declspec(dllexport)
#else
#define XNET_API __declspec(dllimport)
#endif

#define XNET_CLASS XNET_API



#include <winsock2.h>

//#include <basetsd.h>
//#include <iphlpapi.h>

#include "xCommon.h"



typedef struct tagNET_HOST_INFO
{
	TCHAR szMacAddr[NET_HOST_MAX_MAC];
	TCHAR szName[NET_HOST_MAX_NAME];
	TCHAR szIPAddr[NET_HOST_MAX_IPV6];
}
NET_HOST_INFO, *PNET_HOST_INFO;


class XNET_CLASS CxLocalHostIPAddrFind
{
public:
	CxLocalHostIPAddrFind();
	virtual ~CxLocalHostIPAddrFind();
	
public:
	BOOL FindIPAddr();
	BOOL FindNextIPAddr();
	void FindIPAddrClose();
	
	void GetIPAddr(LPTSTR lpszIPAddr) { lstrcpy(lpszIPAddr, m_szIPAddr); }
	
private:
	hostent* m_pHost;
	int m_nIndex;
	TCHAR m_szIPAddr[NET_HOST_MAX_IPV6];
};


/*
class XNET_CLASS CxAdapterFind
{
public:
	CxAdapterFind();
	virtual ~CxAdapterFind();
	
public:
	BOOL FindAdapter();
	BOOL FindNextAdapter();
	void FindAdapterClose();
	
	void GetAdapterInfo(PIP_ADAPTER_INFO pAdapterInfo) { *pAdapterInfo = m_aiCur; }
	
private:
	IP_ADAPTER_INFO m_aiHead, m_aiCur;
};


class XNET_CLASS CxNetworkHostFind  
{
public:
	CxNetworkHostFind();
	virtual ~CxNetworkHostFind();
	
public:
	BOOL FindHost();
	BOOL FindNextHost();
	void FindHostClose();
	
	void GetHostInfo(PNET_HOST_INFO pNetHostInfo)  { *pNetHostInfo = m_nhi; }
	
protected:
	typedef struct tagFIND_HOST_PROC_DATA
	{
		LPVOID lpThis;
		DWORD dwData;
	}
	FIND_HOST_PROC_DATA, *PFIND_HOST_PROC_DATA;
	
	static DWORD WINAPI FindHostProc(LPVOID lpParam);
	void OnFindHostProc(DWORD dwData);
	
protected:
	BYTE m_btImpNO;
	sockaddr_in m_saLocalHost;
	
	HANDLE m_hThreads[MAXIMUM_WAIT_OBJECTS];
	FIND_HOST_PROC_DATA m_fhpd[MAXIMUM_WAIT_OBJECTS];
	BYTE m_btOnline[MAXBYTE];
	
	NET_HOST_INFO m_nhi;
};
*/



XNET_API void StringToMacAddress(LPCTSTR lpszMac, PULONGLONG pullAddr);
XNET_API void MacAddressToString(const PULONGLONG pullAddr, LPTSTR lpszMac, DWORD dwSize);
XNET_API BOOL GetFirstValidIPAddress(LPTSTR lpszIPAddr);
XNET_API BOOL IsValidIPAddress(LPCTSTR lpszIPAddr);
XNET_API BOOL GetIPAddrByDomainName(LPCTSTR lpszDomainName, LPTSTR lpszIPAddr);


#endif // _XNET_H_
