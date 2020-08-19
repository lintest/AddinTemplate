#include "TestComponent.h"

std::vector<std::u16string> TestComponent::names = {
	AddComponent(u"AddInNative", TestComponent::create)
};

TestComponent::TestComponent()
{
	AddProperty({ u"Test", u"Тест" }, nullptr, nullptr);
}

bool TestComponent::getTestString(tVariant* pvarPropVal)
{
	return VA(pvarPropVal) << std::u16string(u"Test component");
}
