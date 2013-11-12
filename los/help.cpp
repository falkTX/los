//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: help.cpp,v 1.7.2.4 2009/07/05 23:06:21 terminator356 Exp $
//
//  (C) Copyright 1999/2000 Werner Schweer (ws@seh.de)
//=========================================================

#include <unistd.h>
#include <stdlib.h>

#include <QDesktopServices>
#include <QMessageBox>
#include <QUrl>

#include "app.h"
#include "globals.h"
#include "gconfig.h"
#include "icons.h"
#include "aboutbox_impl.h"

//---------------------------------------------------------
//   startHelpBrowser
//---------------------------------------------------------

void LOS::startHelpBrowser()
{
      QString losHelp = QString("https://github.com/ccherrett/oom/wiki/Quick-Start-Manual");
      launchBrowser(losHelp);
}

//---------------------------------------------------------
//   startHelpBrowser
//---------------------------------------------------------

void LOS::startHomepageBrowser()
{
      QString losHome = QString("http://www.openoctave.org");

      launchBrowser(losHome);
}

//---------------------------------------------------------
//   startBugBrowser
//---------------------------------------------------------

void LOS::startBugBrowser()
{
      //QString losBugPage("http://www.openoctave.org/wiki/index.php/Report_a_bug");
      QString losBugPage("http://www.openoctave.org/index.php/Report_a_bug");
      launchBrowser(losBugPage);
}

//---------------------------------------------------------
//   about
//---------------------------------------------------------

void LOS::about()
{
      AboutBoxImpl ab;
      ab.show();
      ab.exec();
}

//---------------------------------------------------------
//   aboutQt
//---------------------------------------------------------

void LOS::aboutQt()
{
      QMessageBox::aboutQt(this, QString("LOS"));
}

void LOS::launchBrowser(QString &whereTo)
{
      if (! QDesktopServices::openUrl(QUrl(whereTo)))
      {
            QMessageBox::information(this, tr("Unable to launch help"),
                                     tr("For some reason LOS has to launch the default\n"
                                        "browser on your machine."),
                                     QMessageBox::Ok, QMessageBox::Ok);
            printf("Unable to launch help\n");
      }
}
