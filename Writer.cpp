#include "Writer.h"
#include <libxml++/libxml++.h>

Glib::ustring DRI::Writer::generateRawXml(std::list<DRI::Device> devices) {
    Glib::ustring output("<driconf>\n");

    for (auto &device : devices) {
        output.append("  <device screen=\"");
        output.append(std::to_string(device.getScreen()));
        output.append("\" driver=\"");
        output.append(device.getDriver());
        output.append("\">\n");

        for (auto &app : device.getApplications()) {
            output.append("    <application");
            if(!app.getName().empty()) {
                output.append(" name=\"");
                /* TODO: Check if we need to make a special escaping here */
                output.append(app.getName());
                output.append("\"");
            }

            if(!app.getExecutable().empty()) {
                /* TODO: Check if we need to scape this too. */
                output.append(" executable=\"");
                output.append(app.getExecutable());
                output.append("\"");
            }

            output.append(">\n");

            for(auto &option : app.getOptions()) {
                output.append("      <option name=\"");
                output.append(option.first);
                output.append("\" value=\"");
                output.append(option.second);
                output.append("\" />\n");
            }

            output.append("    </application>\n");
        }

        output.append("  </device>\n");
    }

    output.append("</driconf>");

    return output;
}