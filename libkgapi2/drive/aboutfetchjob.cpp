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

#include "aboutfetchjob.h"
#include "about.h"
#include "account.h"
#include "debug.h"
#include "driveservice.h"
#include "utils.h"

#include <KDE/KLocalizedString>

#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>

using namespace KGAPI2;

class DriveAboutFetchJob::Private
{
  public:
    Private();

    bool includeSubscribed;
    qlonglong maxChangeIdCount;
    qlonglong startChangeId;
};

DriveAboutFetchJob::Private::Private():
    includeSubscribed(true),
    maxChangeIdCount(0),
    startChangeId(0)
{
}

DriveAboutFetchJob::DriveAboutFetchJob(const AccountPtr &account, QObject *parent):
    FetchJob(account, parent),
    d(new Private)
{
}

DriveAboutFetchJob::~DriveAboutFetchJob()
{
    delete d;
}

void DriveAboutFetchJob::setIncludeSubscribed(bool includeSubscribed)
{
    if (isRunning()) {
        kWarning() << "Can't modify includeSubscribed property when job is running";
        return;
    }

    d->includeSubscribed = includeSubscribed;
}

bool DriveAboutFetchJob::includeSubscribed() const
{
    return d->includeSubscribed;
}

void DriveAboutFetchJob::setMaxChangeIdCount(qlonglong maxChangeIdCount)
{
    if (isRunning()) {
        kWarning() << "Can't modify maxChangeIdCount property when job is running";
        return;
    }

    d->maxChangeIdCount = maxChangeIdCount;
}

qlonglong DriveAboutFetchJob::maxChangeIdCount() const
{
    return d->maxChangeIdCount;
}

void DriveAboutFetchJob::setStartChangeId(qlonglong startChangeId)
{
    if (isRunning()) {
        kWarning() << "Can't modify startChangeId property when job is running";
        return;
    }

    d->startChangeId = startChangeId;
}

qlonglong DriveAboutFetchJob::startChangeId() const
{
    return d->startChangeId;
}

DriveAboutPtr DriveAboutFetchJob::aboutData() const
{
    if (isRunning() || items().count() == 0) {
        return DriveAboutPtr();
    }

    return items().first().dynamicCast<DriveAbout>();
}

void DriveAboutFetchJob::start()
{
    QNetworkRequest request;
    request.setRawHeader("Authorization", "Bearer " + account()->accessToken().toLatin1());
    request.setUrl(DriveService::fetchAboutUrl(d->includeSubscribed, d->maxChangeIdCount, d->startChangeId));

    enqueueRequest(request);
}

KGAPI2::ObjectsList DriveAboutFetchJob::handleReplyWithItems(const QNetworkReply *reply,
                                                             const QByteArray &rawData)
{
    ObjectsList items;

    const QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
    ContentType ct = Utils::stringToContentType(contentType);
    if (ct == KGAPI2::JSON) {
        DriveAboutPtr about = DriveAbout::fromJSON(rawData);
        items << about;
    } else {
        setError(KGAPI2::InvalidResponse);
        setErrorString(i18n("Invalid response content type"));
    }

    emitFinished();
    return items;
}

#include "aboutfetchjob.moc"
