#pragma once

#ifndef	_XBTH_H_
#define	_XBTH_H_


#ifdef LIBCOMM_EXPORTS
#define XBTH_API __declspec(dllexport)
#else
#define XBTH_API __declspec(dllimport)
#endif

#define XBTH_CLASS XBTH_API



#include "xCommon.h"

#include <objbase.h>
#include <initguid.h>		// DEFINE_GUID
#include <SpecStrings.h>	// __checkReturn, __in. belong to WinDDK
#include <BluetoothAPIs.h>	// BTH_MAX_XXX. belong to WinDDK



typedef struct tagBTH_DEV_INFO
{
	TCHAR szMacAddr[BLUETOOTH_MAX_MAC_SIZE];
	TCHAR szName[BLUETOOTH_MAX_NAME_SIZE];
	ULONG ulClassofDevice;
}
BTH_DEV_INFO, *PBTH_DEV_INFO;


class XBTH_CLASS CxBthRemoteDeviceFind  
{
public:
	CxBthRemoteDeviceFind();
	virtual ~CxBthRemoteDeviceFind();
	
public:
	BOOL FindDevice();
	BOOL FindNextDevice();
	void Close();
	
	void GetDeviceInfo(PBTH_DEV_INFO pBthDevInfo) { *pBthDevInfo = m_bdi; }
	
protected:
	HANDLE m_hLookup;
	BTH_DEV_INFO m_bdi;
};


class XBTH_CLASS CxBthRadioFind  
{
public:
	CxBthRadioFind();
	virtual ~CxBthRadioFind();
	
public:
	BOOL FindRadio(BOOL bCloseHandle = TRUE);
	BOOL FindNextRadio(BOOL bCloseHandle = TRUE);
	void FindRadioClose();
	void CloseRadioHandle();
	
	HANDLE GetRadioHandle() { return m_hRadio; }
	void GetDeviceInfo(PBTH_DEV_INFO pBthDevInfo) { *pBthDevInfo = m_bdi; }

protected:
	HBLUETOOTH_RADIO_FIND m_hFind;
	HANDLE m_hRadio;
	BTH_DEV_INFO m_bdi;
};


class XBTH_CLASS CxBthRadio  
{
public:
	CxBthRadio();
	virtual ~CxBthRadio();
	
	void Attach(HANDLE hRadio) { m_hRadio = hRadio; }
	
public:
	BOOL Open(LPCTSTR lpszMacAddr);
	void Close();
	HANDLE GetSafeHandle() { return m_hRadio; }
	BOOL EnableDiscovery(BOOL bEnable);
	BOOL EnableIncomingConnections(BOOL bEnable);
	DWORD EnumerateInstalledServices(PBLUETOOTH_DEVICE_INFO pbtdi, DWORD *pcServices,
										GUID *pGuidServices);
	DWORD GetRadioInfo(PBLUETOOTH_RADIO_INFO pRadioInfo);

	BOOL FindRemoteDevice(BOOL bFlushCache = TRUE);
	BOOL FindNextRemoteDevice();
	void FindRemoteDeviceClose();
	
	void GetRemoteDeviceInfo(PBTH_DEV_INFO pBthDevInfo) { *pBthDevInfo = m_bdiRemote; }
	void GetRemoteDeviceInfoStruct(PBLUETOOTH_DEVICE_INFO pBthDevInfo) { *pBthDevInfo = m_bdis; }
	
	BOOL IsRemoteDeviceAuthenticated() { return m_bdis.fAuthenticated; }
	BOOL IsRemoteDeviceInstallSPPService();
	BOOL InstallRemoteDeviceSPPService();
	
	DWORD AuthenticateRemoteDevice(LPCTSTR lpszPasskey, ULONG uCch);

	DWORD AuthenticateRemoteDevice(LPCTSTR lpszMacAddr, LPCTSTR lpszPasskey, ULONG uCch);
	static DWORD RemoveRemoteDevice(LPCTSTR lpszMacAddr);

private:
	void FlushCacheRemoteDeviceInfo();
	
private:
	HANDLE m_hRadio;
	
	HANDLE m_hRemoteDeviceFind;
	BTH_DEV_INFO m_bdiRemote;
	BLUETOOTH_DEVICE_INFO m_bdis;
};


#define	HBTHAR	HBLUETOOTH_AUTHENTICATION_REGISTRATION

class XBTH_CLASS CxBthAuthenResponse
{
public:
	CxBthAuthenResponse();
	virtual ~CxBthAuthenResponse();
	
public:
	void SetDeviceInfoStruct(PBLUETOOTH_DEVICE_INFO pBdi) { m_bdis = *pBdi; }
	void GetDeviceInfoStruct(PBLUETOOTH_DEVICE_INFO pBdi) { *pBdi = m_bdis; }
	
	DWORD RegisterForAuthentication(HANDLE hRadio, LPCTSTR lpszPasskey);
	BOOL UnregisterAuthentication();
	
protected:
	static BOOL /*CALLBACK*/ AuthenticateProc(LPVOID pvParam, PBLUETOOTH_DEVICE_INFO pDevice);
	BOOL OnAuthenticate(PBLUETOOTH_DEVICE_INFO pDevice);
	
private:
	HBTHAR m_hBthAr;
	HANDLE m_hRadio;
	TCHAR m_szPasskey[BLUETOOTH_MAX_PASSKEY_SIZE];
	BLUETOOTH_DEVICE_INFO m_bdis;
};




XBTH_API BOOL BthFindDevice(LPCTSTR lpszRemoteDevName, LPTSTR lpszRadioMacAddr, LPTSTR lpszRemoteMacAddr,
							BYTE btDevNearIndex, BOOL bFlushCache, LPBYTE lpbtBthError);
XBTH_API BOOL BthAuthenDevice(LPCTSTR lpszRadioMacAddr, LPCTSTR lpszRemoteMacAddr, LPCTSTR lpszPasskey);


#endif //_XBTH_H_