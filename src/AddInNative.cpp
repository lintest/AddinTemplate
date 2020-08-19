#include "stdafx.h"

#ifdef _WINDOWS
#include <locale>
#pragma warning (disable : 4267)
#pragma warning (disable : 4302)
#pragma warning (disable : 4311)
#else
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#endif

#include <wchar.h>
#include <string>
#include <algorithm>
#include <codecvt>
#include <cwctype>
#include <sstream>

#include "AddInNative.h"

#ifdef _WINDOWS
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
#endif

const WCHAR_T* GetClassNames()
{
	static const std::u16string names(AddInNative::getComponentNames());
	return (const WCHAR_T*)names.c_str();
}

long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface)
{
	if (*pInterface) return 0;
	auto cls_name = std::u16string(reinterpret_cast<const char16_t*>(wsName));
	return long(*pInterface = AddInNative::createObject(cls_name));
}

long DestroyObject(IComponentBase** pInterface)
{
	if (!*pInterface) return -1;
	delete* pInterface;
	*pInterface = nullptr;
	return 0;
}

std::map<std::u16string, CompFunction> AddInNative::components;

bool AddInNative::Init(void* pConnection)
{
	m_iConnect = static_cast<IAddInDefBase*>(pConnection);
	if (m_iConnect) m_iConnect->SetEventBufferDepth(100);
	return m_iConnect != nullptr;
}

bool AddInNative::setMemManager(void* memory)
{
	m_iMemory = static_cast<IMemoryManager*>(memory);
	return m_iMemory != nullptr;
}

long AddInNative::GetInfo()
{
	return 2000;
}

void AddInNative::Done()
{
}

bool AddInNative::RegisterExtensionAs(WCHAR_T** wsLanguageExt)
{
	*wsLanguageExt = W(name.c_str());
	return true;
}

long AddInNative::GetNProps()
{
	return properties.size();
}

long AddInNative::FindProp(const WCHAR_T* wsPropName)
{
	std::u16string name((char16_t*)wsPropName);
	for (auto it = properties.begin(); it != properties.end(); it++) {
		for (auto n = it->names.begin(); n != it->names.end(); n++) {
			if (n->compare(name) == 0) return long(it - properties.begin());
		}
	}
	return -1;
}

const WCHAR_T* AddInNative::GetPropName(long lPropNum, long lPropAlias)
{
	auto it = std::next(properties.begin(), lPropNum);
	if (it == properties.end()) return nullptr;
	auto nm = std::next(it->names.begin(), lPropAlias);
	if (nm == it->names.end()) return nullptr;
	return W(nm->c_str());
}

bool AddInNative::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{
	auto it = std::next(properties.begin(), lPropNum);
	if (it == properties.end()) return false;
	try {
		return it->getter(pvarPropVal);
	}
	catch (...) {
		return false;
	}
}

bool AddInNative::SetPropVal(const long lPropNum, tVariant* pvarPropVal)
{
	auto it = std::next(properties.begin(), lPropNum);
	if (it == properties.end()) return false;
	try {
		return it->setter(pvarPropVal);
	}
	catch (...) {
		return false;
	}
}

bool AddInNative::IsPropReadable(const long lPropNum)
{
	auto it = std::next(properties.begin(), lPropNum);
	if (it == properties.end()) return false;
	return (bool)it->getter;
}

bool AddInNative::IsPropWritable(const long lPropNum)
{
	auto it = std::next(properties.begin(), lPropNum);
	if (it == properties.end()) return false;
	return (bool)it->setter;
}

long AddInNative::GetNMethods()
{
	return 0;
}

long AddInNative::FindMethod(const WCHAR_T* wsMethodName)
{
	return -1;
}

const WCHAR_T* AddInNative::GetMethodName(const long lMethodNum,
	const long lMethodAlias)
{
	return 0;
}

long AddInNative::GetNParams(const long lMethodNum)
{
	return 0;
}

bool AddInNative::GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant* pvarParamDefValue)
{
	return false;
}

bool AddInNative::HasRetVal(const long lMethodNum)
{
	return false;
}

bool AddInNative::CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray)
{
	return false;
}

bool AddInNative::CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{
	return false;
}

void AddInNative::SetLocale(const WCHAR_T* locale)
{
	try {
		std::locale::global(std::locale{ WCHAR2MB(locale) });
	}
	catch (std::runtime_error&) {
		std::locale::global(std::locale{ "" });
	}
}

std::u16string AddInNative::getComponentNames() {
	const char16_t* const delim = u"|";
	std::vector<std::u16string> names;
	for (auto it = components.begin(); it != components.end(); ++it) names.push_back(it->first);
	std::basic_ostringstream<char16_t, std::char_traits<char16_t>, std::allocator<char16_t>> imploded;
	std::copy(names.begin(), names.end(), std::ostream_iterator<std::u16string, char16_t, std::char_traits<char16_t>>(imploded, delim));
	std::u16string result = imploded.str();
	result.pop_back();
	return result;
}

std::u16string AddInNative::AddComponent(const std::u16string& name, CompFunction creator)
{
	components.insert(std::pair<std::u16string, CompFunction>(name, creator));
	return name;
}

