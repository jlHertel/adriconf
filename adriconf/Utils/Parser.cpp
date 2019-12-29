#include "Parser.h"
#include <glibmm/i18n.h>

std::list<Section>
Parser::parseAvailableConfiguration(const Glib::ustring &xml, const Glib::ustring &currentLocale) {
    this->logger->debug(Glib::ustring::compose(this->translator->trns("Parsing XML %1"), xml));
    std::list<Section> availableSections;
    try {
        xmlpp::DomParser parser;
        parser.set_throw_messages(true);
        parser.set_substitute_entities(true);
        parser.set_include_default_attributes(true);
        parser.parse_memory(xml);

        if (parser) {
            auto rootNode = parser.get_document()->get_root_node();

            auto sections = rootNode->get_children("section");
            for (auto section : sections) {
                Section confSection;


                auto descriptions = section->get_children("description");
                Glib::ustring englishName;
                Glib::ustring localizedName;

                for (auto description : descriptions) {
                    auto descriptionElement = dynamic_cast<xmlpp::Element *>(description);
                    Glib::ustring lang(descriptionElement->get_attribute("lang")->get_value());
                    Glib::ustring value(descriptionElement->get_attribute("text")->get_value());

                    if (lang == "en") {
                        englishName = value;
                    }

                    if (lang == currentLocale) {
                        localizedName = value;
                    }
                }

                confSection.setDescription(localizedName.empty() ? englishName : localizedName);

                auto options = section->get_children("option");

                for (auto option : options) {
                    auto parsedOption = parseSectionOptions(option, currentLocale);

                    confSection.addOption(parsedOption);
                }

                availableSections.push_back(confSection);
            }
        }

    } catch (const std::exception &ex) {
        this->logger->error(Glib::ustring::compose("LibXML exception caught: %1", ex.what()));
        this->logger->error(Glib::ustring::compose("XML Parsed: %1", xml));
    }

    return availableSections;
}

DriverOption Parser::parseSectionOptions(xmlpp::Node *option, const Glib::ustring &currentLocale) {
    DriverOption parsedOption;

    auto optionElement = dynamic_cast<xmlpp::Element *>(option);

    parsedOption.setName(optionElement->get_attribute("name")->get_value());
    Glib::ustring type = optionElement->get_attribute("type")->get_value();
    parsedOption.setType(parsedOption.stringToEnum(type));

    auto defaultValue = optionElement->get_attribute("default");
    if (defaultValue != nullptr) {
        parsedOption.setDefaultValue(defaultValue->get_value());
    }

    auto validValues = optionElement->get_attribute("valid");
    if (validValues != nullptr) {
        parsedOption.setValidValues(validValues->get_value());
    }

    auto descriptions = option->get_children("description");

    xmlpp::Node *descriptionHolder = nullptr;
    Glib::ustring correctDescription;

    for (auto description : descriptions) {
        auto descriptionElement = dynamic_cast<xmlpp::Element *>(description);
        Glib::ustring lang(descriptionElement->get_attribute("lang")->get_value());
        Glib::ustring value(descriptionElement->get_attribute("text")->get_value());

        if (lang == "en") {
            if (correctDescription.empty()) {
                correctDescription = value;
                descriptionHolder = description;
            }
        }

        if (lang == currentLocale) {
            correctDescription = value;
            descriptionHolder = description;
        }
    }

    parsedOption.setDescription(correctDescription);

    if (parsedOption.getType() == DriverOptionType::ENUM) {
        if (descriptionHolder != nullptr) {
            auto enumOptions = descriptionHolder->get_children("enum");
            for (auto enumOption : enumOptions) {
                auto enumElement = dynamic_cast<xmlpp::Element *>(enumOption);
                Glib::ustring value(enumElement->get_attribute("value")->get_value());
                Glib::ustring text(enumElement->get_attribute("text")->get_value());

                parsedOption.addEnumValue(text, value);
            }
        }
    }

    parsedOption.updateFakeBool();

    return parsedOption;
}

std::list<Device_ptr> Parser::parseDevices(Glib::ustring &xml) {
    std::list<Device_ptr> deviceList;

    this->logger->debug(Glib::ustring::compose(this->translator->trns("Parsing device for XML: %1"), xml));
    try {
        xmlpp::DomParser parser;
        parser.set_throw_messages(true);
        parser.set_substitute_entities(true);
        parser.set_include_default_attributes(true);
        parser.parse_memory(xml);

        if (parser) {
            auto rootNode = parser.get_document()->get_root_node();

            auto devices = rootNode->get_children("device");
            for (auto device : devices) {
                auto deviceConf = std::make_shared<Device>();

                auto deviceElement = dynamic_cast<xmlpp::Element *>(device);
                auto deviceScreen = deviceElement->get_attribute("screen");
                if (deviceScreen != nullptr) {
                    deviceConf->setScreen(std::stoi(deviceScreen->get_value()));
                }

                auto deviceDriver = deviceElement->get_attribute("driver");
                if (deviceDriver != nullptr) {
                    deviceConf->setDriver(deviceDriver->get_value());
                }

                auto applications = device->get_children("application");

                for (auto application : applications) {
                    auto parsedApp = parseApplication(application);
                    deviceConf->addApplication(parsedApp);
                }

                deviceList.emplace_back(deviceConf);
            }
        }
    } catch (const std::exception &ex) {
        this->logger->error(
                Glib::ustring::compose(
                        this->translator->trns("Exception caught during device XML parsing: %1"),
                        ex.what()
                )
        );
    }

    return deviceList;
}

Application_ptr Parser::parseApplication(xmlpp::Node *application) {
    auto app = std::make_shared<Application>();

    auto applicationElement = dynamic_cast<xmlpp::Element *>(application);

    auto applicationName = applicationElement->get_attribute("name");
    if (applicationName != nullptr) {
        app->setName(applicationName->get_value());
    }

    auto applicationExecutable = applicationElement->get_attribute("executable");
    if (applicationExecutable != nullptr) {
        app->setExecutable(applicationExecutable->get_value());
    }

    auto options = application->get_children("option");

    for (auto option : options) {
        auto optionElement = dynamic_cast<xmlpp::Element *>(option);
        auto optionName = optionElement->get_attribute("name");
        auto optionValue = optionElement->get_attribute("value");
        if (optionName != nullptr && optionValue != nullptr) {
            auto newOption = std::make_shared<ApplicationOption>();
            newOption->setName(optionName->get_value());
            newOption->setValue(optionValue->get_value());

            app->addOption(newOption);
        }
    }

    return app;
}