#ifndef SIMECONF_WRITER_H
#define SIMECONF_WRITER_H

#include <list>
#include <glibmm/ustring.h>
#include "Device.h"

namespace DRI::Writer {
    Glib::ustring generateRawXml(std::list<DRI::Device> devices);
}

#endif
