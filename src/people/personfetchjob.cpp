/*
 * This file is part of LibKGAPI library
 *
 * SPDX-FileCopyrightText: 2022 Claudio Cambra <claudio.cambra@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "personfetchjob.h"
#include "peopleservice.h"
#include "person.h"
#include "../debug.h"
#include "utils.h"
#include "account.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

namespace KGAPI2::People
{

class Q_DECL_HIDDEN PersonFetchJob::Private
{
public:
    Private(PersonFetchJob *parent);

    QNetworkRequest createRequest(const QUrl &url);

    QString readMask;
    QString fetchQuery;

private:
    PersonFetchJob * const q;
};

PersonFetchJob::Private::Private(PersonFetchJob *parent)
    : q(parent)
{
}

QNetworkRequest PersonFetchJob::Private::createRequest(const QUrl& url)
{
    QNetworkRequest request(url);
    request.setRawHeader("Host", "people.googleapis.com");
    return request;
}

PersonFetchJob::PersonFetchJob(const AccountPtr& account, QObject* parent)
    : FetchJob(account, parent)
    , d(new Private(this))
{
}

PersonFetchJob::PersonFetchJob(const QString &fetchQuery,
                               const QString &readMask,
                               const AccountPtr &account,
                               QObject* parent)
    : FetchJob(account, parent)
    , d(new Private(this))
{
    d->fetchQuery = fetchQuery;
    d->readMask = readMask;
}

PersonFetchJob::~PersonFetchJob()
{
    delete d;
}

void PersonFetchJob::start()
{
    QUrl url;
    if (d->fetchQuery.isEmpty()) {
        url = PeopleService::fetchAllContactsUrl();
    } else {
        url = PeopleService::fetchContactUrl(d->fetchQuery, d->readMask);
    }

    const QNetworkRequest request = d->createRequest(url);
    enqueueRequest(request);
}

ObjectsList PersonFetchJob::handleReplyWithItems(const QNetworkReply *reply, const QByteArray &rawData)
{
    FeedData feedData;
    ObjectsList items;
    const QString contentType = reply->header(QNetworkRequest::ContentTypeHeader).toString();
    ContentType ct = Utils::stringToContentType(contentType);
    if (ct == KGAPI2::JSON) {
        if (d->fetchQuery.isEmpty()) {
            items = PeopleService::parseConnectionsJSONFeed(feedData, rawData);
        } else {
            const auto jsonDocumentFromData = QJsonDocument::fromJson(rawData);
            if(jsonDocumentFromData.isObject()) {
                const auto results = jsonDocumentFromData.object().value(QStringLiteral("results")).toArray();

                for(const auto &result : results) {
                    if(result.isObject()) {
                        items << People::Person::fromJSON(result.toObject());
                    } else {
                        qDebug() << "Result was not an object... skipping.";
                    }
                }

            } else {
                qDebug() << "JSON document does not have object";
            }
        }

        if (feedData.nextPageUrl.isValid()) {
            emitProgress(feedData.startIndex, feedData.totalResults);

            const QNetworkRequest request = d->createRequest(feedData.nextPageUrl);
            enqueueRequest(request);
        } else {
            emitFinished();
        }

        return items;
    }

    return ObjectsList();
}

}
