/*
    SPDX-FileCopyrightText:  2009-2023 Marcel Hasler, 2024 Qtilities team
    SPDX-License-Identifier: GPL-3.0-only

    This file is part of OMGMounter application.
    Authors:
        Marcel Hasler    <mahasler@gmail.com> as KDE CDEmu Manager
        Andrea Zanellato <redtide@gmail.com>
*/
#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "dialogabout.hpp"
#include "devicelistitem.hpp"
#include "exception.hpp"

#include <QFileDialog>
#include <QHeaderView>
#include <QMessageBox>
#include <QSettings>

namespace {
    constexpr const char* FileTypes = "Images (*.mds *.mdx *.b5t *.b6t *.ccd *.sub *.img *.cue "
                                              "*.bin *.toc *.cdi *.cif *.c2d *.iso *.nrg *.udf);;"
                                      "Containers (*.dmg *.cdr *.cso *.ecm *.gz "
                                                  "*.gbi *.daa *.isz *.xz)";
    constexpr int MaxHistorySize = 10;

    constexpr const char* HistoryKey      = "History";
    constexpr const char* LastFilePathKey = "LastFilePath";
    constexpr const char* ShowTrayIconKey = "ShowTrayIcon";
    constexpr const char* SizeKey         = "Size";
    constexpr const char* PositionKey     = "Position";
}

MainWindow::MainWindow(const CDEmu& cdemu, QWidget* parent)
    : QMainWindow(parent)
    , m_ui(std::make_unique<Ui::MainWindow>())
    , m_cdemu(cdemu)
    , m_trayMenu(new QMenu(this))
{
    // Keep running in system tray if enabled
    setAttribute(Qt::WA_DeleteOnClose, false);

    m_ui->setupUi(this);

    // Menus
    QAction* quit = new QAction(QIcon::fromTheme("application-exit"), tr("&Quit"), m_ui->menuFile);
    m_ui->menuFile->addAction(quit);
    connect(quit, &QAction::triggered, qApp, &QApplication::exit);

    QMenu* mnuHelp = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(mnuHelp);

    QAction* about = new QAction(QIcon::fromTheme("help-about"), tr("&About..."), mnuHelp);
    mnuHelp->addAction(about);
    connect(about, &QAction::triggered, this, [this]{
        Qtilities::DialogAbout about(this);
        about.exec();
    });

    m_trayMenu->addMenu(mnuHelp);
    m_trayMenu->addAction(quit);

    updateHistory();

    // Settings
    QSettings settings;

    // TODO: probably not working on Wayland
    const QPoint windowPos  = settings.value(PositionKey, QPoint(200, 200)).toPoint();
    const QSize  windowSize = settings.value(SizeKey, QSize(480, 320)).toSize();
    move(windowPos);
    resize(windowSize);
    connect(qApp, &QApplication::aboutToQuit, this, [this]() {
        QSettings settings;
        settings.setValue(PositionKey, pos());
        settings.setValue(SizeKey, size());
    });

    // Tray icon
    const bool showTrayIcon = settings.value(ShowTrayIconKey, true).toBool();
    setTrayIconVisible(showTrayIcon);

    m_ui->actionTrayIcon->setChecked(showTrayIcon);
    connect(m_ui->actionTrayIcon, SIGNAL(toggled(bool)), this, SLOT(setTrayIconVisible(bool)));

    // Device list
    m_ui->deviceList->header()->setStretchLastSection(false);

    m_ui->deviceList->header()->setSectionResizeMode(0, QHeaderView::Fixed);
    m_ui->deviceList->header()->setSectionResizeMode(1, QHeaderView::Stretch);

    const QString header = tr(m_ui->deviceList->headerItem()->text(0).toLocal8Bit()) + "xxx";
    m_ui->deviceList->header()->resizeSection(0, QFontMetrics(font()).horizontalAdvance(header));

    // Device handling
    connect(m_ui->addDevice, SIGNAL(clicked()), this, SLOT(addDevice()));
    connect(m_ui->removeDevice, SIGNAL(clicked()), this, SLOT(removeDevice()));

    connect(&m_cdemu, SIGNAL(deviceAdded()), this, SLOT(updateDeviceList()));
    connect(&m_cdemu, SIGNAL(deviceRemoved()), this, SLOT(updateDeviceList()));
    connect(&m_cdemu, SIGNAL(deviceChanged(int)), this, SLOT(onDeviceChanged(int)));
    connect(&m_cdemu, SIGNAL(daemonChanged(bool)), this, SLOT(onDaemonChanged(bool)));

    // Status bar
    m_statusLabel = new QLabel(this);
    m_statusLabel->setIndent(10);
    statusBar()->addWidget(m_statusLabel);
    onDaemonChanged(m_cdemu.isDaemonRunning());
}

MainWindow::~MainWindow() = default;

void MainWindow::closeEvent(QCloseEvent* event)
{
    QMainWindow::closeEvent(event);

    if (!m_trayIcon)
        qApp->quit();
}

void MainWindow::onDaemonChanged(bool running)
{
    Q_ASSERT(m_statusLabel != nullptr);

    m_ui->centralWidget->setEnabled(running);

    if (running) {
        updateDeviceList();
        m_statusLabel->setText(tr("CDEmu daemon is running."));
    } else {
        m_statusLabel->setText(tr("CDEmu daemon not running."));
    }
}

