#ifndef ADRICONF_COMBOBOXCOLUMN_H
#define ADRICONF_COMBOBOXCOLUMN_H

#include <gtkmm/liststore.h>

class ComboBoxColumn : public Gtk::TreeModel::ColumnRecord {
public:

    ComboBoxColumn() {
        add(optionName);
        add(optionValue);
    }

    Gtk::TreeModelColumn<Glib::ustring> optionName;
    Gtk::TreeModelColumn<Glib::ustring> optionValue;
};

#endif
