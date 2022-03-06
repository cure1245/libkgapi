/*
 * SPDX-FileCopyrightText: 2021 Daniel Vrátil <dvratil@kde.org>
 *
 * SPDX-License-Identifier: LGPL-2.1-only
 * SPDX-License-Identifier: LGPL-3.0-only
 * SPDX-License-Identifier: LicenseRef-KDE-Accepted-LGPL
 */

#include "organization.h"

#include "fieldmetadata.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QSharedData>

#include <algorithm>

namespace KGAPI2::People
{

struct OrganizationDefinition
{
    FieldMetadata metadata;
    QString type;
    QString formattedType;
    QDate startDate;
    QDate endDate;
    bool current;
    QString name;
    QString phoneticName;
    QString department;
    QString title;
    QString jobDescription;
    QString symbol;
    QString domain;
    QString location;
    QString costCenter;
    int fullTimeEquivalentMillipercent;
};

class Organization::Private : public QSharedData
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
        return location == other.location && title == other.title && type == other.type && metadata == other.metadata && symbol == other.symbol
            && formattedType == other.formattedType && name == other.name && current == other.current && costCenter == other.costCenter
            && department == other.department && domain == other.domain && startDate == other.startDate && jobDescription == other.jobDescription
            && endDate == other.endDate && phoneticName == other.phoneticName && fullTimeEquivalentMillipercent == other.fullTimeEquivalentMillipercent;
    }

    bool operator!=(const Private &other) const
    {
        return !(*this == other);
    }

    QString location{};
    QString title{};
    QString type{};
    FieldMetadata metadata{};
    QString symbol{};
    QString formattedType{};
    QString name{};
    bool current{};
    QString costCenter{};
    QString department{};
    QString domain{};
    QDate startDate{};
    QString jobDescription{};
    QDate endDate{};
    QString phoneticName{};
    int fullTimeEquivalentMillipercent{};
};

Organization::Organization()
    : d(new Private)
{
}

Organization::Organization(const OrganizationDefinition &definition)
    : d(new Private)
{
    d->metadata = definition.metadata;
    d->type = definition.type;
    d->formattedType = definition.formattedType;
    d->startDate = definition.startDate;
    d->endDate = definition.endDate;
    d->current = definition.current;
    d->name = definition.name;
    d->phoneticName = definition.phoneticName;
    d->department = definition.department;
    d->title = definition.title;
    d->jobDescription = definition.jobDescription;
    d->symbol = definition.symbol;
    d->domain = definition.domain;
    d->location = definition.location;
    d->costCenter = definition.costCenter;
    d->fullTimeEquivalentMillipercent = definition.fullTimeEquivalentMillipercent;
}

Organization::Organization(const Organization &) = default;
Organization::Organization(Organization &&) noexcept = default;
Organization &Organization::operator=(const Organization &) = default;
Organization &Organization::operator=(Organization &&) noexcept = default;
Organization::~Organization() = default;

bool Organization::operator==(const Organization &other) const
{
    return *d == *other.d;
}

bool Organization::operator!=(const Organization &other) const
{
    return !(*this == other);
}

QString Organization::location() const
{
    return d->location;
}

void Organization::setLocation(const QString &value)
{
    d->location = value;
}
QString Organization::title() const
{
    return d->title;
}

void Organization::setTitle(const QString &value)
{
    d->title = value;
}
QString Organization::type() const
{
    return d->type;
}

void Organization::setType(const QString &value)
{
    d->type = value;
}
FieldMetadata Organization::metadata() const
{
    return d->metadata;
}

void Organization::setMetadata(const FieldMetadata &value)
{
    d->metadata = value;
}
QString Organization::symbol() const
{
    return d->symbol;
}

void Organization::setSymbol(const QString &value)
{
    d->symbol = value;
}
QString Organization::formattedType() const
{
    return d->formattedType;
}
QString Organization::name() const
{
    return d->name;
}

void Organization::setName(const QString &value)
{
    d->name = value;
}
bool Organization::current() const
{
    return d->current;
}

void Organization::setCurrent(const bool &value)
{
    d->current = value;
}
QString Organization::costCenter() const
{
    return d->costCenter;
}

void Organization::setCostCenter(const QString &value)
{
    d->costCenter = value;
}
QString Organization::department() const
{
    return d->department;
}

void Organization::setDepartment(const QString &value)
{
    d->department = value;
}
QString Organization::domain() const
{
    return d->domain;
}

void Organization::setDomain(const QString &value)
{
    d->domain = value;
}
QDate Organization::startDate() const
{
    return d->startDate;
}

