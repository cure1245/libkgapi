/*
 * SPDX-FileCopyrightText: 2021 Daniel Vrátil <dvratil@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 * SPDX-License-Identifier: LGPL-3.0-only
 * SPDX-License-Identifier: LicenseRef-KDE-Accepted-LGPL
 */

#include "source.h"

#include "profilemetadata.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QSharedData>

#include <algorithm>

namespace KGAPI2::People
{

struct SourceDefinition
{
    Source::Type type;
    QString id;
    QString etag;
    QString updateTime;
    ProfileMetadata profileMetadata;
};

class Source::Private : public QSharedData
{
public:
    explicit Private() = default;
    Private(const Private &) = default;
    Private(Private &&) noexcept = delete;
    Private &operator=(const Private &) = delete;
    Private &operator=(Private &&) noexcept = delete;
    ~Private() = default;

    bool operator==(const Private &other) const
    {
        return etag == other.etag && profileMetadata == other.profileMetadata && id == other.id && updateTime == other.updateTime && type == other.type;
    }

    bool operator!=(const Private &other) const
    {
        return !(*this == other);
    }

    QString etag{};
    ProfileMetadata profileMetadata{};
    QString id{};
    QString updateTime{};
    Source::Type type{};
};

Source::Source()
    : d(new Private)
{
}

Source::Source(SourceDefinition definition)
    : d(new Private)
{
    d->type = definition.type;
    d->id = definition.id;
    d->etag = definition.etag;
    d->updateTime = definition.updateTime;
    d->profileMetadata = definition.profileMetadata;
}

Source::Source(const Source &) = default;
Source::Source(Source &&) noexcept = default;
Source &Source::operator=(const Source &) = default;
Source &Source::operator=(Source &&) noexcept = default;
Source::~Source() = default;

bool Source::operator==(const Source &other) const
{
    return *d == *other.d;
}

bool Source::operator!=(const Source &other) const
{
    return !(*this == other);
}

QString Source::etag() const
{
    return d->etag;
}

void Source::setEtag(const QString &value)
{
    d->etag = value;
}
ProfileMetadata Source::profileMetadata() const
{
    return d->profileMetadata;
}
QString Source::id() const
{
    return d->id;
}

void Source::setId(const QString &value)
{
    d->id = value;
}
QString Source::updateTime() const
{
    return d->updateTime;
}
Source::Source::Type Source::type() const
{
    return d->type;
}

void Source::setType(const Source::Type &value)
{
    d->type = value;
}

Source Source::fromJSON(const QJsonObject &obj)
{
    if(!obj.isEmpty()) {
        SourceDefinition definition;

        const auto typeEnumString = obj.value(QStringLiteral("type"));

        if(typeEnumString == QStringLiteral("ACCOUNT")) {
            definition.type = Type::ACCOUNT;
        } else if(typeEnumString == QStringLiteral("PROFILE")) {
            definition.type = Type::PROFILE;
        } else if(typeEnumString == QStringLiteral("DOMAIN_PROFILE")) {
            definition.type = Type::DOMAIN_PROFILE;
        } else if(typeEnumString == QStringLiteral("CONTACT")) {
            definition.type = Type::CONTACT;
        } else if(typeEnumString == QStringLiteral("OTHER_CONTACT")) {
            definition.type = Type::OTHER_CONTACT;
        } else if(typeEnumString == QStringLiteral("DOMAIN_CONTACT")) {
            definition.type = Type::DOMAIN_CONTACT;
        } else {
            definition.type = Type::SOURCE_TYPE_UNSPECIFIED;
        }

        definition.id = obj.value(QStringLiteral("id")).toString();
        definition.etag = obj.value(QStringLiteral("etag")).toString();
        definition.updateTime = obj.value(QStringLiteral("id")).toString();
        definition.profileMetadata = ProfileMetadata::fromJSON(obj.value(QStringLiteral("profileMetadata")).toObject());

        return Source(definition);
    }
    return Source();
}

QJsonValue Source::toJSON() const
{
    QJsonObject obj;

    obj.insert(QStringView{u"etag"}, d->etag);
    obj.insert(QStringView{u"profileMetadata"}, d->profileMetadata.toJSON());
    obj.insert(QStringView{u"id"}, d->id);
    obj.insert(QStringView{u"updateTime"}, d->updateTime);
    switch (d->type) {
    case Type::SOURCE_TYPE_UNSPECIFIED:
        obj.insert(QStringView{u"type"}, QStringLiteral(u"SOURCE_TYPE_UNSPECIFIED"));
        break;
    case Type::ACCOUNT:
        obj.insert(QStringView{u"type"}, QStringLiteral(u"ACCOUNT"));
        break;
    case Type::PROFILE:
        obj.insert(QStringView{u"type"}, QStringLiteral(u"PROFILE"));
        break;
    case Type::DOMAIN_PROFILE:
        obj.insert(QStringView{u"type"}, QStringLiteral(u"DOMAIN_PROFILE"));
        break;
    case Type::CONTACT:
        obj.insert(QStringView{u"type"}, QStringLiteral(u"CONTACT"));
        break;
    case Type::OTHER_CONTACT:
        obj.insert(QStringView{u"type"}, QStringLiteral(u"OTHER_CONTACT"));
        break;
    case Type::DOMAIN_CONTACT:
        obj.insert(QStringView{u"type"}, QStringLiteral(u"DOMAIN_CONTACT"));
        break;
    }
    return obj;
}

} // namespace KGAPI2::People
