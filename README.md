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

## TODOs

Some things that still need to be done:

- Properly support systems that do not have X installed
  (such as some Wayland systems). Currently, the GLX functions mandate
  a Xlib display object; there must be another way to get the driver options.
- Unit tests are always welcome. If you can contribute some tests,
  I will gladly accept them.
- Usability improvements for the GUI are always welcome.
- Additional language translations are always welcome too.
  Please keep in mind that option descriptions come directly from the driver;
  therefore, you need to translate them directly on
  [Mesa](https://www.mesa3d.org/).

## Author

This tool is written and maintained by Jean Lorenz Hertel.

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
