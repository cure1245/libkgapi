/*
    Copyright (C) 2012  Jan Grulich <grulja@gmail.com>
    Copyright (C) 2012  Dan Vratil <dan@progdan.cz>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <libkgapi/common.h>
#include <libkgapi/account.h>

namespace Ui {
    class MainWindow;
}

namespace KGAPI {
  class AccessManager;
  class Reply;
  class Request;
};

class KJob;
class QListWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        virtual ~MainWindow();

    private Q_SLOTS:
        /**
         * Retrieves tokens from Google that we will use to authenticate
         * fursther requests
         */
        void authenticate();

        /**
         * Authentication was successful and we retrieved a new KGAPI::Account
         */
        void authenticationFinished(KGAPI::Account::Ptr& acc);


        /**
         * Retrieves list of all contacts from user's Google Contacts
         * addressbook */
        void fetchContactList();

        /**
         * All contacts were successfully retrieved
         */
        void fetchJobFinished(KJob *job);

        /**
         * A specific contact in contact list has been selected. Sends a request
         * to Google to retrieve full details about the specific contact
         */
        void contactSelected();

        /**
         * We have successfully retrieved a complete contact data from Google
         */
        void replyReceived(KGAPI::Reply *reply);

        /**
         * An error occurred during an operation
         */
        void error(KGAPI::Error err,QString msg);

    private:
        Ui::MainWindow *m_ui;

        KGAPI::Account::Ptr m_account;
        KGAPI::AccessManager *m_accessManager;
};

#endif // MAINWINDOW_H
