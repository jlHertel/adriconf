#include <iostream>
#include "Logger.h"

Logger::Logger() : level(LoggerLevel::INFO) {}

void Logger::debug(Glib::ustring msg) {
    if (this->level <= LoggerLevel::DEBUG) {
        std::cout << "DEBUG: " << msg << std::endl;
    }
}

void Logger::info(Glib::ustring msg) {
    if (this->level <= LoggerLevel::INFO) {
        std::cout << "INFO: " << msg << std::endl;
    }
}

void Logger::warning(Glib::ustring msg) {
    if (this->level <= LoggerLevel::WARNING) {
        std::cout << "WARNING: " << msg << std::endl;
    }
}

void Logger::error(Glib::ustring msg) {
    if (this->level <= LoggerLevel::ERROR) {
        std::cerr << "ERROR: " << msg << std::endl;
    }
}

void Logger::setLevel(LoggerLevel level) {
    Logger::level = level;
}
