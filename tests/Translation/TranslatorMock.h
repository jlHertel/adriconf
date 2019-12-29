#ifndef ADRICONF_TRANSLATORMOCK_H
#define ADRICONF_TRANSLATORMOCK_H

#include "gmock/gmock.h"
#include "../../adriconf/Translation/TranslatorInterface.h"

class TranslatorMock : public TranslatorInterface {
public:
    MOCK_METHOD(const char *, trns, (const char *text), (override));
};

#endif //ADRICONF_TRANSLATORMOCK_H