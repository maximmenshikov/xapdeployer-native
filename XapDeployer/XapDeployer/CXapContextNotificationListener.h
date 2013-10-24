#pragma once
#ifndef XAPCONTEXTNOTIFICATIONLISTENER_H
#define XAPCONTEXTNOTIFICATIONLISTENER_H

#include "CNotificationListener.h"

class XapContextNotificationListener : public NotificationListener
{
public:
	HRESULT QueryInterface (REFIID riid, LPVOID * ppvObj);
	ULONG AddRef();
	ULONG Release();
	void HandleTokenNotification(PACMAN_CLIENT_TOKEN_NOTIFICATION *notify);
	void HandleLifeCycleNotification(PACMAN_CLIENT_LIFECYCLE_NOTIFICATION *notify);
	void HandleBnsNotification(PACMAN_CLIENT_BNS_NOTIFICATION *notify);
	HANDLE SetMsgQueue(HANDLE hMsgQueue);
	HANDLE GetMsgQueue();
	GUID* SetGuid(GUID *guid);
	GUID* GetGuid();
	void SelfRegister();
	void SelfUnregister();
	DWORD EncodeState(DWORD state, DWORD progress, DWORD error);
	void DecodeState(DWORD data, DWORD *state, DWORD *progress, DWORD *error);
private:
	LONG m_cRef;
	HANDLE m_hMsgQueue;
	GUID m_guid;
};


#endif