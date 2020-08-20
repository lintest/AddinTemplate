#ifndef __TESTCOMPONENT_H__
#define __TESTCOMPONENT_H__

#include "AddInNative.h"

class TestComponent:
    public AddInNative
{
private:
    static std::vector<std::u16string> names;
    TestComponent();
private:
    std::u16string text;
    std::u16string getTestString();
    void setTestString(const std::u16string &text);
};
#endif //__TESTCOMPONENT_H__