/*
 * This file is part of LibKGAPI library
 *
 * Copyright (C) 2013  Daniel Vrátil <dvratil@redhat.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) version 3, or any
 * later version accepted by the membership of KDE e.V. (or its
 * successor approved by the membership of KDE e.V.), which shall
 * act as a proxy defined in Section 6 of version 3 of the license.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "tasklist.h"
#include "../debug.h"

using namespace KGAPI2;

class Q_DECL_HIDDEN TaskList::Private
{
  public:
    Private() = default;
    Private(const Private &other) = default;

    QString title;
    QString selfLink;
    QString updated;
    QString uid;
};

TaskList::TaskList():
    Object(),
    d(new Private)
{
}

TaskList::TaskList (const TaskList& other):
    Object(other),
    d(new Private(*(other.d.get())))
{
}

TaskList::~TaskList() = default;

bool TaskList::operator==(const TaskList &other) const
{
    if (!Object::operator==(other)) {
        return false;
    }

    if (d->uid != other.d->uid) {
        qCDebug(KGAPIDebug) << "UIDs don't match";
        return false;
    }

    if (d->title != other.d->title) {
        qCDebug(KGAPIDebug) << "Titles don't match";
        return false;
    }

    return true;
}

void TaskList::setUid(const QString &uid)
{
    d->uid = uid;
}

QString TaskList::uid() const
{
    return d->uid;
}

void TaskList::setTitle(const QString& title)
{
    d->title = title;
}

QString TaskList::title() const
{
    return d->title;
}

void TaskList::setSelfLink(const QString& selfLink)
{
    d->selfLink = selfLink;
}

QString TaskList::selfLink() const
{
    return d->selfLink;
}

void TaskList::setUpdated(const QString& updated)
{
    d->updated = updated;
}

QString TaskList::updated() const
{
    return d->updated;
}
