/*
    SPDX-FileCopyrightText:  2009-2023 Marcel Hasler, 2024 Qtilities team
    SPDX-License-Identifier: GPL-3.0-only

    This file is part of OMGMounter application.
    Authors:
        Marcel Hasler    <mahasler@gmail.com> as KDE CDEmu Manager
        Andrea Zanellato <redtide@gmail.com>
*/
#pragma once

#include <stdexcept>

enum class Error
{
    DeviceInUse,
    DeviceNotAvailable,
    NoFreeDevice,
    FileNotFound,
    DaemonNotRunning,
    UnknownError
};

class Exception : public std::runtime_error
{
public:
    Exception(Error error);
};
