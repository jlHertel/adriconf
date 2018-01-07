#include "Parser.h"

std::list<DRI::Section>
DRI::Parser::parseAvailableConfiguration(const Glib::ustring &xml, const Glib::ustring &currentLocale) {
    std::list<DRI::Section> availableSections;
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
                DRI::Section confSection;


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
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    }

    return availableSections;
}

DRI::DriverOption DRI::Parser::parseSectionOptions(xmlpp::Node *option, const Glib::ustring &currentLocale) {
    DRI::DriverOption parsedOption;

    auto optionElement = dynamic_cast<xmlpp::Element *>(option);

    parsedOption.setName(optionElement->get_attribute("name")->get_value());
    parsedOption.setType(optionElement->get_attribute("type")->get_value());

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

    if (parsedOption.getType() == "enum") {
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

    return parsedOption;
}

std::list<std::shared_ptr<DRI::Device>> DRI::Parser::parseDevices(Glib::ustring &xml) {
    std::list<std::shared_ptr<DRI::Device>> deviceList;

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
                auto deviceConf = std::make_shared<DRI::Device>();

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
                    /* If an application doesn't have any option, why include it? */
                    if (!parsedApp->getOptions().empty()) {
                        deviceConf->addApplication(parsedApp);
                    }
                }

                deviceList.emplace_back(deviceConf);
            }
        }
    } catch (const std::exception &ex) {
        std::cerr << "Exception caught: " << ex.what() << std::endl;
    }

    return deviceList;
}

std::shared_ptr<DRI::Application> DRI::Parser::parseApplication(xmlpp::Node *application) {
    auto app = std::make_shared<DRI::Application>();

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
            auto newOption = std::make_shared<DRI::ApplicationOption>();
            newOption->setName(optionName->get_value());
            newOption->setValue(optionValue->get_value());

            app->addOption(newOption);
        }
    }

    return app;
}

std::list<Glib::ustring> DRI::Parser::convertSectionsToOptions(const std::list<DRI::Section> &sections) {
    std::list<Glib::ustring> options;

    for (auto &section : sections) {
        for (const auto &option : section.getOptions()) {
            options.emplace_back(option.getName());
        }
    }

    return options;
}

std::list<DRI::DriverOption> DRI::Parser::convertSectionsToOptionsObject(const std::list<DRI::Section> &sections) {
    std::list<DRI::DriverOption> options;

    for (const auto &section : sections) {
        for (const auto &option : section.getOptions()) {
            options.emplace_back(option);
        }
    }

    return options;
}