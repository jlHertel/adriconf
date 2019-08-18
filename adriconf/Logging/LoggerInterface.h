#ifndef ADRICONF_LOGGERINTERFACE_H
#define ADRICONF_LOGGERINTERFACE_H

#include <glibmm/ustring.h>

class LoggerInterface {
public:

    virtual ~LoggerInterface(){}

    /**
     * Logs a debug message
     * @param msg
     */
    virtual void debug(Glib::ustring msg) = 0;

    /**
     * Logs a info message
     * @param msg
     */
    virtual void info(Glib::ustring msg) = 0;

    /**
     * Logs a warning message
     * @param msg
     */
    virtual void warning(Glib::ustring msg) = 0;

    /**
     * Logs an error message
     * @param msg
     */
    virtual void error(Glib::ustring msg) = 0;
};

#endif //ADRICONF_LOGGERINTERFACE_H