#pragma once
#ifndef XNAFRAMEWORKCORE_H
#define XNAFRAMEWORKCORE_H

typedef enum DismissedReason
{
	ButtonPressed,
	Canceled,
	TimedOut,
	Dismissed
};

typedef struct 
{
	DismissedReason disReason;
	int iButtonPressed;
	int cEditFieldLength;
}GS_ActionDialogResult;

typedef enum 
{
	None,
	Error,
	Warning,
	Alert
}MessageBoxIcon;

extern "C" HANDLE CreateMessageBoxDialog(wchar_t *title, wchar_t *text, wchar_t *button1, wchar_t *button2, MessageBoxIcon icon);
extern "C" int GetDialogResult(HANDLE hDialog, GS_ActionDialogResult *result);
extern "C" int ReleaseDialog(HANDLE hDialog);

#endif