AddInNative* AddInNative::createObject(const std::u16string& name) {
	auto it = components.find(name);
	if (it == components.end()) return nullptr;
	AddInNative* object = it->second();
	object->name = name;
	return object;
}

void AddInNative::AddProperty(const std::vector<std::u16string>& names, PropFunction getter, PropFunction setter)
{
	properties.push_back({ names, getter, setter });
}

void AddInNative::AddProperty(const std::u16string& nameEn, const std::u16string& nameRu, PropFunction getter, PropFunction setter)
{
	properties.push_back({ { nameRu, nameEn }, getter, setter });
}

void AddInNative::AddMethod(const std::vector<std::u16string>& names, MethFunction handler)
{
	methods.push_back({ names, handler });
}

void AddInNative::AddMethod(const std::u16string& nameEn, const std::u16string& nameRu, MethFunction handler)
{
	methods.push_back({ { nameRu, nameEn }, handler });
}

bool ADDIN_API AddInNative::AllocMemory(void** pMemory, unsigned long ulCountByte) const
{
	return m_iMemory ? m_iMemory->AllocMemory(pMemory, ulCountByte) : false;
}

std::string AddInNative::WCHAR2MB(std::basic_string_view<WCHAR_T> src) {
#ifdef _WINDOWS
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> cvt_utf8_utf16;
	return cvt_utf8_utf16.to_bytes(src.data(), src.data() + src.size());
#else
	static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt_utf8_utf16;
	return cvt_utf8_utf16.to_bytes(reinterpret_cast<const char16_t*>(src.data()),
		reinterpret_cast<const char16_t*>(src.data() + src.size()));
#endif
}

std::wstring AddInNative::WCHAR2WC(std::basic_string_view<WCHAR_T> src) {
#ifdef _WINDOWS
	return std::wstring(src);
#else
	std::wstring_convert<std::codecvt_utf16<wchar_t, 0x10ffff, std::little_endian>> conv;
	return conv.from_bytes(reinterpret_cast<const char*>(src.data()),
		reinterpret_cast<const char*>(src.data() + src.size()));
#endif
}

std::u16string AddInNative::MB2WCHAR(std::string_view src) {
#ifdef _WINDOWS
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> cvt_utf8_utf16;
	std::wstring tmp = cvt_utf8_utf16.from_bytes(src.data(), src.data() + src.size());
	return std::u16string(reinterpret_cast<const char16_t*>(tmp.data()), tmp.size());
#else
	static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> cvt_utf8_utf16;
	return cvt_utf8_utf16.from_bytes(src.data(), src.data() + src.size());
#endif
}

std::u16string AddInNative::upper(std::u16string& str)
{
	std::transform(str.begin(), str.end(), str.begin(), std::towupper);
	return str;
}

std::wstring AddInNative::upper(std::wstring& str)
{
	std::transform(str.begin(), str.end(), str.begin(), std::towupper);
	return str;
}

AddInNative::VarinantHelper& AddInNative::VarinantHelper::operator<<(const std::string& str)
{
	return operator<<(AddInNative::MB2WCHAR(str));
}

AddInNative::VarinantHelper& AddInNative::VarinantHelper::operator<<(const std::wstring& str)
{
#ifdef _WINDOWS
	return operator<<(std::u16string(reinterpret_cast<const char16_t*>(str.data()), str.size()));
#else
	return operator<<(AddInNative::WC2MB(str));
#endif
}

AddInNative::VarinantHelper& AddInNative::VarinantHelper::operator<<(int64_t value)
{
	TV_VT(pvar) = VTYPE_I4;
	TV_I8(pvar) = value;
	return *this;
}

AddInNative::VarinantHelper& AddInNative::VarinantHelper::operator<<(int32_t value)
{
	TV_VT(pvar) = VTYPE_I4;
	TV_I4(pvar) = value;
	return *this;
}

AddInNative::VarinantHelper& AddInNative::VarinantHelper::operator<<(bool value)
{
	TV_VT(pvar) = VTYPE_BOOL;
	TV_BOOL(pvar) = value;
	return *this;
}

AddInNative::VarinantHelper& AddInNative::VarinantHelper::operator<<(const std::u16string& str)
{
	TV_VT(pvar) = VTYPE_PWSTR;
	pvar->pwstrVal = nullptr;
	size_t size = (str.size() + 1) * sizeof(char16_t);
	if (addin->AllocMemory(reinterpret_cast<void**>(&pvar->pwstrVal), size)) {
		memcpy(pvar->pwstrVal, str.c_str(), size);
		pvar->wstrLen = str.size();
	}
	success = pvar->pwstrVal != nullptr;
	return *this;
}

WCHAR_T* AddInNative::W(const char16_t* str) const
{
	WCHAR_T* res = NULL;
	size_t length = std::char_traits<char16_t>::length(str);
	if (str && length) {
		unsigned long size = (length + 1) * sizeof(WCHAR_T);
		if (AllocMemory((void**)&res, size)) {
			memcpy(res, str, size);
		}
	}
	return res;
}
