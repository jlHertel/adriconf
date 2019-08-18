#ifndef ADRICONF_LOGGER_H
#define ADRICONF_LOGGER_H

#include "LoggerInterface.h"
#include "LoggerLevel.h"

class Logger : public LoggerInterface {
private:
    LoggerLevel level;

public:
    virtual ~Logger(){};

    Logger();

    virtual void debug(Glib::ustring msg);
    virtual void info(Glib::ustring msg);
    virtual void warning(Glib::ustring msg);
    virtual void error(Glib::ustring msg);

    void setLevel(LoggerLevel level);
};

#endif //ADRICONF_LOGGER_H