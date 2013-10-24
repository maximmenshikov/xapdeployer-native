// XapInstall.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "XapDeployer\XapDeployer.h"

#ifndef LIBRARIANVERSION
#include "XNAFrameworkCore.h"

typedef struct
{
	wchar_t *name;
	wchar_t item[500];
}LANGUAGE_ITEM;

LANGUAGE_ITEM items[] = {
							{L"Title", L"Xap installer"},
							{L"UnexpectedError", L"Unexpected error while opening *.xap"},
							{L"InstallQuestion", L"Do you really want to install this application?"},
							{L"InstallationCompleted", L"Application was installed successfully"},
							{L"InstallationFailed", L"Installation failed"},
							{L"AlreadyInstalled", L"Application is already installed. Do you want to update it?"},
							{L"Yes", L"Yes"},
							{L"No", L"No"},
							{L"Ok", L"Ok"},
							{L"Cancel", L"Cancel"},
							{L"Retry", L"Retry"},
							{NULL, L" "}
};

typedef enum
{
	Title = 0,
	UnexpectedError = 1,
	InstallQuestion = 2,
	InstallationCompleted = 3,
	InstallationFailed = 4,
	AlreadyInstalled = 5,
	Yes = 6,
	No = 7,
	Ok = 8,
	Cancel = 9,
	Retry = 10
}Language;


wchar_t *GetTranslation(Language lng)
{
	return items[lng].item;
}

/*
#define MB_OK                       0x00000000L
#define MB_OKCANCEL                 0x00000001L
#define MB_ABORTRETRYIGNORE         0x00000002L
#define MB_YESNOCANCEL              0x00000003L
#define MB_YESNO                    0x00000004L
#define MB_RETRYCANCEL              0x00000005L

#define MB_ICONHAND                 0x00000010L
#define MB_ICONQUESTION             0x00000020L
#define MB_ICONEXCLAMATION          0x00000030L
#define MB_ICONASTERISK             0x00000040L
*/

#define XNA_RESULT_NONE		0
#define XNA_RESULT_LEFT		1
#define XNA_RESULT_RIGHT	2

int XnaMessageBox(HWND hWnd, wchar_t *text, wchar_t *caption, int flags)
{
	int icon = None;
	wchar_t *btn1 = GetTranslation(Ok);
	wchar_t *btn2 = NULL;
	if (flags & MB_OKCANCEL)
	{
		btn2 = GetTranslation(Cancel);
	}
	else if (flags & MB_YESNO)
	{
		btn1 = GetTranslation(Yes);
		btn2 = GetTranslation(No);
	}
	else if (flags & MB_RETRYCANCEL)
	{
		btn1 = GetTranslation(Retry);
		btn2 = GetTranslation(Cancel);
	}
	if (flags & MB_ICONQUESTION)
		icon = Alert;
	else if (flags & MB_ICONEXCLAMATION)
		icon = Warning;

	GS_ActionDialogResult result;
	int funcResult = XNA_RESULT_NONE;
	HANDLE mb = CreateMessageBoxDialog(caption, text, btn1, btn2, Alert);
	
	GetDialogResult(mb, &result);
	if (result.disReason == ButtonPressed)
	{
		if (result.iButtonPressed == 0)
		{
			funcResult = XNA_RESULT_LEFT;
		}
		else if (result.iButtonPressed == 1)
		{
			funcResult = XNA_RESULT_RIGHT;
		}
	}
	ReleaseDialog(mb);
	return funcResult;
}

void InitializeLanguages()
{
	DWORD dwValue = 0;
	HKEY hKey = NULL;
	wchar_t keyName[500];
	swprintf(keyName, L"Software\\OEM\\XapInstaller\\Localization");
	RegCreateKeyExW(HKEY_LOCAL_MACHINE, keyName, 0, NULL, 0, 0, NULL, &hKey, &dwValue);

	if (hKey)
	{
		int x = 0;
		while (true)
		{
			if (items[x].name == NULL)
				break;
			DWORD size = 500;
			DWORD type = REG_SZ;
			RegQueryValueEx(hKey, items[x].name, 0, &type, (LPBYTE)items[x].item, &size);
			x++;
		}
		RegCloseKey(hKey);
	}
}

void ShowResult(HWND hWnd, wchar_t *shortPath, BOOL result, bool showUi)
{
	if (showUi == true)
	{
		//MessageBeep(0x30);
		if (result == TRUE)
			XnaMessageBox(hWnd, GetTranslation(InstallationCompleted), shortPath, MB_OK);
		else
			XnaMessageBox(hWnd, GetTranslation(InstallationFailed), shortPath, MB_OK);
	}
}

bool PerformInstall(XAPINSTALLCONTEXT *context, bool update, BOOL debugMode)
{
	bool result = false;
	if (Xap_Install(context, update, debugMode) == S_OK)
	{
		InstallationState state;
		DWORD progress = 0;
		HRESULT error = 0;
		while (Xap_WaitForEvents(context, INFINITE, &state, &progress, &error) < Error_First)
		{
			if (state == AppInstallCompleted || state == AppUpdateCompleted)
			{
				if (error == 0)
					result = true;
				break;
			}
		}
	}
	return result;
}

bool PerformUninstall(XAPINSTALLCONTEXT *context)
{
	bool result = false;
	if (Xap_Uninstall(context) == TRUE)
	{
		InstallationState state;
		DWORD progress = 0;
		HRESULT hr = 0;
		while (Xap_WaitForEvents(context, INFINITE, &state, &progress, &hr) < Error_First)
		{
			if (state == AppRemoveCompleted)
			{
				result = true;
				break;
			}
		}
	}
	return result;
}

