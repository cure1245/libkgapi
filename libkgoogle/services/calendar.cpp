/*
    <one line to give the program's name and a brief idea of what it does.>
    Copyright (C) 2011  Dan Vratil <dan@progdan.cz>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include "calendar.h"
#include "objects/calendar.h"
#include "objects/event.h"
#include "kgoogleaccessmanager.h"

#ifdef WITH_KCAL
#include <kcal/event.h>
#include <kcal/attendee.h>
#include <kcal/alarm.h>
#include <kcal/recurrence.h>
#include <kcal/recurrencerule.h>
#include <kcal/icalformat.h>
#else
#include <kcalcore/event.h>
#include <kcalcore/attendee.h>
#include <kcalcore/alarm.h>
#include <kcalcore/recurrence.h>
#include <kcalcore/recurrencerule.h>
#include <kcalcore/icalformat.h>
#endif

#include <qjson/parser.h>
#include <qjson/serializer.h>
#include <kcalcore/alarm.h>
#include <ksystemtimezone.h>

using namespace KGoogle;

#ifdef WITH_KCAL
using namespace KCal;
#else
using namespace KCalCore;
#endif


/********** PUBLIC JSON INTERFACE ************/

KGoogleObject* Service::Calendar::JSONToObject(const QByteArray& jsonData)
{
  QJson::Parser parser;
  
  QVariantMap object = parser.parse(jsonData).toMap();
  QVariantMap data = object["data"].toMap();
  
  if (object["kind"] == "calendar#calendar")
    return JSONToCalendar(data);
  else if (object["kind"] == "calendar#event")
    return JSONToEvent(data);
  else {
    /* FIXME: This is a very very wild assumption */
    return JSONToEvent(data);
  }

}

QByteArray Service::Calendar::objectToJSON(KGoogleObject* object)
{
  QVariantMap map;
  
  if (dynamic_cast<const Object::Calendar*>(object)) {
    map = calendarToJSON(object);
  } else if (dynamic_cast<const Object::Event*>(object)) {
    map = eventToJSON(object);
  }

  QJson::Serializer serializer;
  return serializer.serialize(map);
}

QList< KGoogleObject* > Service::Calendar::parseJSONFeed(const QByteArray& jsonFeed, FeedData* feedData)
{
  QJson::Parser parser;
  
  QVariantMap map = parser.parse(jsonFeed).toMap();
  QVariantMap data = map["data"].toMap();
  
  QList< KGoogleObject* > list;
  
  if (data["kind"] == "calendar#calendarFeed") {
    list = parseCalendarJSONFeed(data["items"].toList());
  } else if (data["kind"] == "calendar#eventFeed") {
    list = parseEventJSONFeed(data["items"].toList());
  }
    
  if (feedData) {
    feedData->itemsPerPage = data["itemsPerPage"].toInt();
    feedData->startIndex = data["startIndex"].toInt();
    feedData->totalResults = data["totalResults"].toInt();
    feedData->nextLink = data["nextLink"].toUrl();
  }
  
  return list;
}


/************* PUBLIC XML INTERFACE ***********/

KGoogleObject* Service::Calendar::XMLToObject(const QByteArray& xmlData)
{
  Q_UNUSED (xmlData);
  
  return 0;
}

QByteArray Service::Calendar::objectToXML(KGoogleObject* object)
{
  Q_UNUSED (object);
  
  return QByteArray();
}

QList< KGoogleObject* > Service::Calendar::parseXMLFeed(const QByteArray& xmlFeed, FeedData* feedData)
{
  Q_UNUSED (xmlFeed);
  Q_UNUSED (feedData);
  
  return QList< KGoogleObject* >();
}


/************* URLS **************/

QString Service::Calendar::scopeUrl() 
{ 
  return "https://www.google.com/calendar/feeds/"; 
}

QString Service::Calendar::fetchAllUrl() 
{
  // %1 = user (default or user@gmail.com)
  // %2 = visibility (allcalendars for list of calendars, private for list of events)
  return "https://www.google.com/calendar/feeds/%1/%2/full?alt=jsonc&v=2"; 
}

