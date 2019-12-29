#include "GetTextTranslator.h"

#include <glibmm/i18n.h>

const char * GetTextTranslator::trns(const char *text) {
    return gettext(text);
}
