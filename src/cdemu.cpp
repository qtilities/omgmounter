/*
    SPDX-FileCopyrightText:  2009-2023 Marcel Hasler, 2024 Qtilities team
    SPDX-License-Identifier: GPL-3.0-only

    This file is part of OMGMounter application.
    Authors:
        Marcel Hasler    <mahasler@gmail.com> as KDE CDEmu Manager
        Andrea Zanellato <redtide@gmail.com>
*/
#include "cdemu.hpp"

#include <QFile>

namespace {
    constexpr const char* ServiceName   = "net.sf.cdemu.CDEmuDaemon";
    constexpr const char* PathName      = "/Daemon";
    constexpr const char* InterfaceName = "net.sf.cdemu.CDEmuDaemon";
}

CDEmu::CDEmu()
    : m_watcher(this)
{
    connect(&m_watcher, SIGNAL(serviceRegistered(QString)),
            this,       SLOT(onServiceRegistered(QString)));

    connect(&m_watcher, SIGNAL(serviceUnregistered(QString)),
            this,       SLOT(onServiceUnregistered(QString)));

    m_watcher.setConnection(QDBusConnection::sessionBus());
    m_watcher.addWatchedService(ServiceName);

    QDBusConnection::sessionBus().interface()->startService(ServiceName);

    if (!isDaemonRunning())
        throw Exception(Error::DaemonNotRunning);

    connectMethod("DeviceAdded", SIGNAL(deviceAdded()));
    connectMethod("DeviceRemoved", SIGNAL(deviceRemoved()));
    connectMethod("DeviceStatusChanged", SIGNAL(deviceChanged(int)));
}

auto CDEmu::isDaemonRunning() const -> bool
{
    return QDBusConnection::sessionBus().interface()->isServiceRegistered(ServiceName);
}

auto CDEmu::getDeviceCount() const -> int
{
    const QDBusReply<int> reply = callMethod("GetNumberOfDevices");

    if (reply.isValid())
        return reply.value();

    return 0;
}

auto CDEmu::getNextFreeDevice() const -> int
{
    const int count = getDeviceCount();

    for (int i = 0; i < count; ++i)
    {
        if (!isLoaded(i))
            return i;
    }

    return -1;
}

auto CDEmu::getStatus(int index) const -> Status
{
    QDBusMessage m = createMethodCall("DeviceGetStatus");
    m << index;

    try {
        const QDBusMessage reply = callMethod(m);

        const QList<QVariant> args = reply.arguments();

        const bool loaded = args.at(0).toBool();
        const QList<QVariant> filenames = args.at(1).toList();

        if (loaded)
        {
            if (filenames.empty()) // Shouldn't happen
                throw Exception(Error::UnknownError);

            return { true, filenames.at(0).toString() };
        }
    }
    catch (const Exception& e) {
        qDebug() << "Unable to get device status:" << e.what();
    }

    return { false, QString() };
}

auto CDEmu::isLoaded(int index) const -> bool
{
    Status status = getStatus(index);
    return status.loaded;
}

auto CDEmu::getFileName(int index) const -> QString
{
    Status status = getStatus(index);
    return status.fileName;
}

void CDEmu::mount(const QString& filename, int index) const
{
    if (!QFile::exists(filename))
        throw Exception(Error::FileNotFound);

    if (index < 0 || index >= getDeviceCount())
        throw Exception(Error::DeviceNotAvailable);

    if (isLoaded(index))
        throw Exception(Error::DeviceInUse);

    QStringList filenames;
    filenames << filename;

    QVariantMap parameters; // Unused for now

    QDBusMessage m = createMethodCall("DeviceLoad");
    m << index << filenames << parameters;

    callMethod(m);
}

void CDEmu::unmount(int index) const
{
    if (index < 0 || index >= getDeviceCount())
        throw Exception(Error::DeviceNotAvailable);

    QDBusMessage m = createMethodCall("DeviceUnload");
    m << index;

    callMethod(m);
}

auto CDEmu::addDevice() const -> int
{
    callMethod("AddDevice");
    return getDeviceCount() - 1;
}

void CDEmu::removeDevice() const
{
    callMethod("RemoveDevice");
}

void CDEmu::onServiceRegistered(const QString& service)
{
    if (service == ServiceName)
        emit daemonChanged(true);
}

void CDEmu::onServiceUnregistered(const QString& service)
{
    if (service == ServiceName)
        emit daemonChanged(false);
}

void CDEmu::connectMethod(const QString& name, const char* slot)
{
    QDBusConnection::sessionBus().connect(ServiceName, PathName, InterfaceName, name, this, slot);
}

auto CDEmu::callMethod(const QDBusMessage& method) const -> QDBusMessage
{
    if (!isDaemonRunning())
        throw Exception(Error::DaemonNotRunning);

    const QDBusMessage reply = QDBusConnection::sessionBus().call(method);

    if (reply.type() != QDBusMessage::ReplyMessage)
        throw Exception(Error::UnknownError);

    return reply;
}

auto CDEmu::callMethod(const QString& method) const -> QDBusMessage
{
    return callMethod(createMethodCall(method));
}

auto CDEmu::createMethodCall(const QString& method) -> QDBusMessage
{
    return QDBusMessage::createMethodCall(ServiceName, PathName, InterfaceName, method);
}