QString Service::Calendar::fetchUrl()
{
  // %1 = user (default or user@gmail.com)
  // %2 = visibility (allcalendars for list of calendars, private for list of events)
  // %3 = event/calendar ID
  return "https://www.google.com/calendar/feeds/%1/%2/full/%3?alt=jsonc&v=2";
}

QString Service::Calendar::createUrl()
{
  // %1 = user (default or user@gmail.com)
  // %2 = visibility (allcalendars for list of calendars, private for list of events)
  return "https://www.google.com/calendar/feeds/%1/%2/full?alt=jsonc&v=2";
}

QString Service::Calendar::updateUrl()
{
  // %1 = user (default or user@gmail.com)
  // %2 = visibility (allcalendars for list of calendars, private for list of events)
  // %3 = event/calendar ID
  return "https://www.google.com/calendar/feeds/%1/%2/full/%3?alt=jsonc&v=2";
}

 QString Service::Calendar::removeUrl() 
{ 
  // %1 = user (default or user@gmail.com)
  // %2 = visibility (allcalendars for list of calendars, private for list of events)
  // %3 = event/calendar ID
  return "https://www.google.com/calendar/feeds/%1/%2/full/%3?v=2"; 
}

const QString Service::Calendar::protocolVersion()
{
  return "2";
}

bool Service::Calendar::supportsJSONRead(QString* urlParam)
{
  if (urlParam)
    *urlParam = "jsonc";
  
  return true;
}

bool Service::Calendar::supportsJSONWrite(QString* urlParam)
{
  if (urlParam)
    *urlParam = "jsonc";
  
  return true;
}




/******** PRIVATE METHODS ************/
KGoogleObject* Service::Calendar::JSONToCalendar(const QVariantMap& calendar)
{
  Object::Calendar *object = new Object::Calendar();
  
  QString id = calendar["id"].toString().replace("%40", "@");
  id.remove("http://www.google.com/calendar/feeds/default/calendars/"); // Remove the URL prefix
    
  object->setId(id);
  object->setEtag(calendar["etag"].toString());
  object->setTitle(calendar["title"].toString());
  object->setColor(calendar["color"].toString());
  object->setDetails(calendar["details"].toString());
  object->setEditable(calendar["editable"].toBool());
  object->setCreated(KGoogleAccessManager::RFC3339StringToDate(calendar["created"].toString()));
  object->setUpdated(KGoogleAccessManager::RFC3339StringToDate(calendar["created"].toString()));
 
  return dynamic_cast< KGoogleObject* >(object);
}

QVariantMap Service::Calendar::calendarToJSON(KGoogleObject* calendar)
{
  QVariantMap output, entry;
  Object::Calendar *object = static_cast< Object::Calendar* >(calendar);
  
  entry["id"] = "http://www.google.com/calendar/feeds/default/calendars/" + calendar->id(); 
  entry["title"] = object->title();
  entry["color"] = object->color();
  entry["details"] = object->details();
  entry["editable"] = object->editable();
  
  output["apiVersion"] = "2.3";
  output["data"] = entry;
  
  return output;
}

QList< KGoogleObject* > Service::Calendar::parseCalendarJSONFeed(const QVariantList& feed)
{
  QList< KGoogleObject* > output;
 
  foreach (QVariant i, feed) {
    output.append(JSONToCalendar(i.toMap()));
  }

  return output;
}

