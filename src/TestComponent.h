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
    bool getTestString(tVariant* pvar);
};
#endif //__TESTCOMPONENT_H__