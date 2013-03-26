/*
 * This file is part of LibKGAPI library
 *
 * Copyright (C) 2013  Daniel Vrátil <dvratil@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#include "user.h"

using namespace KGAPI2;


class DriveUser::Private
{
  public:
    Private();
    Private(const Private &other);

    QString displayName;
    QUrl pictureUrl;
    bool isAuthenticatedUser;
    QString permissionId;
};

DriveUser::Private::Private():
    isAuthenticatedUser(false)
{
}

DriveUser::Private::Private(const Private &other):
    displayName(other.displayName),
    pictureUrl(other.pictureUrl),
    isAuthenticatedUser(other.isAuthenticatedUser),
    permissionId(other.permissionId)
{
}

DriveUser::DriveUser():
    d(new Private)
{
}

DriveUser::DriveUser(const DriveUser &other):
    d(new Private(*(other.d)))
{
}

DriveUser::~DriveUser()
{
    delete d;
}

QString DriveUser::displayName() const
{
    return d->displayName;
}

QUrl DriveUser::pictureUrl() const
{
    return d->pictureUrl;
}

bool DriveUser::isAuthenticatedUser() const
{
    return d->isAuthenticatedUser;
}

QString DriveUser::permissionId() const
{
    return d->permissionId;
}

DriveUserPtr DriveUser::fromJSON(const QVariantMap &map)
{
    if (!map.contains(QLatin1String("kind")) ||
        map[QLatin1String("kind")].toString() != QLatin1String("drive#user"))
    {
        return DriveUserPtr();
    }

    DriveUserPtr user(new DriveUser());
    user->d->displayName = map[QLatin1String("displayName")].toString();
    const QVariantMap picture = map[QLatin1String("picture")].toMap();
    user->d->pictureUrl = picture[QLatin1String("url")].toUrl();
    user->d->isAuthenticatedUser = map[QLatin1String("isAuthenticatedUser")].toBool();
    user->d->permissionId = map[QLatin1String("permissionId")].toString();

    return user;
}
