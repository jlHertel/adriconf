#ifndef SIMECONF_WRITER_H
#define SIMECONF_WRITER_H

#include <list>
#include <glibmm/ustring.h>
#include "../ValueObject/Device.h"
#include "WriterInterface.h"

class Writer : public WriterInterface {
public:
    ~Writer() override = default;

    Glib::ustring generateRawXml(const std::list<Device_ptr> &devices) override;
};

#endif