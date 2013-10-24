/*
	File: guidconverter.cpp
	Project: ComXapHandler
	State: internal
	Purpose: guid->string and opposite conversion
*/
#include "stdafx.h"

void GUID2String(GUID *guid, LPOLESTR *str)
{
	if (guid && str)
	{
		CLSID clsid = *guid;
		StringFromCLSID(clsid, str);
	}
}

void String2GUID(LPOLESTR str, GUID *guid)
{
	if (str && guid)
	{
		wchar_t *targetStr = str;

		wchar_t strGuid[50];
		if (str[0] == L'{')
		{
			targetStr = str;
		}
		else
		{
			memset(strGuid, 0, sizeof(strGuid));
			strGuid[0] = L'{';
			wcscat_s(strGuid, 50, str);
			wcscat_s(strGuid, 50, L"}");
			targetStr = strGuid;
		}
		CLSIDFromString(targetStr, guid);
	}
}
