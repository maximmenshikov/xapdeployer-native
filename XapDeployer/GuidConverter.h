/*
	Header: GuidConverter.h
	Project: ComXapHandler
	State: internal
*/
#pragma once
#ifndef GUIDCONVERTER_H
#define GUIDCONVERTER_H

void GUID2String(GUID *guid, LPOLESTR *str);
void String2GUID(LPOLESTR str, GUID *guid);

#endif