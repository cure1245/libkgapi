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

#include "event.h"
#include "../debug.h"

using namespace KGAPI2;

class Q_DECL_HIDDEN Event::Private
{
  public:
    explicit Private();
    Private(const Private &other);

    QString id;
    bool deleted;
    bool useDefaultReminders;
};

Event::Private::Private():
    deleted(false),
    useDefaultReminders(false)
{
}

Event::Private::Private(const Private &other):
    id(other.id),
    deleted(other.deleted),
    useDefaultReminders(other.useDefaultReminders)
{
}

Event::Event():
    Object(),
    KCalendarCore::Event(),
    d(new Private)
{
}

Event::Event(const Event &other):
    Object(other),
    KCalendarCore::Event(other),
    d(new Private(*(other.d)))
{
}

Event::Event(const KCalendarCore::Event &other):
    Object(),
    KCalendarCore::Event(other),
    d(new Private)
{
}

Event::~Event()
{
    delete d;
}

bool Event::operator==(const Event &other) const
{
    if (!Object::operator==(other)) {
        return false;
    }
    if (!KCalendarCore::Event::operator==(other)) {
        return false;
    }
    if (d->deleted != other.d->deleted) {
        qCDebug(KGAPIDebug) << "Deleted does not match";
        return false;
    }
    if (d->useDefaultReminders != other.d->useDefaultReminders) {
        qCDebug(KGAPIDebug) << "UseDefaultReminders does not match";
        return false;
    }

    return true;
}


bool Event::deleted() const
{
    return d->deleted;
}

bool Event::useDefaultReminders() const
{
    return d->useDefaultReminders;
}

void Event::setDeleted(bool deleted)
{
    d->deleted = deleted;
}

void Event::setUseDefaultReminders(bool useDefault)
{
    d->useDefaultReminders = useDefault;
}

QString Event::id() const
{
    QString val = customProperty("LIBKGAPI", "EventId");
    if (val.isEmpty()) {
        // Backwards compatibility: prior to introducing "id", UID was used for
        // remote identification: use it
        return KCalendarCore::Incidence::uid();
    }
    return val;
}

void Event::setId(const QString &id)
{
    setCustomProperty("LIBKGAPI", "EventId", id);
}
