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

    void debug(Glib::ustring msg) override;
    void info(Glib::ustring msg) override;
    void warning(Glib::ustring msg) override;
    void error(Glib::ustring msg) override;

    void setLevel(LoggerLevel level);
};

#endif //ADRICONF_LOGGER_H