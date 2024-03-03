/*
    SPDX-FileCopyrightText:  2009-2023 Marcel Hasler, 2024 Qtilities team
    SPDX-License-Identifier: GPL-3.0-only

    This file is part of OMGMounter application.
    Authors:
        Marcel Hasler    <mahasler@gmail.com> as KDE CDEmu Manager
        Andrea Zanellato <redtide@gmail.com>
*/
#pragma once

#include <QLabel>
#include <QPushButton>
#include <QTreeWidgetItem>

class DeviceListItem : public QObject, public QTreeWidgetItem
{
    Q_OBJECT

public:
    DeviceListItem(int index);

    void setFileName(const QString& name);
    auto fileName() const -> QString;

    auto widget() const -> QWidget*;

Q_SIGNALS:
    void mountClicked(int index);
    void unmountClicked(int index);

private Q_SLOTS:
    void onButtonClicked();

    int          m_index;
    QWidget*     m_widget;
    QLabel*      m_label;
    QPushButton* m_button;
};