KGoogleObject* Service::Calendar::JSONToEvent(const QVariantMap& event)
{
  Object::Event *object = new Object::Event();
  
  /* ID */
  object->setId(event["id"].toString());
  
  /* ETAG */
  object->setEtag(event["etag"].toString());
  
  /* Deleted? */
  object->setDeleted(event["deleted"].toBool());

  /* Can edit? */
  object->setReadOnly(!event["canEdit"].toBool());
  
  /* Summary */
  object->setSummary(event["title"].toString());
  
  /* Description */
  object->setDescription(event["details"].toString());
  
  /* Created */
  object->setCreated(KGoogleAccessManager::RFC3339StringToDate(event["created"].toString()));
  
  /* Last updated */
  object->setLastModified(KGoogleAccessManager::RFC3339StringToDate(event["updated"].toString()));
  
  /* Status */
  if (event["status"].toString() == "confirmed") {
    object->setStatus(Incidence::StatusConfirmed);
  } else if (event["status"].toString() == "canceled") {
    object->setStatus(Incidence::StatusCanceled);
    object->setDeleted(true);
  } else if (event["status"].toString() == "tentative") {
    object->setStatus(Incidence::StatusTentative);
  } else {
    object->setStatus(Incidence::StatusNone);
  }
  
  /* Transparency */
  if (event["transparency"].toString() == "transparent") {
    object->setTransparency(Event::Transparent);
  } else { /* Assume opaque as default transparency */
    object->setTransparency(Event::Opaque);
  }
  
  /* Location */
  object->setLocation(event["location"].toString());
  
  /* Attendees */
  QVariantList attendees = event["attendees"].toList();
  foreach (const QVariant &a, attendees) {
    QVariantMap att = a.toMap();
    AttendeePtr attendee(new Attendee(att["displayName"].toString(),
				      att["email"].toString()));
    if (att["status"].toString() == "accepted")
      attendee->setStatus(Attendee::Accepted);
    else if (att["status"].toString() == "invited")
      attendee->setStatus(Attendee::None);
    else if (att["status"].toString() == "declined")
      attendee->setStatus(Attendee::Declined);
    else if (att["status"].toString() == "tentative")
      attendee->setStatus(Attendee::Tentative);
    else
      attendee->setStatus(Attendee::None);

    if (att["type"] == "required")
      attendee->setRole(Attendee::ReqParticipant);
    else if (att["type"] == "optional")
      attendee->setRole(Attendee::OptParticipant);
    /* No further roles supported by Google */
    
    /* Google sends organizer as one of attendees with rel="organizer" attribute */
    if (att["rel"] == "organizer") {
      PersonPtr person(new Person(att["displayName"].toString(),
				  att["email"].toString()));
#ifdef WITH_KCAL
      object->setOrganizer(*person);
#else
      object->setOrganizer(person);
#endif
    }
    
    object->addAttendee(attendee, true);
  }
  
  /* Start, End, all-day event */
  QVariantList whens = event["when"].toList();
  foreach (const QVariant &w, whens) {
    QVariantMap when = w.toMap();
    KDateTime dtStart, dtEnd;
    /* When dtStart and dtEnd are only dates without times, than assume all-day event */
    if ((when["start"].toString().length() == 10) &&
       (when["end"].toString().length() == 10)) {
      object->setAllDay(true);
      dtStart = KDateTime::fromString(when["start"].toString(), "%Y-%m-%d");
      dtEnd = KDateTime::fromString(when["end"].toString(), "%Y-%m-%d").addDays(-1);
    } else {
      dtStart = KGoogleAccessManager::RFC3339StringToDate(when["start"].toString());
      dtEnd = KGoogleAccessManager::RFC3339StringToDate(when["end"].toString());
      
      if (!dtStart.isLocalZone())
	dtStart = dtStart.toLocalZone();
      
      if (!dtEnd.isLocalZone())
	dtEnd = dtEnd.toLocalZone();
    }
    
    object->setDtStart(dtStart);
    object->setDtEnd(dtEnd);
  }
  
  /* Recurrence */
  QStringList recrs = event["recurrence"].toString().split("\r\n");
  foreach (const QString &rec, recrs) {
    if (rec.left(7) == "DTSTART") {
      bool allday;
      KDateTime dt = parseRecurrenceDT(rec.mid(8), &allday);
      object->setDtStart(dt);
      object->setAllDay(allday);
    } else  if (rec.left(5) == "DTEND") {
      bool allday;
      KDateTime dt = parseRecurrenceDT(rec.mid(6), &allday);
      if (allday)
        dt = dt.addDays(-1);
      object->setDtEnd(dt);
      object->setAllDay(allday);
    } else if (rec.left(5) == "RRULE") {
      ICalFormat format;
      RecurrenceRule *recurrenceRule = object->recurrence()->defaultRRule(true);
      format.fromString(recurrenceRule, rec.mid(6));
    } else if (rec == "BEGIN:VTIMEZONE") {
      /* TODO: Implement time-zone dependent recurrences. For now take only the "standard"
       * and leave when it comes to any other */
      break;
    }
  }
  
  /* Reminders - the recurrence and/or start/end must be set BEFORE alarms 
   * because alarms must already know dtStart! */
  QVariantList reminders = event["reminders"].toList();
  foreach (const QVariant &r, reminders) {
    QVariantMap reminder = r.toMap();
    AlarmPtr alarm(new Alarm(object));
    alarm->setTime(object->dtStart());
    
    if (reminder["method"].toString() == "alert")
      alarm->setType(Alarm::Display);
    else if (reminder["method"].toString() == "email")
      alarm->setType(Alarm::Email);
    else {
      continue;
    }
    
    if (!reminder["minutes"].toString().isEmpty()) {
      /* Start offset is in seconds and must be negative, so that the alarm trigger times
       * is BEFORE the event start. When alarm should trigger AFTER the event start, 
       * it is specified as "absoluteTime" */
      alarm->setStartOffset(Duration(reminder["minutes"].toInt()*(-60)));
    } else if (!reminder["absoluteTime"].toString().isEmpty()) {
      alarm->setTime(KDateTime::fromString(reminder["absoluteTime"].toString(), "%Y-%m-%dT%H:%M:%S"));
    } else {
      continue;
    }
    alarm->setEnabled(true);
    object->addAlarm(alarm);
  }

  /* Extended properties */
  QVariantList extendedProperties = event["extendedProperties"].toList();
  foreach (const QVariant &p, extendedProperties) {
    QVariantMap property = p.toMap();
    if (property["name"].toString() == "categories") {
	object->setCategories(property["value"].toString());
    }
  }
  /* TODO: Implement support for additional features:
   * http://code.google.com/apis/gdata/docs/2.0/elements.html
   */  
  
  return dynamic_cast< KGoogleObject* >(object);
}

