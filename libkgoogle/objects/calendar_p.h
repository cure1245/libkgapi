/*
    libKGoogle - KGoogleObject - Calendar
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

#include <qshareddata.h>
#include <qstring.h>

#include <kdatetime.h>

namespace KGoogle {
  
  namespace Object {
  
    class CalendarData: public QSharedData
    {
      public:
	CalendarData() {};
	CalendarData(const CalendarData &other);
	~CalendarData() {};
	
	QString title;
	QString color;
	QString details;
	QString timezone;
	bool editable;   
	
	KDateTime created;
	KDateTime updated;
    };
    
  } // namespace Object
  
} // namespace KGoogle
