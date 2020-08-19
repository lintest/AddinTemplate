#pragma once

#include "AddInNative.h"

class TestComponent:
    public AddInNative
{
public:
    static AddInNative* create() {
        return new TestComponent();
    }
private:
    static std::vector<std::u16string> names;
    TestComponent();
private:
    bool getTestString(tVariant* pvarPropVal);
};

