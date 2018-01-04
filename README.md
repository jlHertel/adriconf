Advanced DRI Configurator
=========================

The adriconf (**A**dvanced **DRI** **CONF**igurator) is a tool to set options and configure applications using the standard drirc file used by the Mesa drivers.

Features
--------

The main features of the tool include:

- Automatic removal of invalid options (Options that the driver doesn't support at all)
- Options that have the same value as the system wide options or driver default will be ignored
- Applications with empty options (all options are the same as system-wide config or driver default) will be removed automatically


Author
------

This tool is written and maintained by Jean Lorenz Hertel.

License
-------

The tool is licensed under GPLv3 or superior.