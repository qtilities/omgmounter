/*
    SPDX-FileCopyrightText:  2009-2023 Marcel Hasler, 2024 Qtilities team
    SPDX-License-Identifier: GPL-3.0-only

    This file is part of OMGMounter application.
    Authors:
        Marcel Hasler    <mahasler@gmail.com> as KDE CDEmu Manager
        Andrea Zanellato <redtide@gmail.com>
*/
#include "devicelistitem.hpp"

#include <QHBoxLayout>

DeviceListItem::DeviceListItem(int index)
    : m_index(index),
      m_widget(new QWidget),
      m_label(new QLabel),
      m_button(new QPushButton)
{
    setFlags(Qt::NoItemFlags);

    m_button->setFixedWidth(30);
    m_button->setFlat(true);

    connect(m_button, SIGNAL(clicked()), this, SLOT(onButtonClicked()));

    auto layout = new QHBoxLayout(m_widget);
    layout->addWidget(m_label);
    layout->addWidget(m_button);
    layout->setContentsMargins(0, 0, 0, 0);
    m_widget->setLayout(layout);
}

void DeviceListItem::setFileName(const QString& name)
{
    m_label->setText(name);

    if (name.isEmpty())
    {
        m_button->setIcon(QIcon::fromTheme("document-open"));
        m_button->setToolTip(i18n("Select image file"));
    }
    else
    {
        m_button->setIcon(QIcon::fromTheme("media-eject"));
        m_button->setToolTip(i18n("Unmount current image"));
    }
}

auto DeviceListItem::fileName() const -> QString
{
    return m_label->text();
}

auto DeviceListItem::widget() const -> QWidget*
{
    return m_widget;
}

void DeviceListItem::onButtonClicked()
{
    if (m_label->text().isEmpty())
        emit mountClicked(m_index);
    else
        emit unmountClicked(m_index);
}
