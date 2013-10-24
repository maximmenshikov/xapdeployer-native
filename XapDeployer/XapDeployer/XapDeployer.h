/*
	Header: XapDeployer.h
	Project: XapDeployer
	Author: ultrashot
	Comment: Public definition for Xap Deployer code. 
	
	Please, note that not a single part of Xap Deployer 
	code or its static library should be published without author's permission
*/
#pragma once
#ifndef XAPDEPLOYER_H
#define XAPDEPLOYER_H

#ifndef XAPDEPLOYER_INTERNAL_H

typedef struct
{
	// full path to xap file.
	LPWSTR    path;

	// product id (GUID) in text form.
	LPWSTR    productId;

	// instance id (GUID) in text form.
	LPWSTR    instanceId;

	// offer id (GUID) in text form.
	LPWSTR    offerId;

	// is uninstall disabled?
	BOOL      uninstallDisabled;

	// preinstalling app?
	BOOL      preInstall;

	// license pointer
	LPBYTE    License;

	// license length
	DWORD     LicenseLength;
}XAPINSTALLCONTEXT;

#endif

enum InstallationState
{
	AppInstallStarted = 0,
	AppInstallCompleted = 1,
	AppUpdateStarted = 2,
	AppUpdateCompleted = 3,
	AppRemoveStarted = 4,
	AppRemoveCompleted = 5,
	AppUpdateLicenseStarted = 6,
	AppUpdateLicenseCompleted = 7,
	AppDownloadStarted = 8,
	AppDownloadProgress = 9,
	AppDownloadCompleted = 10,
	AppInstallProgress = 11,
	AppUpdateProgress = 12
};

enum EventReceiveError
{
	Success = 0,
	Error_First = 0xFFFFFFFD,
	Error_InvalidParameter = 0xFFFFFFFD,
	Error_NoData = 0xFFFFFFFE,
	Error_NoListener = 0xFFFFFFFF
};

/// Creates new xap installer context.
// * path - full path to xap file.
// * productId - string representation of guid if you have it, NULL otherwise.
// * instanceId - string representation of instanceID. If you don't have it, productID will be used instead.
// * offerId - string representation of offerID. If you don't have it, random value will be used.
// * uninstallDisabled - TRUE if uninstallation shouldn't be available, FALSE otherwise.
// * preInstall - TRUE if request comes from AppPreInstaller.
// * License - application license.
// * LicenseLength - application license length.
// Returns XAPINSTALLCONTEXT if succeed, or NULL otherwise.
XAPINSTALLCONTEXT* Xap_BeginDeploy(wchar_t *path, 
								   wchar_t *productId, 
								   wchar_t *instanceId, 
								   wchar_t *offerId, 
								   bool uninstallDisabled, 
								   bool preInstall,
								   LPBYTE license,
								   DWORD  licenseLength);

// Checks whether current xap is already installed.
// * context - xap installer context.
// Returns TRUE if installed, FALSE otherwise.
BOOL Xap_IsInstalled(XAPINSTALLCONTEXT* context);

// Starts deploying.
// * context - xap installer context.
// * update - TRUE if you want to update the application that is already installed. 
//			  FALSE if application should be installed from the scratch.
// * debugMode - for applications deployed via debugger. Should be considered default for homebrew applications.
// NOTE: Xap Deployer doesn't uninstall applications automatically!
// Returns S_OK if function succeeds.
HRESULT Xap_Install(XAPINSTALLCONTEXT* context, BOOL update, BOOL debugMode);


// Uninstalls app.
// * context - xap installer context
// Returns TRUE if succeed, FALSE otherwise
HRESULT Xap_Uninstall(XAPINSTALLCONTEXT* context);

// wait for incoming application life cycle events
// * context - xap installer context
// * timeout - time to wait for it. INFINITE to wait infinitely.
// * state - pointer to variable that will receive current state
// * progress - pointer to variable that will receive current progress (valid only with *Progress states)
// * hResult - pointer to variable that will receive install result.
// returns Success (EventReceiveError.Success) on success.
DWORD Xap_WaitForEvents(XAPINSTALLCONTEXT* context, DWORD timeout, InstallationState *state, DWORD *progress, HRESULT *hResult);

// Terminates xap installer context and frees used memory
// * context - xap installer context.
void  Xap_EndDeploy(XAPINSTALLCONTEXT *context);


// some known HRESULTs:

#define S_PROCESSING_IN_PROGRESS            ((HRESULT)0x01030101)
// xap file is invalid (?)
#define E_FILE_INVALID                      ((HRESULT)0x8103010B)
// license is invalid (?)
#define E_LICENSE_INVALID                   ((HRESULT)0x8103010C)
// usually happens when application is already installed, or can't be uninstalled, or is being installed, or can't be updated.
#define E_COULDNT_PROCESS_REQUEST           ((HRESULT)0x8103010D)
// usually happens when Package Manager couldn't open file.
#define E_FILE_UNREADABLE                   ((HRESULT)0x8103010E)
#define E_UNEXPECTED_ERROR                  ((HRESULT)0x8103010F)
#define E_UNEXPECTED_ERROR2                 ((HRESULT)0x81030110)
#define E_BUSY                              ((HRESULT)0x81030111)

#endif
