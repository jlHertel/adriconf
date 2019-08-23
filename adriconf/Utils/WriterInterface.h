#ifndef ADRICONF_WRITERINTERFACE_H
#define ADRICONF_WRITERINTERFACE_H

#include <list>
#include <glibmm/ustring.h>
#include "../ValueObject/Device.h"

class WriterInterface {
public:
    virtual ~WriterInterface() = default;
    virtual Glib::ustring generateRawXml(const std::list<Device_ptr> &devices) = 0;
};

#endif //ADRICONF_WRITERINTERFACE_H