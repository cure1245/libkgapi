/*
 * Copyright (C) 2018  Daniel Vrátil <dvratil@kde.org>
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

#include <QObject>
#include <QTest>

#include "fakenetworkaccessmanagerfactory.h"
#include "testutils.h"
#include "calendartestutils.h"

#include "types.h"
#include "calendarmodifyjob.h"
#include "calendar.h"
#include "account.h"

using namespace KGAPI2;

Q_DECLARE_METATYPE(QList<FakeNetworkAccessManager::Scenario>)
Q_DECLARE_METATYPE(KGAPI2::CalendarsList)

class CalendarModifyJobTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase()
    {
        NetworkAccessManagerFactory::setFactory(new FakeNetworkAccessManagerFactory);
    }

    void testModify_data()
    {
        QTest::addColumn<QList<FakeNetworkAccessManager::Scenario>>("scenarios");
        QTest::addColumn<CalendarsList>("calendars");
        QTest::addColumn<CalendarsList>("responses");

        auto calendar1 = calendarFromFile(QFINDTESTDATA("data/calendar1.json"));
        auto response1 = CalendarPtr::create();
        response1->setUid(calendar1->uid());
        response1->setTitle(calendar1->title());
        response1->setDetails(calendar1->details());
        response1->setTimezone(calendar1->timezone());
        response1->setEtag(calendar1->etag());
        QTest::newRow("change calendar")
            << QList<FakeNetworkAccessManager::Scenario>{
                    scenarioFromFile(QFINDTESTDATA("data/calendar1_modify_request.txt"),
                                     QFINDTESTDATA("data/calendar1_modify_response.txt")),
                }
            << CalendarsList{ calendar1 }
            << CalendarsList{ response1 };


        auto calendar2 = calendarFromFile(QFINDTESTDATA("data/calendar2.json"));
        auto response2 = CalendarPtr::create();
        response2->setUid(calendar2->uid());
        response2->setTitle(calendar2->title());
        response2->setDetails(calendar2->details());
        response2->setTimezone(calendar2->timezone());
        response2->setEtag(calendar2->etag());
        QTest::newRow("batch modify")
            << QList<FakeNetworkAccessManager::Scenario>{
                    scenarioFromFile(QFINDTESTDATA("data/calendar1_modify_request.txt"),
                                     QFINDTESTDATA("data/calendar1_modify_response.txt")),
                    scenarioFromFile(QFINDTESTDATA("data/calendar2_modify_request.txt"),
                                     QFINDTESTDATA("data/calendar2_modify_response.txt")),
                }
            << CalendarsList{ calendar1, calendar2 }
            << CalendarsList{ response1, response2 };
    }

    void testModify()
    {
        QFETCH(QList<FakeNetworkAccessManager::Scenario>, scenarios);
        QFETCH(CalendarsList, calendars);
        QFETCH(CalendarsList, responses);

        FakeNetworkAccessManagerFactory::get()->setScenarios(scenarios);

        auto account = AccountPtr::create(QStringLiteral("MockAccount"), QStringLiteral("MockToken"));
        auto job = new CalendarModifyJob(calendars, account);
        QVERIFY(execJob(job));
        const auto items = job->items();
        QCOMPARE(items.count(), responses.count());
        for (int i = 0; i < responses.count(); ++i) {
            const auto returnedCalendar =  items.at(i).dynamicCast<Calendar>();
            QVERIFY(returnedCalendar);
            QCOMPARE(*returnedCalendar, *responses.at(i));
        }
    }
};

QTEST_GUILESS_MAIN(CalendarModifyJobTest)

#include "calendarmodifyjobtest.moc"