QVariantMap Service::Calendar::eventToJSON(KGoogleObject* event)
{
  Object::Event *object = static_cast<Object::Event*>(event);
  QVariantMap output, data;
  
  /* ID */
  data["id"] = object->id();
  
  /* Deleted? */
  data["deleted"] = object->deleted();
  
  /* Can edit? */
  data["canEdit"] = object->isReadOnly();
   
  /* Summary */
  data["title"] = object->summary();
  
  /* Description */
  data["details"] = object->description();
  
  /* Created */
  data["created"] = KGoogleAccessManager::dateToRFC3339String(object->created());
  
  /* Last updated */
  data["updated"] = KGoogleAccessManager::dateToRFC3339String(object->lastModified());
  
  /* Status */
  if (object->status() == Incidence::StatusConfirmed)
    data["status"] = "confirmed";
  else if (object->status() == Incidence::StatusCanceled)
    data["status"] = "canceled";
  else if (object->status() == Incidence::StatusTentative)
    data["status"] = "tentative";

  /* Transparency */
  if (object->transparency() == Event::Transparent)
    data["transparency"] = "transparent";
  else
    data["transparency"] = "opaque";
  
  /* Location */
  data["location"] = object->location();
  
  /* Attendees */
  QVariantList atts;
  foreach (const AttendeePtr attee, object->attendees()) {
    QVariantMap att;
    
    att["displayName"] = attee->name();
    att["email"] = attee->email();
    
    if (attee->status() == Attendee::Accepted)
      att["status"] = "accepted";
    else if (attee->status() == Attendee::None)
      att["status"] = "invited";
    else if (attee->status() == Attendee::Declined)
      att["status"] = "declined";
    else if (attee->status() == Attendee::Tentative)
      att["status"] == "tentative";
    
    if (attee->role() == Attendee::ReqParticipant)
      att["type"] = "required";
    else if (attee->role() == Attendee::OptParticipant)
      att["type"] = "optional";
    /* No further roles supported by Google */

    atts.append(att);
  }
  
  /* Organizer. Google expects organizator as one of attendees. */
  QVariantMap org;
#ifdef WITH_KCAL
  PersonPtr organizer = new Person(object->organizer());
#else
  PersonPtr organizer = object->organizer();
#endif
  org["displayName"] = organizer->name();
  org["email"] = organizer->email();
  org["rel"] = "organizer";
  atts.append(org);
  
  data["attendees"] = atts;
  
    
  /* Start, end, reminders */
  QVariantMap when;
  
  /* Reminders */
  QVariantList reminders;
  foreach (AlarmPtr a, object->alarms()) {
    QVariantMap alarm;
    if (a->type() == Alarm::Display)
      alarm["method"] = "alert";
    else if (a->type() == Alarm::Email)
      alarm["method"] = "email";
    
    if (a->startOffset().asSeconds() < 0)
      alarm["minutes"] = a->startOffset().asSeconds()/(-60);
    else
      alarm["absoluteTime"] = a->time().toString(KDateTime::RFCDate);
    
    reminders.append(alarm);
  }

  /* Recurrence */
  ICalFormat format;
  QString recStr;
  QString start, end;
  if (object->allDay()) {
    start = "VALUE=DATE:" + object->dtStart().toUtc().toString("%Y%m%d");
    end = "VALUE=DATE:" + object->dtEnd().toUtc().toString("%Y%m%d");
  } else {
    start = "TZID=" + object->dtStart().timeZone().name() + ":" + object->dtStart().toLocalZone().toString("%Y%m%dT%H%M%S");
    end = "TZID=" + object->dtEnd().timeZone().name() + ":" + object->dtEnd().toLocalZone().toString("%Y%m%dT%H%M%S");
  }
  recStr = "DTSTART;" + start + "\r\n" +
	   "DTEND;" + end;
  foreach (RecurrenceRule *rrule, object->recurrence()->rRules())
    recStr += "\r\n" + format.toString(rrule);

  if (object->recurrence()->rRules().length() > 0) {
    data["recurrence"] = recStr;

    /* For recurrent events, reminders must be set outside "gd:when"
     * elements. */
    if (reminders.length() > 0)
      data["reminders"] = reminders;

  } else {

    /* When no recurrence is set, we can set dtStart and dtEnd */    
    if (object->allDay()) {
      when["start"] = object->dtStart().toString("%Y-%m-%d");
      when["end"] = object->dtEnd().addDays(1).toString("%Y-%m-%d");
    } else {
      when["start"] = KGoogleAccessManager::dateToRFC3339String(object->dtStart());
      when["end"] = KGoogleAccessManager::dateToRFC3339String(object->dtEnd());
    }
    when["reminders"] = reminders;
    data["when"] = QVariantList() << when;
  }


  /* Store categories */
  if (!object->categories().isEmpty()) {
    QVariantMap categories;
    categories["name"] = "categories";
    categories["value"] = object->categoriesStr();
    data["extendedProperties"] = QVariantList() << categories;
  }

  /* TODO: Implement support for additional features:
   * http://code.google.com/apis/gdata/docs/2.0/elements.html
   */
  
  output["apiVersion"] = "2.3";
  output["data"] = data;
  
  return output;
}

