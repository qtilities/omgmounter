/*
    SPDX-FileCopyrightText:  2009-2023 Marcel Hasler, 2024 Qtilities team
    SPDX-License-Identifier: GPL-3.0-only

    This file is part of OMGMounter application.
    Authors:
        Marcel Hasler    <mahasler@gmail.com> as KDE CDEmu Manager
        Andrea Zanellato <redtide@gmail.com>
*/
#include "exception.hpp"

#include <QCoreApplication>

namespace OMGMounter {
/*
    https://doc.qt.io/qt-6/i18n-source-translation.html#translate-non-qt-classes
    https://stackoverflow.com/questions/4892575/qt-translate-strings-from-non-source-files
*/
class errorStrings
{
    Q_DECLARE_TR_FUNCTIONS(OMGMounter::errorStrings)

public:
    QString get(const char *s)
    {
        static const QStringList sl{
            QT_TR_NOOP("The selected virtual device is in use."),
            QT_TR_NOOP("The selected virtual device is not available."),
            QT_TR_NOOP("All virtual devices are in use."),
            QT_TR_NOOP("The file doesn't exist."),
            QT_TR_NOOP("Unable to connect to the CDEmu daemon."),
            QT_TR_NOOP("An unknown error occurred.")
        };
        qsizetype i = sl.indexOf(s);
        if (i >= 0)
            return tr(sl.at(i).toStdString().c_str());

        return QString(s);
    }
};
} // namespace OMGMounter

static auto getErrorString(Error error) -> QString
{
    OMGMounter::errorStrings strings;
    switch (error)
    {
    case Error::DeviceInUse:
        return strings.get("The selected virtual device is in use.");

    case Error::DeviceNotAvailable:
        return strings.get("The selected virtual device is not available.");

    case Error::NoFreeDevice:
        return strings.get("All virtual devices are in use.");

    case Error::FileNotFound:
        return strings.get("The file doesn't exist.");

    case Error::DaemonNotRunning:
        return strings.get("Unable to connect to the CDEmu daemon.");

    default:
        return strings.get("An unknown error occurred.");
    }
}

Exception::Exception(Error error)
    : std::runtime_error(getErrorString(error).toLocal8Bit()) {}
