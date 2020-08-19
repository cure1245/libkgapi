/*
    SPDX-FileCopyrightText: 2020 David Barchiesi <david@barchie.si>

    SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProgressBar>
#include <QFile>

#include <types.h>

#include <drive/fileresumablecreatejob.h>

namespace Ui {
    class MainWindow;
}

namespace KGAPI2 {
  class Job;
}


class MainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

  private Q_SLOTS:
    /**
     * Retrieves tokens from Google that we will use to authenticate
     * fursther requests
     */
    void authenticate();

    /**
     * Authentication has finished
     */
    void slotAuthJobFinished(KGAPI2::Job *job);
    
    /**
     * Browses files to select the one to upload
     */
    void browseFiles();
    
    /**
     * Starts resumable file upload of source file to destination directory
     */
    void uploadFile();

    /**
     * FileCreateJob requests data
     */
    void slotFileCreateJobFinished(KGAPI2::Job *job);

    /**
     * FileCreateJob requests data
     */
    void slotFileCreateJobReadyWrite(KGAPI2::Drive::FileAbstractResumableJob *job);

  private:
    Ui::MainWindow *m_ui;

    KGAPI2::AccountPtr m_account;
    
    QFile *uploadingFile;
    int bytesUploaded;
    QProgressBar *uploadProgressBar;
    
    void setInputsEnabled(bool enabled);

};

#endif // MAINWINDOW_H
