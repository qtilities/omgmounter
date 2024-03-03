/*
    SPDX-FileCopyrightText:  2009-2023 Marcel Hasler, 2024 Qtilities team
    SPDX-License-Identifier: GPL-3.0-only

    This file is part of OMGMounter application.
    Authors:
        Marcel Hasler    <mahasler@gmail.com> as KDE CDEmu Manager
        Andrea Zanellato <redtide@gmail.com>
*/
#pragma once

#include "cdemu.hpp"

#include <StatusNotifierItemQt5/statusnotifieritem.h>

#include <QMainWindow>
#include <QLabel>

#include <memory>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const CDEmu& cdemu, QWidget* parent = nullptr);
    ~MainWindow() override;

private Q_SLOTS:
    void onDaemonChanged(bool);
    void onDeviceChanged(int index);

    void updateDeviceList();

    void mount(int index);
    void unmount(int index);

    void mountFromHistory();
    void clearHistory();

    void addDevice();
    void removeDevice();

    void setTrayIconVisible(bool visible);

private:
    void closeEvent(QCloseEvent* event) override;

    void appendHistory(const QString& filename);
    void updateHistory();

    std::unique_ptr<Ui::MainWindow> m_ui;
    const CDEmu&                    m_cdemu;
    QLabel*                         m_statusLabel = nullptr;
    StatusNotifierItem*             m_trayIcon    = nullptr;
};
