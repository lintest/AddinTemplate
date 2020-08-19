// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//
#ifndef __STDAFX_H__
#define __STDAFX_H__

#ifndef __linux__
#include <windows.h>
#endif //__linux__

#include <string>

std::wstring MB2WC(const std::string& source);
std::string WC2MB(const std::wstring& source);

#endif //__STDAFX_H__
