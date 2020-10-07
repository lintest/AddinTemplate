#include "TestComponent.h"

std::vector<std::u16string> TestComponent::names = {
	AddComponent(u"AddInNative", []() { return new TestComponent; }),
	AddComponent(u"SimpleAlias", []() { return new TestComponent; }),
};

TestComponent::TestComponent()
{
	AddProperty(u"Text", u"Текст",
		[&](VH var) { var = this->getTestString(); },
		[&](VH var) { this->setTestString(var); }
	);

	AddProperty(u"Number", u"Число",
		[&](VH var) { var = this->value; },
		[&](VH var) { this->value = var; }
	);

	AddFunction(u"GetText", u"ПолучитьТекст", [&]() { this->result = this->getTestString(); });

	AddProcedure(u"SetText", u"УстановитьТекст", [&](VH par) { this->setTestString(par); }, { {0, u"default: "} });
}

#include <iostream>
#include <ctime>

std::u16string TestComponent::getTestString()
{
	time_t rawtime;
	struct tm timeinfo;
	char buffer[255];
	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);
	strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", &timeinfo);
	return text + MB2WCHAR(buffer);
}

void TestComponent::setTestString(const std::u16string& text)
{
	this->text = text;
}

