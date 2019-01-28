# Advanced DRI Configurator

adriconf (**A**dvanced **DRI** **CONF**igurator) is a GUI tool used to configure
open source graphics drivers. It works by setting options and writing them to
the standard `drirc` file used by the Mesa drivers.

## Features

The main features of the tool are:

- Automatic removal of invalid and unsupported options.
- Options whose value is identical to the system-wide value or the driver's
  default value will be ignored.
- System-wide application options with redundant options will be
  removed automatically.

## Building

To build from source you can use the following commands:

    git clone https://github.com/jlHertel/adriconf.git
    cd adriconf
    mkdir build-dir
    cd build-dir
    cmake ..
    make
    sudo make install

## Translating

To add a new language or to improve a existing one, you can edit the po files
located under po/ dir.
To add a new translation use the following command (for example for German):

    msginit -i adriconf.pot -o de.po --locale=de

To update an existing translation from the pot file you can use the folowing:

    mv de.po de.po~
    msgmerge -o de.po de.po~ adriconf.pot

To update the pot file itself you can run the following:

    intltool-update --pot --gettext-package=adriconf

Please note that many text shown in the application are from mesa directly,
mainly the option descriptions. Therefore if you see any missing translation
you will need to add it to the [mesa](https://www.mesa3d.org/) project itself.

## Author

This tool is written and maintained by Jean Lorenz Hertel.
To see a full list of contributors check the AUTHORS file included in the source
code.

## Contributing

Check our CONTRIBUTING file for further details on how to contribute.

## License

Copyright (C) 2018 Jean Lorenz Hertel

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