void Organization::setStartDate(const QDate &value)
{
    d->startDate = value;
}
QString Organization::jobDescription() const
{
    return d->jobDescription;
}

void Organization::setJobDescription(const QString &value)
{
    d->jobDescription = value;
}
QDate Organization::endDate() const
{
    return d->endDate;
}

void Organization::setEndDate(const QDate &value)
{
    d->endDate = value;
}
QString Organization::phoneticName() const
{
    return d->phoneticName;
}

void Organization::setPhoneticName(const QString &value)
{
    d->phoneticName = value;
}
int Organization::fullTimeEquivalentMillipercent() const
{
    return d->fullTimeEquivalentMillipercent;
}

void Organization::setFullTimeEquivalentMillipercent(const int &value)
{
    d->fullTimeEquivalentMillipercent = value;
}

Organization Organization::fromJSON(const QJsonObject &obj)
{
    if(!obj.isEmpty()) {
        OrganizationDefinition definition;

        const auto metadata = obj.value(QStringLiteral("metadata")).toObject();
        definition.metadata = FieldMetadata::fromJSON(metadata);
        definition.type = obj.value(QStringLiteral("type")).toString();
        definition.formattedType = obj.value(QStringLiteral("formattedType")).toString();

        const auto jsonStartDate = obj.value(QStringLiteral("startDate")).toObject();
        const auto startYear = jsonStartDate.value(QStringLiteral("year")).toInt();
        const auto startMonth = jsonStartDate.value(QStringLiteral("month")).toInt();
        const auto startDay = jsonStartDate.value(QStringLiteral("day")).toInt();
        definition.startDate = QDate(startYear, startMonth, startDay);

        const auto jsonEndDate = obj.value(QStringLiteral("endDate")).toObject();
        const auto endYear = jsonEndDate.value(QStringLiteral("year")).toInt();
        const auto endMonth = jsonEndDate.value(QStringLiteral("month")).toInt();
        const auto endDay = jsonEndDate.value(QStringLiteral("day")).toInt();
        definition.endDate = QDate(endYear, endMonth, endDay);

        definition.current = obj.value(QStringLiteral("current")).toBool();
        definition.name = obj.value(QStringLiteral("name")).toString();
        definition.phoneticName = obj.value(QStringLiteral("phoneticName")).toString();
        definition.department = obj.value(QStringLiteral("department")).toString();
        definition.title = obj.value(QStringLiteral("title")).toString();
        definition.jobDescription = obj.value(QStringLiteral("jobDescription")).toString();
        definition.symbol = obj.value(QStringLiteral("symbol")).toString();
        definition.domain = obj.value(QStringLiteral("domain")).toString();
        definition.location = obj.value(QStringLiteral("location")).toString();
        definition.costCenter = obj.value(QStringLiteral("costCenter")).toString();
        definition.fullTimeEquivalentMillipercent = obj.value(QStringLiteral("fullTimeEquivalentMillipercent")).toInt();

        return Organization(definition);
    }
    return Organization();
}

QVector<Organization> Organization::fromJSONArray(const QJsonArray& data)
{
    QVector<Organization> organizations;

    for(const auto organization : data) {
        if(organization.isObject()) {
            const auto objectifiedOrganization = organization.toObject();
            organizations.append(fromJSON(objectifiedOrganization));
        }
    }

    return organizations;
}

QJsonValue Organization::toJSON() const
{
    QJsonObject obj;

    obj.insert(QStringView{u"location"}, d->location);
    obj.insert(QStringView{u"title"}, d->title);
    obj.insert(QStringView{u"type"}, d->type);
    obj.insert(QStringView{u"metadata"}, d->metadata.toJSON());
    obj.insert(QStringView{u"symbol"}, d->symbol);
    obj.insert(QStringView{u"formattedType"}, d->formattedType);
    obj.insert(QStringView{u"name"}, d->name);
    obj.insert(QStringView{u"current"}, d->current);
    obj.insert(QStringView{u"costCenter"}, d->costCenter);
    obj.insert(QStringView{u"department"}, d->department);
    obj.insert(QStringView{u"domain"}, d->domain);
    obj.insert(QStringView{u"jobDescription"}, d->jobDescription);
    obj.insert(QStringView{u"phoneticName"}, d->phoneticName);
    obj.insert(QStringView{u"fullTimeEquivalentMillipercent"}, d->fullTimeEquivalentMillipercent);
    return obj;
}

} // namespace KGAPI2::People
