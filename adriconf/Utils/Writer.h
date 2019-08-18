#ifndef SIMECONF_WRITER_H
#define SIMECONF_WRITER_H

#include <list>
#include <glibmm/ustring.h>
#include "../ValueObject/Device.h"

namespace Writer {
    Glib::ustring generateRawXml(const std::list<Device_ptr> &devices);
}

#endif
