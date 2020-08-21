# AddinTemplate - шаблон внешней компоненты 1С

Для облегчения работы с шаблоном весь программный код, который не должен 
изменяться разработчиком при реализации собственных библиотек внешних 
компонент, собран в файлах **AddInNative.cpp** и **AddInNative.h**.

Автор предлагает собственную оригинальную реализацию, которая хотя и лишена 
изящества шаблона [Infactum](https://github.com/Infactum/addin-template),
тоже позволяет регистрировать компоненту под несколькими именами, 
использует лямбда-выражения для регистрации компоненты в библиотеке, при определении 
свойств и методов. При обращении к свойствам и методам регистр игнорируется.

Отказ от использования шаблонов для определения процедур и функций в предлагаемой 
реализации компоненты накладывает существенные ограничения на формат лямбда выражений 
в методах **AddProperty**, **AddProcedure** и **AddFunction**, поскольку требует 
обязательного использования агрументов типа **VH** (короткий синоним **VariantHelper**).
Это позволяет легко решить задачу возврата в 1С измененных значений параметров.
Для возврата результата функции используется член базового класса **result**.

Пример кода для регистрации компоненты в библиотеке:
```Cpp
std::vector<std::u16string> TestComponent::names = {
	AddComponent(u"AddInNative", []() { return new TestComponent; }),
	AddComponent(u"SimpleAlias", []() { return new TestComponent; }),
};

TestComponent::TestComponent()
{
	AddProperty(u"Text", u"Текст",
		[&](VH prop) { prop = this->getTestString(); },
		[&](VH prop) { this->setTestString(prop); }
	);

	AddFunction(u"GetText", u"ПолучитьТекст", 
		[&]() { this->result = this->getTestString(); }
	);

	AddProcedure(u"SetText", u"УстановитьТекст", 
		[&](VH param) { this->setTestString(param); }, 
		{ {0, u"Default"} }
	);
}

```
