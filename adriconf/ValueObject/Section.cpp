#include "Section.h"

Section::Section() : options() {}

const Glib::ustring &Section::getDescription() const {
    return this->description;
}

const std::list<DriverOption> &Section::getOptions() const {
    return this->options;
}

Section *Section::setDescription(Glib::ustring description) {
    this->description = std::move(description);

    return this;
}

Section *Section::addOption(DriverOption option) {
    this->options.push_back(option);

    return this;
}

void Section::sortOptions() {
    this->options.sort([](const DriverOption &a, const DriverOption &b) {
        return a.getSortValue() < b.getSortValue();
    });
}