void MainWindow::onDeviceChanged(int index)
{
    // CDEmu emits "DeviceStatusChanged" before "DeviceRemoved" if device was loaded
    auto item = dynamic_cast<DeviceListItem*>(m_ui->deviceList->topLevelItem(index));

    if (index < m_cdemu.getDeviceCount() && item)
        item->setFileName(m_cdemu.getFileName(index));
}

void MainWindow::updateDeviceList()
{
    m_ui->deviceList->clear();

    const int deviceCount = m_cdemu.getDeviceCount();

    for (int i = 0; i < deviceCount; ++i)
    {
        auto item = new DeviceListItem(i);
        item->setFileName(m_cdemu.getFileName(i));

        connect(item, SIGNAL(mountClicked(int)), this, SLOT(mount(int)));
        connect(item, SIGNAL(unmountClicked(int)), this, SLOT(unmount(int)));

        m_ui->deviceList->addTopLevelItem(item);
        m_ui->deviceList->setItemWidget(item, 0, new QLabel(QString("  %1").arg(i)));
        m_ui->deviceList->setItemWidget(item, 1, item->widget());
    }

    m_ui->removeDevice->setEnabled(m_ui->deviceList->topLevelItemCount() > 0);
}

void MainWindow::mount(int index)
{
    QSettings settings;
    QString path = settings.value(LastFilePathKey, QDir::homePath()).toString();

    const QString filename =
            QFileDialog::getOpenFileName(this, tr("Select an image file"), path, FileTypes);

    if (filename.isEmpty())
        return;

    path = QFileInfo(filename).path();
    settings.setValue(LastFilePathKey, path);

    try {
        m_cdemu.mount(filename, index);
        appendHistory(filename);
    }
    catch (const Exception& e) {
        QMessageBox mb;
        mb.setText(e.what());
        mb.exec();
    }
}

void MainWindow::unmount(int index)
{
    try {
        m_cdemu.unmount(index);
    }
    catch (const Exception& e) {
        QMessageBox mb;
        mb.setText(e.what());
        mb.exec();
    }
}

void MainWindow::mountFromHistory()
{
    const auto action = qobject_cast<QAction*>(sender());
    Q_ASSERT(action != nullptr);

    try {
        const QString filename = action->data().toString();
        const int index = m_cdemu.getNextFreeDevice();

        m_cdemu.mount(filename, index);
        appendHistory(filename);
    }
    catch (const Exception& e) {
        QMessageBox mb;
        mb.setText(e.what());
        mb.exec();
    }
}

void MainWindow::clearHistory()
{
    QSettings settings;
    settings.setValue(HistoryKey, QStringList());

    updateHistory();
}

void MainWindow::addDevice()
{
    try {
        m_cdemu.addDevice();
    }
    catch (const Exception& e) {
        QMessageBox mb;
        mb.setText(e.what());
        mb.exec();
    }
}

void MainWindow::removeDevice()
{
    try {
        m_cdemu.removeDevice();
    }
    catch (const Exception& e) {
        QMessageBox mb;
        mb.setText(e.what());
        mb.exec();
    }
}

void MainWindow::setTrayIconVisible(bool visible)
{
    Q_ASSERT(m_trayMenu != nullptr);

    if (visible) {
        if (!m_trayIcon) {
            m_trayIcon = new StatusNotifierItem(qApp->applicationName(), this);
            m_trayIcon->setIconByName("media-optical");
            m_trayIcon->setCategory(StatusNotifierItem::ApplicationStatus);
            m_trayIcon->setStatus(StatusNotifierItem::Active);
            m_trayIcon->setToolTipIconByName("media-optical");
            m_trayIcon->setToolTipTitle(qApp->applicationDisplayName());
            m_trayIcon->setContextMenu(m_trayMenu);
        }
    } else {
        delete m_trayIcon;
        m_trayIcon = nullptr;
    }

    QSettings settings;
    settings.setValue(ShowTrayIconKey, visible);
}

void MainWindow::appendHistory(const QString& filename)
{
    QSettings settings;
    QStringList history = settings.value(HistoryKey).toStringList();

    history.removeAll(filename);
    history.prepend(filename);

    settings.setValue(HistoryKey, history);

    updateHistory();
}

void MainWindow::updateHistory()
{
    QSettings settings;
    QStringList history = settings.value(HistoryKey).toStringList();

    // Delete invalid entries
    QStringList::iterator it = history.begin();

    while (it != history.end())
    {
        if (!QDir().exists(*it))
            it = history.erase(it);
        else
            ++it;
    }

    // Limit size
    while (history.size() > MaxHistorySize)
        history.removeLast();

    settings.setValue(HistoryKey, history);

    // Rebuild menu
    m_ui->menuHistory->clear();

    for (int i = 0; i < history.size(); ++i)
    {
        QAction* action = m_ui->menuHistory->addAction(history.at(i));
        action->setData(history.at(i));
        connect(action, SIGNAL(triggered(bool)), this, SLOT(mountFromHistory()));
    }
    m_ui->menuHistory->addSeparator();

    QAction* action = m_ui->menuHistory->addAction(tr("Clear History"));
    action->setIcon(QIcon::fromTheme("edit-clear-history"));
    action->setEnabled(!history.isEmpty());
    connect(action, SIGNAL(triggered(bool)), this, SLOT(clearHistory()));
}
