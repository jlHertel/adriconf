#ifndef ADRICONF_LOGGERMOCK_H
#define ADRICONF_LOGGERMOCK_H

#include "gmock/gmock.h"
#include "../../adriconf/Logging/LoggerInterface.h"

class LoggerMock : public LoggerInterface {
public:
    MOCK_METHOD(void, debug, (Glib::ustring), (override));
    MOCK_METHOD(void, info, (Glib::ustring), (override));
    MOCK_METHOD(void, warning, (Glib::ustring), (override));
    MOCK_METHOD(void, error, (Glib::ustring), (override));
};

#endif //ADRICONF_LOGGERMOCK_H