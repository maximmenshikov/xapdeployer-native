#include "stdafx.h"
#include "XapDeployer_internal.h"

//
// Product Id code
//
//
#include "..\tinyxml\tinyxml2.h"
#include <string>
#include "..\..\minizip\unzip.h"
#include "..\GUIDConverter.h"


using namespace std;

//#define TEST

bool UnZip(const wstring& zippath , const wstring& pngpath);
class w2a 
{
public:
	explicit w2a(const wchar_t* wstr)
		: buffer_(0)
	{ 
		if (wstr)
		{
			size_t len = wcslen(wstr);
			size_t conv = ::WideCharToMultiByte(CP_ACP, 0, wstr, int(len), 0, 0, 0, 0);
			buffer_ = new char[conv + 1];
			::WideCharToMultiByte(CP_ACP, 0, wstr, int(len), buffer_, conv, 0, 0);
			buffer_[conv] = 0;
		}
	}

	~w2a()
	{ delete[] buffer_;  }

	operator const char*() const
	{ return buffer_; }

private:
	char* buffer_;
};


bool UnZipFile(const wstring& zippath , char *fname, char **outputBuf, wstring* output)
{	
	unzFile uzFile = unzOpen(w2a(zippath.c_str()));
	if (uzFile == NULL)
	{
#ifdef TEST
		MessageBox(NULL, L"Couldn't open xap", NULL, 0);
#endif
		return false;
	}
	int result = unzGoToFirstFile(uzFile);
	while (result == UNZ_OK)
	{
		unz_file_info uzfi;
		char filename[MAX_PATH];
		char comment[255];
		ZeroMemory(&uzfi, sizeof(uzfi));

		if (UNZ_OK != unzGetCurrentFileInfo(uzFile, &uzfi, filename, MAX_PATH, NULL, 0, comment, 255))
			break;

		if (strcmp(filename, fname) == 0)
		{
			if (unzOpenCurrentFile(uzFile) != UNZ_OK)
				break;

			// read the file and output
			int nRet = UNZ_OK;
			char pBuffer[20480];
			char *buf = new char[uzfi.uncompressed_size];
			int pos = 0;
			do{
				nRet = unzReadCurrentFile(uzFile, pBuffer, 20480);

				if (nRet > 0)
				{
					memcpy(buf + pos, pBuffer, nRet);
				}
				else
				{
					break;
				}
				pos += nRet;
			}while (nRet > 0);

			if (outputBuf == NULL)
			{
				HANDLE hOutputFile = ::CreateFile(output->c_str(), GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
				if (INVALID_HANDLE_VALUE == hOutputFile)
					break;
				DWORD dwBytesWritten;
				WriteFile(hOutputFile, buf, uzfi.uncompressed_size, &dwBytesWritten, NULL);
				CloseHandle(hOutputFile);
				delete(buf);
			}
			else
			{
				*outputBuf = buf;
			}
			unzCloseCurrentFile(uzFile);
		}
		result = unzGoToNextFile(uzFile);
	}
	
	return true;
}

using namespace tinyxml2;

wchar_t *GetProductID(wchar_t *path)
{
	char *buf = NULL;
	UnZipFile(path, "WMAppManifest.xml", &buf, NULL);
	char *newBuf = NULL;
	if (buf)
	{
		const char *result = NULL;
		XMLDocument doc;

		if (doc.Parse(buf))
		{
			int outputSize = WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)buf, -1, NULL, 0, NULL, NULL);
			newBuf = new char[outputSize];
			WideCharToMultiByte(CP_UTF8, 0, (wchar_t*)buf, -1, newBuf, outputSize, NULL, NULL);

			doc.Parse(newBuf);
			delete[] newBuf;
			if (doc.ErrorID())
			{
				return NULL;
			}
		}
		char *levels[] = {"Deployment", "App", "ProductID"};
		int levelCount = 3;
		XMLElement* elem = doc.FirstChildElement(levels[0]);
		for (int i = 1; i < levelCount; i++)
		{
			if (i == (levelCount - 1))
			{
				result = elem->Attribute(levels[i]);
			}
			else
			{
				elem = elem->FirstChildElement(levels[i]);
				if (!elem)
					break;
			}
		}
		if (result)
		{
			wchar_t *str = new wchar_t[strlen(result) + 1];
			mbstowcs(str, result, strlen(result) + 1);
			return str;
		}
	}
	return NULL;
}

XAPINSTALLCONTEXT* Xap_BeginDeploy(wchar_t *path, 
								   wchar_t *newProductId, 
								   wchar_t *newInstanceId, 
								   wchar_t *newOfferId, 
								   bool uninstallDisabled, 
								   bool preInstall,
								   LPBYTE newLicense,
								   DWORD  newLicenseLength)
{
#ifdef TEST
	//MessageBox(NULL, path, 0, 0);
	if (GetFileAttributes(path) == INVALID_FILE_ATTRIBUTES)
	{
		MessageBox(NULL, path, L"GetFileAttributes = INVALID_FILE_ATTRIBUTES", 0);
		return NULL;
	}
#endif

	wchar_t *productId = NULL;
	if (newProductId)
	{
		productId = new wchar_t[wcslen(newProductId) + 1];
		wcscpy(productId, newProductId);
	}
	else
	{
		productId = GetProductID(path);
		if (productId == NULL)
			return NULL;
	}

	wchar_t *instanceId = NULL;
	if (newInstanceId)
	{
		instanceId = new wchar_t[wcslen(newInstanceId) + 1];
		wcscpy(instanceId, newInstanceId);
	}

	wchar_t *offerId = NULL;
	if (newOfferId)
	{
		offerId = new wchar_t[wcslen(newOfferId) + 1];
		wcscpy(offerId, newOfferId);
	}

	XAPINSTALLCONTEXT *result = new XAPINSTALLCONTEXT;
	memset(result, 0, sizeof(XAPINSTALLCONTEXT));
	if (path)
	{
		result->path = new wchar_t[wcslen(path) + 1];
		wcscpy(result->path, path);
	}
	result->productId = productId;
	result->instanceId = instanceId;
	result->offerId = offerId;
	result->uninstallDisabled = uninstallDisabled;
	result->preInstall = preInstall;

	if (newLicense && newLicenseLength)
	{
		result->License = new BYTE[newLicenseLength];
		memcpy(result->License, newLicense, newLicenseLength);
		result->LicenseLength = newLicenseLength;
	}

	MSGQUEUEOPTIONS opt;
	opt.dwSize = sizeof(MSGQUEUEOPTIONS);
	opt.dwFlags = MSGQUEUE_NOPRECOMMIT | MSGQUEUE_ALLOW_BROKEN;
	opt.dwMaxMessages = 0;
	opt.cbMaxMessage = sizeof(PACMAN_CLIENT_LIFECYCLE_NOTIFICATION);
	opt.bReadAccess = TRUE;
	result->hMsgQueue = CreateMsgQueue(NULL, &opt);

	return result;
}

