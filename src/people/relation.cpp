/*
 * SPDX-FileCopyrightText: 2021 Daniel Vrátil <dvratil@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 * SPDX-License-Identifier: LGPL-3.0-only
 * SPDX-License-Identifier: LicenseRef-KDE-Accepted-LGPL
 */

#include "relation.h"

#include "fieldmetadata.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QSharedData>

#include <algorithm>

namespace KGAPI2::People
{

struct RelationDefinition
{
    FieldMetadata metadata;
    QString person;
    QString type;
    QString formattedType;
};

class Relation::Private : public QSharedData
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
        return type == other.type && person == other.person && formattedType == other.formattedType && metadata == other.metadata;
    }

    bool operator!=(const Private &other) const
    {
        return !(*this == other);
    }

    QString type{};
    QString person{};
    QString formattedType{};
    FieldMetadata metadata{};
};

Relation::Relation()
    : d(new Private)
{
}

Relation::Relation(const RelationDefinition &definition)
    : d(new Private)
{
    d->metadata = definition.metadata;
    d->person = definition.person;
    d->type = definition.type;
    d->formattedType = definition.formattedType;
}

Relation::Relation(const Relation &) = default;
Relation::Relation(Relation &&) noexcept = default;
Relation &Relation::operator=(const Relation &) = default;
Relation &Relation::operator=(Relation &&) noexcept = default;
Relation::~Relation() = default;

bool Relation::operator==(const Relation &other) const
{
    return *d == *other.d;
}

bool Relation::operator!=(const Relation &other) const
{
    return !(*this == other);
}

QString Relation::type() const
{
    return d->type;
}

void Relation::setType(const QString &value)
{
    d->type = value;
}
QString Relation::person() const
{
    return d->person;
}

void Relation::setPerson(const QString &value)
{
    d->person = value;
}
QString Relation::formattedType() const
{
    return d->formattedType;
}
FieldMetadata Relation::metadata() const
{
    return d->metadata;
}

void Relation::setMetadata(const FieldMetadata &value)
{
    d->metadata = value;
}

Relation Relation::fromJSON(const QJsonObject &obj)
{
    if(!obj.isEmpty()) {
        RelationDefinition definition;

        const auto metadata = obj.value(QStringLiteral("metadata")).toObject();
        definition.metadata = FieldMetadata::fromJSON(metadata);
        definition.person = obj.value(QStringLiteral("person")).toString();
        definition.type = obj.value(QStringLiteral("type")).toString();
        definition.formattedType = obj.value(QStringLiteral("formattedType")).toString();

        return Relation(definition);
    }

    return Relation();
}

QVector<Relation> Relation::fromJSONArray(const QJsonArray& data)
{
    QVector<Relation> relations;

    for(const auto relation : data) {
        if(relation.isObject()) {
            const auto objectifiedRelation = relation.toObject();
            relations.append(fromJSON(objectifiedRelation));
        }
    }

    return relations;
}

QJsonValue Relation::toJSON() const
{
    QJsonObject obj;

    obj.insert(QStringView{u"type"}, d->type);
    obj.insert(QStringView{u"person"}, d->person);
    obj.insert(QStringView{u"formattedType"}, d->formattedType);
    obj.insert(QStringView{u"metadata"}, d->metadata.toJSON());
    return obj;
}

} // namespace KGAPI2::People
