# AddinTemplate - шаблон внешней компоненты 1С

Для облегчения работы с шаблоном весь программный код, который не должен 
изменяться разработчиком при реализации собственных библиотек внешних 
компонент, собран в файле **AddInNative.cpp**.

Автор предлагает собственную оригинальную реализацию, которая хотя и лишена 
изящества шаблона [Infactum](https://github.com/Infactum/addin-template),
тоже позволяет регистрировать компоненту под несколькими именами, 
использет лямбда-выражения для регистрации компоненты в библиотеке, при определении 
свойств, методов. При обращении к свойствам и методам регистр игнорируется.

```Cpp
std::vector<std::u16string> TestComponent::names = {
	AddComponent(u"AddInNative", []() { return new TestComponent; }),
	AddComponent(u"SimpleAlias", []() { return new TestComponent; }),
};

TestComponent::TestComponent()
{
	AddProperty(u"Text", u"Текст",
		[&](VH var) { var << this->getTestString(); },
		[&](VH var) { this->setTestString(var); }
	);

	AddFunction(u"GetText", u"ПолучитьТекст", 
		[&]() { this->result << this->getTestString(); }
	);

	AddProcedure(u"SetText", u"УстановитьТекст", 
		[&](VH par) { this->setTestString(par); }, 
		{ {0, u"default: "} }
	);
}

```
