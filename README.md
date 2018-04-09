Advanced DRI Configurator
=========================

The adriconf (**A**dvanced **DRI** **CONF**igurator) is a tool to set options and configure applications using the standard drirc file used by the Mesa drivers.
In other words, It's a GUI tool used to configure the Open Source Driver of the Grapics Cards.

Features
--------

The main features of the tool include:

- Automatic removal of invalid options (Options that the driver doesn't support at all)
- Options that have the same value as the system wide options or driver default will be ignored
- System-Wide Applications with empty options (all options are the same as system-wide config or driver default) will be removed automatically


TODOs
-----

Some things that still need to be done:

- Properly support a system without X (wayland systems?) Currently the glX functions mandate a Xlib display object. There must be another way to get the driver options
- Unit tests are always welcome. If you can contribute with one I will gladly accept it
- Any improvement to the GUI which can make it easier to use or more user friendly is always welcome
- Translations to more language are always welcome. Please keep in mind that the options descriptions come directly from the driver, therefore to translate the options you need to translate it directly on [mesa][1]

Author
------

This tool is written and maintained by Jean Lorenz Hertel.

License
-------

The tool is licensed under GPLv3 or superior.

[1]: https://www.mesa3d.org/