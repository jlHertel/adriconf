#include "Section.h"

DRI::Section::Section() : options() {}

Glib::ustring DRI::Section::getDescription() {
    return this->description;
}

std::list<DRI::Option > DRI::Section::getOptions() {
    return this->options;
}

DRI::Section *DRI::Section::setDescription(Glib::ustring description) {
    this->description = std::move(description);

    return this;
}

DRI::Section *DRI::Section::addOption(DRI::Option option) {
    this->options.push_back(option);

    return this;
}