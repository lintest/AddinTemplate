#ifndef __ADDINNATIVE_H__
#define __ADDINNATIVE_H__

#ifdef _WINDOWS
#include <wtypes.h>
#endif //_WINDOWS

#include <map>
#include <string>
#include <vector>
#include <variant>
#include <functional>

#include "ComponentBase.h"
#include "AddInDefBase.h"
#include "IMemoryManager.h"

class AddInNative;

using CompFunction = std::function<AddInNative* ()>;

class DefaultHelper {
public:
	std::variant<
		std::monostate,
		std::u16string,
		int32_t,
		double,
		bool
	> variant;
public:
	DefaultHelper() : variant(std::monostate()) {}
	DefaultHelper(const std::u16string& s) : variant(s) {}
	DefaultHelper(int32_t value) : variant(value) {}
	DefaultHelper(double value) : variant(value) {}
	DefaultHelper(bool value) : variant(value) {}
	DefaultHelper(const char16_t* value) {
		if (value) variant = std::u16string(value);
		else variant = std::monostate();
	}
};

class AddInNative : public IComponentBase
{
protected:
	class VarinantHelper {
	private:
		tVariant* pvar = nullptr;
		AddInNative* addin = nullptr;
	public:
		VarinantHelper(const VarinantHelper& va) :pvar(va.pvar), addin(va.addin) {}
		VarinantHelper(tVariant* pvar, AddInNative* addin) :pvar(pvar), addin(addin) {}
		VarinantHelper& operator=(const VarinantHelper& va) { pvar = va.pvar; addin = va.addin; return *this; }
		VarinantHelper& operator<<(const std::string& str);
		VarinantHelper& operator<<(const std::wstring& str);
		VarinantHelper& operator<<(const std::u16string& str);
		VarinantHelper& operator<<(int32_t value);
		VarinantHelper& operator<<(int64_t value);
		VarinantHelper& operator<<(bool value);
		operator std::string() const;
		operator std::wstring() const;
		operator std::u16string() const;
		operator int32_t() const;
		operator bool() const;
		void clear();
	};

	using VH = VarinantHelper;
	using MethDefaults = std::map<int, DefaultHelper>;
	using PropFunction = std::function<void(VH)>;

	using MethFunction0 = std::function<void()>;
	using MethFunction1 = std::function<void(VH)>;
	using MethFunction2 = std::function<void(VH, VH)>;
	using MethFunction3 = std::function<void(VH, VH, VH)>;
	using MethFunction4 = std::function<void(VH, VH, VH, VH)>;
	using MethFunction5 = std::function<void(VH, VH, VH, VH, VH)>;

	using MethFunction = std::variant<
		MethFunction0,
		MethFunction1,
		MethFunction2,
		MethFunction3,
		MethFunction4,
		MethFunction5
	>;

	bool ADDIN_API AllocMemory(void** pMemory, unsigned long ulCountByte) const;
	void ADDIN_API FreeMemory(void** pMemory) const;
	void AddProperty(const std::u16string& nameEn, const std::u16string& nameRu, PropFunction getter, PropFunction setter = nullptr);
	void AddProcedure(const std::u16string& nameEn, const std::u16string& nameRu, MethFunction handler, MethDefaults defs = {});
	void AddFunction(const std::u16string& nameEn, const std::u16string& nameRu, MethFunction handler, MethDefaults defs = {});
	static std::u16string AddComponent(const std::u16string& name, CompFunction creator);
	VarinantHelper result;

	VarinantHelper variant(tVariant* pvar) { return VarinantHelper(pvar, this); }
	static std::u16string AddInNative::upper(std::u16string& str);
	static std::wstring AddInNative::upper(std::wstring& str);
	static std::string WCHAR2MB(std::basic_string_view<WCHAR_T> src);
	static std::wstring WCHAR2WC(std::basic_string_view<WCHAR_T> src);
	static std::u16string MB2WCHAR(std::string_view src);
	WCHAR_T* W(const char16_t* str) const;

private:
	class Prop {
	public:
		std::vector<std::u16string> names;
		PropFunction getter;
		PropFunction setter;
	};

	struct Meth {
		std::vector<std::u16string> names;
		MethFunction handler;
		MethDefaults defs;
		bool hasRetVal;
	};

	bool CallMethod(MethFunction* function, tVariant* paParams, const long lSizeArray);

	static std::map<std::u16string, CompFunction> components;
	std::vector<Prop> properties;
	std::vector<Meth> methods;
	std::u16string name;

public:
	static std::u16string AddInNative::getComponentNames();
	static AddInNative* createObject(const std::u16string& name);

public:
	AddInNative(void) : result(nullptr, this) {}
	virtual ~AddInNative() {}
	// IInitDoneBase
	virtual bool ADDIN_API Init(void*);
	virtual bool ADDIN_API setMemManager(void* mem);
	virtual long ADDIN_API GetInfo();
	virtual void ADDIN_API Done();
	// ILanguageExtenderBase
	virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T** wsLanguageExt);
	virtual long ADDIN_API GetNProps();
	virtual long ADDIN_API FindProp(const WCHAR_T* wsPropName);
	virtual const WCHAR_T* ADDIN_API GetPropName(long lPropNum, long lPropAlias);
	virtual bool ADDIN_API GetPropVal(const long lPropNum, tVariant* pvarPropVal);
	virtual bool ADDIN_API SetPropVal(const long lPropNum, tVariant* pvarPropVal);
	virtual bool ADDIN_API IsPropReadable(const long lPropNum);
	virtual bool ADDIN_API IsPropWritable(const long lPropNum);
	virtual long ADDIN_API GetNMethods();
	virtual long ADDIN_API FindMethod(const WCHAR_T* wsMethodName);
	virtual const WCHAR_T* ADDIN_API GetMethodName(const long lMethodNum, const long lMethodAlias);
	virtual long ADDIN_API GetNParams(const long lMethodNum);
	virtual bool ADDIN_API GetParamDefValue(const long lMethodNum, const long lParamNum, tVariant* pvarParamDefValue);
	virtual bool ADDIN_API HasRetVal(const long lMethodNum);
	virtual bool ADDIN_API CallAsProc(const long lMethodNum, tVariant* paParams, const long lSizeArray);
	virtual bool ADDIN_API CallAsFunc(const long lMethodNum, tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray);
	operator IComponentBase* () { return (IComponentBase*)this; }
	// LocaleBase
	virtual void ADDIN_API SetLocale(const WCHAR_T* loc);
private:
	IMemoryManager* m_iMemory = nullptr;
	IAddInDefBase* m_iConnect = nullptr;
};
#endif //__ADDINNATIVE_H__
