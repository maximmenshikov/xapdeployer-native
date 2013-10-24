#pragma once
#ifndef PACMANCLIENT_H
#define PACMANCLIENT_H


typedef unsigned long long APPID;
class CApplicationInfo : public IUnknown
{
public:
	virtual APPID AppID() = NULL;
	virtual GUID *ProductID() = NULL;
	virtual GUID *InstanceID() = NULL;
	virtual GUID *OfferID() = NULL;
	virtual LPWSTR DefaultTask() = NULL;
	virtual LPWSTR AppTitle() = NULL;
	virtual LPWSTR AppIconPath() = NULL;
	virtual BOOL IsNotified() = NULL; 
	virtual int AppInstallType() = NULL; 
	virtual int State() = NULL;
	virtual BOOL IsRevoked() = NULL; 
	virtual BOOL IsUpdateAvailable() = NULL; 
	virtual FILETIME* LastTimeRun() = NULL; 
	virtual FILETIME* InstallDate() = NULL;
	virtual BOOL IsUninstallable() = NULL;
	virtual BOOL IsThemable() = NULL; 
	virtual LPWSTR InstallPath() = NULL; 
	virtual LPWSTR IsolatedStorageRoot() = NULL; 
	virtual unsigned short Rating() = NULL;
	virtual unsigned short Genre() = NULL; 
	virtual LPWSTR Publisher() = NULL; 
	virtual LPWSTR Author() = NULL; 
	virtual LPWSTR Description() = NULL; 
	virtual LPWSTR Version() = NULL; 
	virtual HRESULT GetInvocationInfo(LPWSTR* pszInvocationUrn, LPWSTR* pszInvocationParameters) = NULL;
	virtual LPWSTR ImagePath() = NULL; 

	/* for GetApplicationInfo2 functions */
	virtual char GetAppPlatMajorVersion() = NULL;
	virtual char GetAppPlatMinorVersion() = NULL;
};


class CDatabaseIterator : public IUnknown
{
public:
	virtual int GetNext(CApplicationInfo **) = NULL;
};


// System APIs
extern "C" 
{
	HRESULT	GetApplicationInfoByProductID(GUID, CApplicationInfo **);
	HRESULT	GetAllApplications(CDatabaseIterator **);
	HRESULT	GetAllVisibleApplications(CDatabaseIterator **);
	BOOL	SetEventData(HANDLE hEvent, DWORD dwData);
	DWORD	GetEventData(HANDLE hEvent); 
	HRESULT PMBeginInstall(void *package);
	HRESULT PMBeginUpdate(void *package);
	HRESULT PMBeginDeployPackage(void *package);
	HRESULT PMBeginUpdateDeployedPackage(void *package);
	HRESULT PMBeginUninstall(GUID guid);
	HRESULT PMRegisterForNotification(/*NotificationListener*/void *, DWORD dwNotificationType, DWORD dwUnknown);
	HRESULT PMUnregisterFromNotification(/*NotificationListener*/void *);
}

#endif
