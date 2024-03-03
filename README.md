# OMGMounter - CD/DVD image mounter GUI

[![CI]](https://github.com/qtilities/omgmounter/actions/workflows/build.yml)

## Overview

This is a fork of [KDE CDEmu Manager], a frontend for [CDEmu], a Linux CD/DVD image mounter.
Currently it has the same features, except it differs in code because it doesn't depends on KDE libraries.

## Dependencies

Runtime:

- Qt5/6 base
- [CDEmu]
- [StatusNotifierItemQt]

Build:

- CMake
- Qt Linguist Tools
- [Qtilitools] CMake modules
- Git (optional, to pull latest VCS checkouts)

## Build

`CMAKE_BUILD_TYPE` is usually set to `Release`, though `None` might be a valid [alternative].<br>
`CMAKE_INSTALL_PREFIX` has to be set to `/usr` on most operating systems.<br>
Using `sudo make install` is discouraged, instead use the system package manager where possible.

```bash
cmake -B build -D CMAKE_BUILD_TYPE=Release -D CMAKE_INSTALL_PREFIX=/usr -W no-dev
cmake --build build --verbose
DESTDIR="$(pwd)/package" cmake --install build
```

## Licenses

- Licensed under the [GPL-3.0-only] license.


[alternative]:          https://wiki.archlinux.org/title/CMake_package_guidelines#Fixing_the_automatic_optimization_flag_override
[CDEmu]:                https://cdemu.sourceforge.net/
[CI]:                   https://github.com/qtilities/sqeleton/actions/workflows/build.yml/badge.svg
[GPL-3.0-only]:         COPYING
[Qtilitools]:           https://github.com/qtilities/qtilitools/
[StatusNotifierItemQt]: https://github.com/qtilities/libstatusnotifieritem-qt/