void Deploy(wchar_t *path, wchar_t *VisibleName, bool showUi, bool forceInstall, bool forceUpdate, wchar_t *instanceId, wchar_t *offerId, bool uninstallDisabled, bool preInstall, LPBYTE pbLicense, DWORD cbLicense)
{
	XAPINSTALLCONTEXT *context = Xap_BeginDeploy(path, NULL, instanceId, offerId, uninstallDisabled, preInstall, pbLicense, cbLicense);
	BOOL debugMode = TRUE;
	if (pbLicense || cbLicense || offerId)
		debugMode = FALSE;
	if (context)
	{
		HWND hWnd = GetForegroundWindow();
		wchar_t *shortPath = wcsrchr(VisibleName, L'\\') + 1;
		if (shortPath == NULL)
		{
			shortPath = GetTranslation(Title);
		}
		if (Xap_IsInstalled(context) == FALSE)
		{
			bool install = true;
			if (showUi == true)
			{
				//MessageBeep(0x30);
				if (XnaMessageBox(hWnd, GetTranslation(InstallQuestion), shortPath, MB_YESNO) != XNA_RESULT_LEFT)
				{
					install = false;
				}
			}
			if (install)
			{
				ShowResult(hWnd, shortPath, PerformInstall(context, false, debugMode), showUi);
			}
		}
		else
		{
			if (showUi == true)
			{
				//MessageBeep(0x30);
				if (XnaMessageBox(hWnd, GetTranslation(AlreadyInstalled), shortPath, MB_YESNO) == XNA_RESULT_LEFT)
				{
					ShowResult(hWnd, shortPath, PerformInstall(context, true, debugMode), showUi);
					//ShowResult(hWnd, shortPath, Xap_Install(context, INFINITE, TRUE), showUi);
				}
			}
			else
			{
				if (forceInstall == true)
				{
					ShowResult(hWnd, shortPath, PerformInstall(context, false, debugMode), showUi);
					//ShowResult(hWnd, shortPath, Xap_Install(context, INFINITE, FALSE), showUi);
				}
				else if (forceUpdate == true)
				{
					ShowResult(hWnd, shortPath, PerformInstall(context, true, debugMode), showUi);
					//ShowResult(hWnd, shortPath, Xap_Install(context, INFINITE, TRUE), showUi);
				}
			}
		}
	}
	else
	{
		XnaMessageBox(NULL, GetTranslation(UnexpectedError), GetTranslation(Title), MB_OK | MB_ICONEXCLAMATION);
	}
	Xap_EndDeploy(context);
}

wchar_t *NormalizePath(wchar_t *src)
{
	wchar_t *dest = new wchar_t[wcslen(src) + 1];
	memset(dest, 0, sizeof(wchar_t) * wcslen(src));
	int pos = 0;
	for (unsigned int x = 0; x < wcslen(src); x++)
	{
		if (src[x] == L'%')
		{
			wchar_t t[3];
			t[0] = src[x + 1];
			t[1] = src[x + 2];
			t[2] = L'\0';
			wchar_t *err;
			wchar_t symbol = (wchar_t)wcstoul(t, &err, 16);
			dest[pos] = symbol;
			x += 2;
		}
		else if (src[x] == L'\"')
		{
			continue;
		}
		else
		{
			dest[pos] = src[x];
		}
		pos++;
	}
	return dest;
}

int _tmain(int argc, _TCHAR* argv[])
{
	HANDLE mutex = CreateMutex(NULL, TRUE, L"XapInstallMutex");
	if (GetLastError() == ERROR_ALREADY_EXISTS)
	{
		CloseHandle(mutex);
		return -1;
	}
	
	DWORD result = 0;
	if (argc > 1)
	{
		InitializeLanguages();

		wchar_t *instanceId = NULL;
		wchar_t *offerId = NULL;
		bool showUi = true;
		bool forceInstall = false;
		bool forceUpdate = false;
		bool uninstallDisabled = false;
		bool preInstall = false;
		LPWSTR licensePath = NULL;
		LPBYTE pbLicense = NULL;
		DWORD cbLicense = 0;
		for (int x = 1; x < (argc - 1); x++)
		{
			if (wcscmp(argv[x], L"-noui") == 0)
			{
				showUi = false;
			}
			else if (wcscmp(argv[x], L"-forceinstall") == 0)
			{
				forceInstall = true;
			}
			else if (wcscmp(argv[x], L"-forceupdate") == 0)
			{
				forceUpdate = true;
			}
			else if (wcsnicmp(argv[x], L"-instance", 9) == 0)
			{
				instanceId = argv[x] + 9;
			}
			else if (wcsnicmp(argv[x], L"-offer", 6) == 0)
			{
				instanceId = argv[x] + 6;
			}
			else if (wcscmp(argv[x], L"-nouninstall") == 0)
			{
				uninstallDisabled = true;
			}
			else if (wcscmp(argv[x], L"-preinstall") == 0)
			{
				preInstall = true;
			}
			else if (wcsnicmp(argv[x], L"-license", 8) == 0)
			{
				licensePath = argv[x] + 8;
			}
		}
		if (licensePath)
		{
			HANDLE hFile = CreateFile(licensePath, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				cbLicense = GetFileSize(hFile, NULL);
				if (cbLicense)
				{
					pbLicense = new BYTE[cbLicense];
					DWORD dwBytesRead = 0;
					ReadFile(hFile, pbLicense, cbLicense, &dwBytesRead, NULL);
				}
				CloseHandle(hFile);
			}
		}
		wchar_t *nPath = NormalizePath(argv[argc - 1]);
		Deploy(argv[argc - 1], nPath, showUi, forceInstall, forceUpdate, instanceId, offerId, uninstallDisabled, preInstall, pbLicense, cbLicense);
		delete(nPath);

		result = 1;
	}

	CloseHandle(mutex);
	return result;
}
#endif 
