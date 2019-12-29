#ifndef ADRICONF_TRANSLATORINTERFACE_H
#define ADRICONF_TRANSLATORINTERFACE_H

#include <glibmm/ustring.h>

class TranslatorInterface {
public:
    virtual char *trns(const char *text) = 0;
};


#endif //ADRICONF_TRANSLATORINTERFACE_H
