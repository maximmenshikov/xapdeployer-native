#include "stdafx.h"
#include "CXapContextNotificationListener.h"


HRESULT XapContextNotificationListener::QueryInterface (REFIID riid, LPVOID * ppvObj)
{
	return 0x80004001;
}

ULONG XapContextNotificationListener::AddRef()
{
	InterlockedIncrement(&m_cRef);
	return m_cRef;
}

ULONG XapContextNotificationListener::Release()
{
	ULONG ulRefCount = InterlockedDecrement(&m_cRef);
	if (m_cRef == 0)
	{
		delete this;
	}
	return ulRefCount;
}

void XapContextNotificationListener::HandleTokenNotification(PACMAN_CLIENT_TOKEN_NOTIFICATION *notify)
{
}

void XapContextNotificationListener::HandleLifeCycleNotification(PACMAN_CLIENT_LIFECYCLE_NOTIFICATION *notify)
{
	if (notify->header.notificationType != LIFECYCLE_NOTIFICATION)
		return;

	if (memcmp(&notify->header.productID, &m_guid, sizeof(GUID)) == 0)
	{
		HANDLE hOldQueue = GetMsgQueue();

		HANDLE hModule = GetModuleHandle(NULL);
		if (hModule && hModule != INVALID_HANDLE_VALUE)
		{
			MSGQUEUEOPTIONS opt;
			memset(&opt, 0, sizeof(opt));
			opt.dwSize = sizeof(opt);
			opt.bReadAccess = FALSE;
			HANDLE hMsgQueue = OpenMsgQueue(hModule, hOldQueue, &opt);
			if (hMsgQueue && hMsgQueue != INVALID_HANDLE_VALUE)
			{
				WriteMsgQueue(hMsgQueue, notify, sizeof(PACMAN_CLIENT_LIFECYCLE_NOTIFICATION), INFINITE, 0);
				CloseMsgQueue(hMsgQueue);
			}
			
			CloseHandle(hModule);
		}
		/*
		if (notify->hResult)
		{
			FILE *f = fopen("\\errors.txt", "at");
			fwprintf(f, L"ERROR %X\n", notify->hResult);
			fclose(f);
		}*/
		//DWORD progress = notify->progress;
		//DWORD data = EncodeState(notify->status, progress, notify->hResult ? 1 : 0);
		//SetEventData(m_hEvent, data);
		//SetEvent(m_hEvent);
	}
}

void XapContextNotificationListener::HandleBnsNotification(PACMAN_CLIENT_BNS_NOTIFICATION *notify)
{

}

HANDLE XapContextNotificationListener::SetMsgQueue(HANDLE hMsgQueue)
{
	m_hMsgQueue = hMsgQueue;
	return hMsgQueue;
}

HANDLE XapContextNotificationListener::GetMsgQueue()
{
	return m_hMsgQueue;
}

GUID* XapContextNotificationListener::SetGuid(GUID *guid)
{
	memcpy(&m_guid, guid, sizeof(GUID));
	return &m_guid;
}

GUID* XapContextNotificationListener::GetGuid()
{
	return &m_guid;
}

void XapContextNotificationListener::SelfRegister()
{
	PMRegisterForNotification(this, 1, 1);
}

void XapContextNotificationListener::SelfUnregister()
{
	PMUnregisterFromNotification(this);
}



DWORD XapContextNotificationListener::EncodeState(DWORD state, DWORD progress, DWORD error)
{
	DWORD res = (state & 0xFF) | ((progress & 0xFF) << 8) | ((error & 0xFF) << 16);
	return res;
}

void XapContextNotificationListener::DecodeState(DWORD data, DWORD *state, DWORD *progress, DWORD *error)
{
	*state = data & 0xFF;
	*progress = (data >> 8) & 0xFF;
	*error = (data >> 16) & 0xFF;
}
