#pragma once

#include "AddInNative.h"

class TestComponent:
    public AddInNative
{
private:
    static std::vector<std::u16string> names;
    TestComponent();
private:
    bool getTestString(tVariant* pvar);
};
