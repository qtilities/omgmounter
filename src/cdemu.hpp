/*
    SPDX-FileCopyrightText:  2009-2023 Marcel Hasler, 2024 Qtilities team
    SPDX-License-Identifier: GPL-3.0-only

    This file is part of OMGMounter application.
    Authors:
        Marcel Hasler    <mahasler@gmail.com> as KDE CDEmu Manager
        Andrea Zanellato <redtide@gmail.com>
*/
#pragma once

#include <QtDBus>

class CDEmu : public QObject
{
    Q_OBJECT

public:
    struct Status
    {
        bool loaded;
        QString fileName;
    };

public:
    CDEmu();

    auto isDaemonRunning() const -> bool;

    auto getDeviceCount() const -> int;
    auto getNextFreeDevice() const -> int;

    auto getStatus(int index) const -> Status;

    auto isLoaded(int index) const -> bool;
    auto getFileName(int index) const -> QString;

    void mount(const QString& filename, int index) const;
    void unmount(int index) const;

    auto addDevice() const -> int;
    void removeDevice() const;

Q_SIGNALS:
    void daemonChanged(bool running);

    void deviceAdded();
    void deviceRemoved();
    void deviceChanged(int index);

private Q_SLOTS:
    void onServiceRegistered(const QString& service);
    void onServiceUnregistered(const QString& service);

private:
    void connectMethod(const QString& name, const char* slot);

    auto callMethod(const QDBusMessage& method) const -> QDBusMessage;
    auto callMethod(const QString& method) const -> QDBusMessage;

    static auto createMethodCall(const QString& method) -> QDBusMessage;

    QDBusServiceWatcher m_watcher;
};
