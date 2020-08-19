#include "TestComponent.h"

std::vector<std::u16string> TestComponent::names = {
	AddComponent(u"AddInNative", TestComponent::create)
};

TestComponent::TestComponent()
{
	AddProperty({ u"Test", u"Тест" }
		, [&](tVariant* pvar) { return this->getTestString(pvar); }
		, nullptr
	);
}

bool TestComponent::getTestString(tVariant* pvar)
{
	return VA(pvar) << std::u16string(u"Test component");
}
