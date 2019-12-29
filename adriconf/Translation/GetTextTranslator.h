#ifndef ADRICONF_TRANSLATOR_H
#define ADRICONF_TRANSLATOR_H

#include "TranslatorInterface.h"

class GetTextTranslator : public TranslatorInterface {
public:
    const char * trns(const char *text) override;
};


#endif //ADRICONF_TRANSLATOR_H