QList< KGoogleObject* > Service::Calendar::parseEventJSONFeed(const QVariantList& feed)
{
  QList< KGoogleObject* > output;

  foreach (QVariant i, feed) {
    output.append(JSONToEvent(i.toMap()));
  }

  return output;
}

KDateTime Service::Calendar::parseRecurrenceDT(const QString& dt, bool *allDay)
{
  QString value;
  KTimeZone tz;
  KDateTime date;

  QString left = dt.left(dt.indexOf(":"));
  QStringList params = left.split(";");
  foreach (const QString &param, params) {
    if (param.startsWith("VALUE")) {
      value = param.mid(param.indexOf("=") + 1);
    } else if (param.startsWith("TZID")) {
      QString tzname = param.mid(param.indexOf("=") + 1);
      tz = KSystemTimeZones::zone(tzname);
    }
  }

  QString dateStr = dt.mid(dt.lastIndexOf(":") + 1);

  if (value == "DATE") {
    date = KDateTime::fromString(dateStr, "%Y%m%d");
    *allDay = true;
  } else if (value == "PERIOD") {
    QString start = dateStr.left(dateStr.indexOf("/"));
    date = KGoogleAccessManager::RFC3339StringToDate(start);
    if (tz.isValid())
      date.setTimeSpec(tz);
    *allDay = false;
  } else {
    date = KGoogleAccessManager::RFC3339StringToDate(dateStr);
    if (tz.isValid())
      date.setTimeSpec(tz);

    *allDay = false;
  }

  return date;
}
