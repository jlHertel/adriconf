#ifndef ADRICONF_LOGGERMOCK_H
#define ADRICONF_LOGGERMOCK_H

#include "gmock/gmock.h"
#include "../../adriconf/Logging/LoggerInterface.h"

class LoggerMock : public LoggerInterface {
public:
    MOCK_METHOD1(debug, void(Glib::ustring));
    MOCK_METHOD1(info, void(Glib::ustring));
    MOCK_METHOD1(warning, void(Glib::ustring));
    MOCK_METHOD1(error, void(Glib::ustring));
};

#endif //ADRICONF_LOGGERMOCK_H