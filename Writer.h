#ifndef SIMECONF_WRITER_H
#define SIMECONF_WRITER_H

#include <list>
#include <glibmm/ustring.h>
#include "Device.h"

namespace DRI::Writer {
    Glib::ustring generateRawXml(const std::list<std::shared_ptr<DRI::Device>> &devices);
}

#endif
