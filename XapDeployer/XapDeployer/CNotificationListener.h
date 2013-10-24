#pragma once
#ifndef NOTIFICATIONLISTENER_H
#define NOTIFICATIONLISTENER_H

#include "PacmanClient_internal.h"

class NotificationListener : public IUnknown
{
public:
	virtual void HandleTokenNotification(PACMAN_CLIENT_TOKEN_NOTIFICATION *notify) = NULL;
	virtual void HandleLifeCycleNotification(PACMAN_CLIENT_LIFECYCLE_NOTIFICATION *notify) = NULL;
	virtual void HandleBnsNotification(PACMAN_CLIENT_BNS_NOTIFICATION *notify) = NULL;
};

#endif
