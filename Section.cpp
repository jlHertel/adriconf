#include "Section.h"

DRI::Section::Section() : options() {}

const Glib::ustring &DRI::Section::getDescription() const {
    return this->description;
}

const std::list<DRI::DriverOption> &DRI::Section::getOptions() const {
    return this->options;
}

DRI::Section *DRI::Section::setDescription(Glib::ustring description) {
    this->description = std::move(description);

    return this;
}

DRI::Section *DRI::Section::addOption(DRI::DriverOption option) {
    this->options.push_back(option);

    return this;
}