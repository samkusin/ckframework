
#ifndef CINEK_ENTITY_TEST_COMPONENT_HPP
#define CINEK_ENTITY_TEST_COMPONENT_HPP

#include "ckentity/entity.h"

struct TestComponent
{
    COMPONENT_DEFINITION(TestComponent);

    int propInt;
    float propFloat;
    bool propBool;

    char propString[32];
};

#endif
