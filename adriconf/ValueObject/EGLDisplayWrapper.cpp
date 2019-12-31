#include "EGLDisplayWrapper.h"
#include <glibmm/i18n.h>

EGLDisplayWrapper::EGLDisplayWrapper(TranslatorInterface *translator)
        : rawDisplay(), minorVersion(0), majorVersion(0), extensions(), translator(translator) {
    this->queryDriverName = (const char *(*)(EGLDisplay)) eglGetProcAddress("eglGetDisplayDriverName");
    this->queryDriverOptions = (const char *(*)(EGLDisplay)) eglGetProcAddress("eglGetDisplayDriverConfig");
}

EGLDisplayWrapper::~EGLDisplayWrapper() {
    eglTerminate(this->rawDisplay);
}

void EGLDisplayWrapper::setRawDisplay(EGLDisplay display) {
    this->rawDisplay = display;
}

void EGLDisplayWrapper::setMinorVersion(EGLint minor) {
    this->minorVersion = minor;
}

void EGLDisplayWrapper::setMajorVersion(EGLint major) {
    this->majorVersion = major;
}

void EGLDisplayWrapper::setExtensions(const Glib::ustring &exts) {
    this->extensions = exts;
}

bool EGLDisplayWrapper::hasMesaQueryDriverExtension() const {
    std::size_t found = this->extensions.find("EGL_MESA_query_driver");
    return found != std::string::npos;
}

const char *EGLDisplayWrapper::getDriverName() {
    if (!this->hasMesaQueryDriverExtension()) {
        throw std::runtime_error(this->translator->trns("Display is missing EGL_MESA_query_driver extension"));
    }

    return this->queryDriverName(this->rawDisplay);
}

const char *EGLDisplayWrapper::getDriverOptions() {
    if (!this->hasMesaQueryDriverExtension()) {
        throw std::runtime_error(this->translator->trns("Display is missing EGL_MESA_query_driver extension"));
    }

    return this->queryDriverOptions(this->rawDisplay);
}

Glib::ustring EGLDisplayWrapper::getExtensions() const {
    return this->extensions;
}

