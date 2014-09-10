#pragma once

#ifndef	_XDEVICE_H_
#define	_XDEVICE_H_


#ifdef LIBCOMM_EXPORTS
#define XDEVICE_API __declspec(dllexport)
#else
#define XDEVICE_API __declspec(dllimport)
#endif

#define XDEVICE_CLASS XDEVICE_API


#include <setupapi.h>




class XDEVICE_CLASS CxDeviceFind
{
public:
	CxDeviceFind();
	virtual ~CxDeviceFind();

public:
	virtual BOOL FindDevice(LPGUID lpguidDev);
	virtual BOOL FindNextDevice();
	void Close();

	void GetDevicePath(LPTSTR lpszPath) { lstrcpy(lpszPath, m_szPath); }
	void GetDeviceHardwareID(LPTSTR lpszHardID) { lstrcpy(lpszHardID, m_szHardID); }
	void GetDeviceFriendlyName(LPTSTR lpszFriendlyName) { lstrcpy(lpszFriendlyName, m_szFriendlyName); }
	void GetDeviceInstanceID(LPTSTR lpszInstanceID) { lstrcpy(lpszInstanceID, m_szInstanceID); }
	void GetDeviceLocationInfo(LPTSTR lpszLocationInfo) { lstrcpy(lpszLocationInfo, m_szLocationInfo); }
	void GetDeviceDescription(LPTSTR lpszDesc) { lstrcpy(lpszDesc, m_szDescription); }

	void GetDeviceGuid(LPGUID lpguidDev) { *lpguidDev = m_guidDev; }
	void GetClassGuid(LPGUID lpguidCls) { *lpguidCls = m_guidCls; }

protected:
	HDEVINFO m_hDevInfoSet;
	ULONG m_nCurSel;
	GUID m_guidDev, m_guidCls;

	TCHAR m_szInstanceID[MAX_PATH];
	TCHAR m_szPath[MAX_PATH];
	TCHAR m_szHardID[MAX_PATH];
	TCHAR m_szFriendlyName[MAX_PATH];
	TCHAR m_szLocationInfo[MAX_PATH];
	TCHAR m_szDescription[MAX_PATH];
};


class XDEVICE_CLASS CxDeviceMapFind  
{
public:
	CxDeviceMapFind();
	virtual ~CxDeviceMapFind();

public:
	virtual BOOL FindDeviceMap(LPGUID lpguidDev);
	virtual BOOL FindNextDeviceMap();
	void Close();
	
	void GetDevicePath(LPTSTR lpszPath) { lstrcpy(lpszPath, m_szPath); }

	
protected:
	HKEY m_hKeyDevMap;
	ULONG m_nCurSel;
	TCHAR m_szPath[MAX_PATH];
};


#endif // _XDEVICE_H_