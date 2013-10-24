#pragma once
#ifndef PACMANCLIENT_INTERNAL_H
#define PACMANCLIENT_INTERNAL_H

#include "PacmanClient.h"

#pragma pack(1)

typedef unsigned long long APPID;

typedef enum
{
    APP_INSTALL_STARTED = 0, 
    APP_INSTALL_FINISHED = 1,
    APP_UPDATE_STARTED = 2, 
    APP_UPDATE_FINISHED = 3,
    APP_UNINSTALL_STARTED = 4,
    APP_UNINSTALL_FINISHED = 5,
    APP_UPDATELICENSE_STARTED = 6,
    APP_UPDATELICENSE_FINISHED = 7,
    APP_DOWNLOAD_STARTED = 8,
    APP_DOWNLOAD_INPROGRESS = 9,
    APP_DOWNLOAD_FINISHED = 10, 
    APP_INSTALL_INPROGRESS = 11,
    APP_UPDATE_INPROGRESS = 12,
} PACMAN_CLIENT_LIFECYCLE_NOTIFICATION_TYPE;


typedef struct
{
	GUID			productID;
	LPWSTR			pszFilePath;		
	GUID			instanceID;		
	LPWSTR          pszUnknown; 	
	LPWSTR          pszUnknown2;
	PBYTE           pbLicense;		
	DWORD           cbLicense;
	BOOL            bUninstallDisabled;
	BOOL            bPreInstall;
	GUID            offerID;
}PACMAN_CLIENT_INSTALLINFO;

typedef enum
{
    LIFECYCLE_NOTIFICATION = 0x1,
    TOKEN_NOTIFICATION     = 0x2,
    BNS_NOTIFICATION       = 0x4,
} PACMAN_CLIENT_NOTIFICATION_TYPE;


typedef struct
{
    ULONG                                           cbSize;
    PACMAN_CLIENT_NOTIFICATION_TYPE                 notificationType;
    APPID                                           appID;
    GUID                                            productID;
} PACMAN_CLIENT_NOTIFICATION;

typedef struct
{
    PACMAN_CLIENT_NOTIFICATION                      header;
    PACMAN_CLIENT_LIFECYCLE_NOTIFICATION_TYPE       status;
    HRESULT                                         hResult;
    USHORT                                          progress;
} PACMAN_CLIENT_LIFECYCLE_NOTIFICATION; 

typedef enum
{
    BNS_CREATED = 0,
    BNS_DELETED,
    BNS_UPDATED,
    BNS_LIMITREACHED,
} PACMAN_CLIENT_BNS_NOTIFICATION_TYPE;


typedef struct
{
    PACMAN_CLIENT_NOTIFICATION                  header;
    PACMAN_CLIENT_BNS_NOTIFICATION_TYPE         notificationType;
    GUID                                        productID;
    ULONG                                       cNotifications;
} PACMAN_CLIENT_BNS_NOTIFICATION;

typedef enum
{
    STARTTOKEN_TYPE_PRIMARY          = 0x1,  
    STARTTOKEN_TYPE_SECONDARY        = 0x2,  
    STARTTOKEN_TYPE_APPLIST          = 0x3,
    STARTTOKEN_TYPE_APPLISTPRIMARY   = 0x4, 
} PACMAN_CLIENT_STARTTOKEN_TYPE;

typedef enum
{
    TOKEN_CREATED = 0,
    TOKEN_DELETED,
    TOKEN_UPDATED,
    TOKEN_APPVISIBLE,
    TOKEN_APPINVISIBLE,
    TOKEN_PINNED,
    TOKEN_UNPINNED,
} PACMAN_CLIENT_TOKEN_NOTIFICATION_TYPE;


typedef struct
{
    PACMAN_CLIENT_NOTIFICATION                  header;
    PACMAN_CLIENT_TOKEN_NOTIFICATION_TYPE       notificationType;
    PACMAN_CLIENT_STARTTOKEN_TYPE               tokenType;
    ULONG                                       cchTokenID;
    WCHAR                                       rgTokenID[1];
} PACMAN_CLIENT_TOKEN_NOTIFICATION;

#pragma pack()

#endif