void Xap_EndDeploy(XAPINSTALLCONTEXT *context)
{
	if (context)
	{
		if (context->listener)
		{
			context->listener->SelfUnregister();
			context->listener->Release();
		}
		if (context->path)
			delete(context->path);
		if (context->productId)
			delete(context->productId);
		if (context->instanceId)
			delete(context->instanceId);
		if (context->offerId)
			delete(context->offerId);
		if (context->License)
			delete [](context->License);

		if (context->hMsgQueue && context->hMsgQueue != INVALID_HANDLE_VALUE)
			CloseMsgQueue(context->hMsgQueue);

		delete(context);
	}
}

BOOL Xap_IsInstalled(XAPINSTALLCONTEXT* context)
{
	BOOL result = FALSE;

	if (context->productId)
	{
		CApplicationInfo *appInfo = NULL;

		GUID guid;
		CLSIDFromString(context->productId, &guid);

		if (GetApplicationInfoByProductID(guid, &appInfo) == 0)
		{
			result = TRUE;
			delete(appInfo);
		}
	}
	return result;
}

void Xap_CreateListener(XAPINSTALLCONTEXT* context, GUID guid)
{
	XapContextNotificationListener *listener = new XapContextNotificationListener;
	if (listener)
	{
		listener->AddRef();
		listener->SetMsgQueue(context->hMsgQueue);
		listener->SetGuid(&guid);
		listener->SelfRegister();
		context->listener = listener;
	}
}

HRESULT Xap_Install(XAPINSTALLCONTEXT* context, BOOL update, BOOL debugMode)
{
	if (context == NULL || context->path == NULL || context->productId == NULL)
		return E_INVALIDARG;
	GUID productGuid;
	String2GUID(context->productId, &productGuid);

	if (context->listener == NULL)
		Xap_CreateListener(context, productGuid);

	PACMAN_CLIENT_INSTALLINFO package;
	memset(&package, 0, sizeof(PACMAN_CLIENT_INSTALLINFO));
	package.pszFilePath = context->path;

	memcpy(&package.productID, &productGuid, sizeof(GUID));

	if (context->instanceId)
		String2GUID(context->instanceId, &package.instanceID);
	else
		memcpy(&package.instanceID, &productGuid, sizeof(GUID));

	if (context->offerId)
		String2GUID(context->offerId, &package.offerID);
	else
		CeGenRandom(0x10, (PBYTE)&package.offerID);

	package.pbLicense = context->License;
	package.cbLicense = context->LicenseLength;

	package.bUninstallDisabled = context->uninstallDisabled;
	package.bPreInstall = context->preInstall;

	HRESULT result;
	if (debugMode)
	{
		if (update)
			result = PMBeginUpdateDeployedPackage(&package);
		else
			result = PMBeginDeployPackage(&package);
	}
	else
	{
		if (update)
			result = PMBeginUpdate(&package);
		else
			result = PMBeginInstall(&package);
	}
	return result;
}

DWORD Xap_WaitForEvents(XAPINSTALLCONTEXT* context, DWORD timeout, InstallationState *state, DWORD *progress, HRESULT *hResult)
{
	if (context == NULL || state == NULL || progress == NULL || hResult == NULL)
		return Error_InvalidParameter;
	if (context->listener)
	{
		HANDLE hMsgQueue = context->hMsgQueue;
		if (hMsgQueue && hMsgQueue != INVALID_HANDLE_VALUE)
		{
			DWORD dwBytesRead = 0;
			DWORD dwMsgFlags = 0;
			PACMAN_CLIENT_LIFECYCLE_NOTIFICATION msg;
			if (ReadMsgQueue(hMsgQueue, &msg, sizeof(PACMAN_CLIENT_LIFECYCLE_NOTIFICATION), &dwBytesRead, timeout, &dwMsgFlags) == TRUE)
			{
				*hResult = msg.hResult;
				*state = (InstallationState)msg.status;
				*progress = msg.progress;
				return Success;
			}
		}
		return Error_NoData;
	}
	return Error_NoListener;
}

HRESULT Xap_Uninstall(XAPINSTALLCONTEXT* context)
{
	if (context == NULL || context->productId == NULL)
		return E_INVALIDARG;

	GUID productGuid;
	CLSIDFromString(context->productId, &productGuid);

	if (context->listener == NULL)
		Xap_CreateListener(context, productGuid);

	return PMBeginUninstall(productGuid);
}
