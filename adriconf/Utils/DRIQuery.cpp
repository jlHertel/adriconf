#include <glibmm/i18n.h>
#include <string>

#include <stdexcept>

#include "DRIQuery.h"
#include "PCIDatabaseQuery.h"

#include <EGL/egl.h>

DRIQuery::DRIQuery(
        LoggerInterface *logger,
        TranslatorInterface *translator,
        ParserInterface *parser,
        PCIDatabaseQueryInterface *pciQuery,
        DRMDeviceFactoryInterface *drmDeviceFactory,
        GBMDeviceFactoryInterface *gbmDeviceFactory,
        EGLDisplayFactoryInterface *eglWrapper
) : logger(logger),
    translator(translator),
    parser(parser),
    pciQuery(pciQuery),
    drmDeviceFactory(drmDeviceFactory),
    gbmDeviceFactory(gbmDeviceFactory),
    eglDisplayFactory(eglWrapper) {}

std::map<Glib::ustring, GPUInfo_ptr> DRIQuery::enumerateDRIDevices(const Glib::ustring &locale) {
    this->logger->debug(this->translator->trns("Enumerating DRI Devices"));
    std::map<Glib::ustring, GPUInfo_ptr> gpus;

    auto drmDevices = this->drmDeviceFactory->getDevices();
    this->logger->debug(
            Glib::ustring::compose(this->translator->trns("Found %1 devices"), drmDevices.size())
    );

    int i = 0;
    for (auto &device : drmDevices) {
        i++;
        try {
            GPUInfo_ptr gpu = std::make_shared<GPUInfo>();

            gpu->setPciId(device->getFormattedPCIId());
            this->logger->debug(
                    Glib::ustring::compose(
                            this->translator->trns("Processing GPU with PCI ID \"%1\""),
                            gpu->getPciId()
                    )
            );
            gpu->setVendorId(device->getVendorPCIId());
            gpu->setDeviceId(device->getDevicePCIId());

            gpu->setVendorName(this->pciQuery->queryVendorName(gpu->getVendorId()));
            gpu->setDeviceName(this->pciQuery->queryDeviceName(gpu->getVendorId(), gpu->getDeviceId()));

            this->logger->debug(
                    Glib::ustring::compose(
                            this->translator->trns("GPU has been detected as \"%1\" from \"%2\""),
                            gpu->getDeviceName(),
                            gpu->getVendorName()
                    )
            );


            this->logger->debug(
                    Glib::ustring::compose(
                            this->translator->trns("Generating gbm device for path \"%1\""),
                            device->getDeviceRenderNodeName()
                    )
            );

            GBMDevice gbmDevice = this->gbmDeviceFactory->generateDeviceFromPath(device->getDeviceRenderNodeName());

            this->logger->debug(
                    Glib::ustring::compose(
                            this->translator->trns("Generating EGL Display from GBM device for \"%1\""),
                            device->getDeviceRenderNodeName()
                    )
            );

            auto display = this->eglDisplayFactory->createDisplayFromGBM(gbmDevice);
            gpu->setDriverName(display->getDriverName());
            this->logger->debug(
                    Glib::ustring::compose(
                            this->translator->trns("GPU \"%1\" has the driver \"%2\""),
                            i,
                            gpu->getDriverName()
                    )
            );

            this->logger->debug(this->translator->trns("Loading driver options"));
            const char *driverOptions = display->getDriverOptions();

            // If for some reason mesa is unable to query the options we simply skip this gpu
            if (driverOptions == nullptr) {
                this->logger->error(
                        Glib::ustring::compose(
                                this->translator->trns("Unable to extract configuration for driver %1"),
                                gpu->getDriverName()
                        )
                );

                continue;
            }

            Glib::ustring options(driverOptions);
            if (options.empty()) {
                this->logger->error(
                        Glib::ustring::compose(
                                this->translator->trns("Unable to extract configuration for driver %1"),
                                gpu->getDriverName()
                        )
                );

                continue;
            }

            auto parsedSections = this->parser->parseAvailableConfiguration(options, locale);
            gpu->setSections(parsedSections);

            gpus[gpu->getPciId()] = gpu;
        } catch (std::runtime_error &ex) {
            this->logger->error(
                    Glib::ustring::compose(
                            this->translator->trns("Skipping device %1 due to error: %2"),
                            i,
                            ex.what()
                    )
            );
        }
    }

    return gpus;
}