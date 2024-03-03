/*
    SPDX-FileCopyrightText:  2009-2023 Marcel Hasler, 2024 Qtilities team
    SPDX-License-Identifier: GPL-3.0-only

    This file is part of OMGMounter application.
    Authors:
        Marcel Hasler    <mahasler@gmail.com> as KDE CDEmu Manager
        Andrea Zanellato <redtide@gmail.com>
*/
#include "exception.hpp"

static auto getErrorString(Error error) -> QString
{
    switch (error)
    {
    case Error::DeviceInUse:
        return i18n("The selected virtual device is in use.");

    case Error::DeviceNotAvailable:
        return i18n("The selected virtual device is not available.");

    case Error::NoFreeDevice:
        return i18n("All virtual devices are in use.");

    case Error::FileNotFound:
        return i18n("The file doesn't exist.");

    case Error::DaemonNotRunning:
        return i18n("Unable to connect to the CDEmu daemon.");

    default:
        return i18n("An unknown error occured.");
    }
}

Exception::Exception(Error error)
    : std::runtime_error(getErrorString(error).toLocal8Bit()) {}
