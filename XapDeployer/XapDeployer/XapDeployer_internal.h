/*
	Header: XapDeployer.h
	Project: XapDeployer
	Author: ultrashot
	Comment: private definition for Xap Deployer code. 
	
	Please, note that not a single part of Xap Deployer 
	code or its static library should be published without author's permission.
*/
#pragma once
#ifndef XAPDEPLOYER_INTERNAL_H
#define XAPDEPLOYER_INTERNAL_H

#include "ProductId.h"
#include "PacmanClient.h"
#include "CNotificationListener.h"
#include "CXapContextNotificationListener.h"


typedef struct
{
	LPWSTR    path;
	LPWSTR    productId;
	LPWSTR    instanceId;
	LPWSTR    offerId;
	BOOL      uninstallDisabled;
	BOOL      preInstall;
	LPBYTE    License;
	DWORD     LicenseLength;
	/* private */
	XapContextNotificationListener *listener;
	HANDLE    hMsgQueue;
}XAPINSTALLCONTEXT;

#include "XapDeployer.h"


#endif
