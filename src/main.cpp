/*
    SPDX-FileCopyrightText:  2009-2023 Marcel Hasler, 2024 Qtilities team
    SPDX-License-Identifier: GPL-3.0-only

    This file is part of OMGMounter application.
    Authors:
        Marcel Hasler    <mahasler@gmail.com> as KDE CDEmu Manager
        Andrea Zanellato <redtide@gmail.com>
*/
#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QMessageBox>
#include <QTextStream>

#include "cdemu.hpp"
#include "exception.hpp"
#include "mainwindow.hpp"

static void mountImage(const CDEmu& cdemu, const QString& filename)
{
    int index = cdemu.getNextFreeDevice();

    if (index < 0)
        index = cdemu.addDevice();

    cdemu.mount(QDir().absoluteFilePath(filename), index);
}

static void unmountImage(const CDEmu& cdemu, int index)
{
    cdemu.unmount(index);
}

static void printStatus(const CDEmu& cdemu)
{
    static constexpr const char* Tab = "\t\t";

    QTextStream out(stdout, QIODevice::WriteOnly);
    out << "Device" << Tab << "Loaded" << Tab << "Image" << Qt::endl;

    const int deviceCount = cdemu.getDeviceCount();

    for (int i = 0; i < deviceCount; ++i)
    {
        CDEmu::Status status = cdemu.getStatus(i);

        if (status.loaded)
            out << i << Tab << "Yes" << Tab << status.fileName;
        else
            out << i << Tab << "No"  << Tab << "None";

        out << Qt::endl;
    }
}

auto main(int argc, char* argv[]) -> int
{
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme("media-optical"));
    app.setApplicationName(APPLICATION_NAME);
    app.setApplicationDisplayName(APPLICATION_DISPLAY_NAME);
    app.setOrganizationName(ORGANIZATION_NAME);
    app.setOrganizationDomain(ORGANIZATION_DOMAIN);
    app.setApplicationVersion(APPLICATION_VERSION);

    QCommandLineParser parser;
    parser.setApplicationDescription(PROJECT_DESCRIPTION);
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption mountOption("mount", QObject::tr("Mount an image."), QObject::tr("file"));
    parser.addOption(mountOption);

    QCommandLineOption unmountOption("unmount", QObject::tr("Unmount an image."), QObject::tr("device number"));
    parser.addOption(unmountOption);

    QCommandLineOption statusOption("status", QObject::tr("Show information about devices."));
    parser.addOption(statusOption);

    parser.process(app);

    try {
        CDEmu cdemu;

        if (parser.isSet(mountOption))
            mountImage(cdemu, parser.value(mountOption));
        else if (parser.isSet(unmountOption))
            unmountImage(cdemu, parser.value(unmountOption).toInt());
        else if (parser.isSet(statusOption))
            printStatus(cdemu);
        else
        {
            // Allow only one application instance
            //TODO: const KDBusService service(KDBusService::Unique);

            MainWindow window(cdemu);
            window.show();

            return QApplication::exec();
        }
    }
    catch (const Exception& e)
    {
        QMessageBox mb;
        mb.setText(e.what());
        mb.exec();
        return -1;
    }
    return 0;
}
