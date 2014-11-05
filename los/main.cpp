//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: app.cpp,v 1.113.2.68 2009/12/21 14:51:51 spamatica Exp $
//
//  (C) Copyright 1999-2004 Werner Schweer (ws@seh.de)
//=========================================================

#include <QApplication>
#include <QDir>
#include <QKeyEvent>
#include <QMessageBox>
#include <QLocale>
#include <QTimer>
#include <QTranslator>

#include <signal.h>
#include <sys/mman.h>
#include <alsa/asoundlib.h>

#include "app.h"
#include "audio.h"
#include "driver/audiodev.h"
#include "gconfig.h"
#include "globals.h"
#include "icons.h"

extern void initIcons();
extern void initToolbars();
extern bool initJackAudio();
extern void initMidiController();
extern void initShortCuts();
extern void readConfiguration();

static QString locale_override;

//---------------------------------------------------------
//   printVersion
//---------------------------------------------------------

static void printVersion(const char* prog)
{
    fprintf(stderr, "%s: Libre Octave Studio; Version %s\n", prog, VERSION);
}

//---------------------------------------------------------
//   ladish stuff
//---------------------------------------------------------

bool g_ladish_l1_save_requested = false;

static void ladish_l1_save(int sig)
{
    if (sig == SIGUSR1)
    {
        g_ladish_l1_save_requested = true;
    }
}

//---------------------------------------------------------
//   Reconnect default project port state on SIGHUP
//---------------------------------------------------------

bool los_reconnect_ports_requested = false;

static void los_reconnect_default_ports(int sig)
{
    if(sig == SIGHUP)
    {
        los_reconnect_ports_requested = true;
    }
}

//---------------------------------------------------------
//   LOSApplication
//---------------------------------------------------------

class LOSApplication : public QApplication
{
    LOS* los;

public:
    LOSApplication(int& argc, char** argv)
    : QApplication(argc, argv)
    {
        los= 0;
    }

    void setLOS(LOS* m)
    {
        los = m;
        startTimer(300);
    }

    bool notify(QObject* receiver, QEvent* event)
    {
        //if (event->type() == QEvent::KeyPress)
        //  printf("notify key press before app::notify accepted:%d\n", event->isAccepted());  // REMOVE Tim
        try
        {
        if(!receiver || !event)
            return false;
        bool flag = QApplication::notify(receiver, event);
        if (event->type() == QEvent::KeyPress)
        {
            //printf("notify key press after app::notify accepted:%d\n", event->isAccepted());   // REMOVE Tim
            QKeyEvent* ke = (QKeyEvent*) event;
            globalKeyState = ke->modifiers();
            bool accepted = ke->isAccepted();
            if (!accepted)
            {
                int key = ke->key();
                if (((QInputEvent*) ke)->modifiers() & Qt::ShiftModifier)
                    key += Qt::SHIFT;
                if (((QInputEvent*) ke)->modifiers() & Qt::AltModifier)
                    key += Qt::ALT;
                if (((QInputEvent*) ke)->modifiers() & Qt::ControlModifier)
                    key += Qt::CTRL;
                los->kbAccel(key);
                return true;
            }
        }
        if (event->type() == QEvent::KeyRelease)
        {
            QKeyEvent* ke = (QKeyEvent*) event;
            globalKeyState = ke->modifiers();
        }

        return flag;
        }
        catch(std::exception e)
        {
            return false;
        }
    }

    virtual void timerEvent(QTimerEvent * /* e */)
    {
        if (g_ladish_l1_save_requested)
        {
            g_ladish_l1_save_requested = false;
            printf("ladish L1 save request\n");
            los->save();
        }
        if(los_reconnect_ports_requested)
        {
            los_reconnect_ports_requested = false;
            printf("LOS reconnect ports called from SIGHUP\n");
            los->connectDefaultSongPorts();
        }
    }

};

//---------------------------------------------------------
//   localeList
//---------------------------------------------------------

static QString localeList()
{
    // Find out what translations are available:
    QStringList deliveredLocaleListFiltered;
    QString distLocale = losGlobalShare + "/locale";
    QFileInfo distLocaleFi(distLocale);
    if (distLocaleFi.isDir())
    {
        QDir dir = QDir(distLocale);
        QStringList deliveredLocaleList = dir.entryList();
        for (QStringList::iterator it = deliveredLocaleList.begin(); it != deliveredLocaleList.end(); ++it)
        {
            QString item = *it;
            if (item.endsWith(".qm"))
            {
                int inipos = item.indexOf("los_") + 5;
                int finpos = item.lastIndexOf(".qm");
                deliveredLocaleListFiltered << item.mid(inipos, finpos - inipos);
            }
        }
        return deliveredLocaleListFiltered.join(",");
    }
    return QString("No translations found!");
}

//---------------------------------------------------------
//   usage
//---------------------------------------------------------

static void usage(const char* prog, const char* txt)
{
    fprintf(stderr, "%s: %s\nusage: %s flags midifile\n   Flags:\n",
            prog, txt, prog);
    fprintf(stderr, "   -h       this help\n");
    fprintf(stderr, "   -v       print version\n");
    fprintf(stderr, "   -d       debug mode: no threads, no RT\n");
    fprintf(stderr, "   -D       debug mode: enable some debug messages\n");
    fprintf(stderr, "   -m       debug mode: trace midi Input\n");
    fprintf(stderr, "   -M       debug mode: trace midi Output\n");
    fprintf(stderr, "   -s       debug mode: trace sync\n");
    fprintf(stderr, "   -a       no audio\n");
    fprintf(stderr, "   -P  n    set audio driver real time priority to n (Dummy only, default 40. Else fixed by Jack.)\n");
    fprintf(stderr, "   -Y  n    force midi real time priority to n (default: audio driver prio +2)\n");
    fprintf(stderr, "   -l  xx   force locale to the given language/country code (xx = %s)\n", localeList().toLatin1().constData());
}

//---------------------------------------------------------
//   main
//---------------------------------------------------------

int main(int argc, char* argv[])
{
    Q_INIT_RESOURCE(los);

    losUser = QDir::homePath();//QString(getenv("HOME"));
    losGlobalLib = QString(LIBDIR);
    losGlobalShare = QString(SHAREDIR);
    losProject = losProjectInitPath; //getcwd(0, 0);
    losInstruments = losGlobalShare + QDir::separator() + QString("instruments");

    // Create config dir if it doesn't exists
    QDir cPath = QDir(configPath);
    if (!cPath.exists())
        cPath.mkpath(".");

    //Create the directory to store the internal routing map
    QDir rPath = QDir(routePath);
    if(!rPath.exists())
        rPath.mkpath(".");

    srand(time(0)); // initialize random number generator
    initMidiController();
    QApplication::setColorSpec(QApplication::ManyColor);
    LOSApplication app(argc, argv);
    app.setStyleSheet("QMessageBox{background-color: #595966;} QPushButton{border-radius: 3px; padding: 5px; background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,stop: 0 #626272, stop:0.1 #5b5b6b, stop: 1.0 #4d4d5b); border: 1px solid #393941; font-family: fixed-width;	font-weight: bold; font-size: 11px; color: #d0d4d0; } QPushButton:pressed, QPushButton::checked, QPushButton::hover { color: #e2e5e5; border-radius: 3px; padding: 3px; border: 1px solid #181819; background-color: #393941; }");
    QPalette p = QApplication::palette();
    p.setColor(QPalette::Disabled, QPalette::Light, QColor(89,89,102));
    QApplication::setPalette(p);

    initShortCuts();
    readConfiguration();

    losUserInstruments = config.userInstrumentsDir;

    int i;

    QString optstr("ahvdDmMsP:Y:l:py");

    while ((i = getopt(argc, argv, optstr.toLatin1().constData())) != EOF)
    {
        char c = (char) i;
        switch (c)
        {
            case 'v': printVersion(argv[0]);
                return 0;
            case 'd':
            case 'D': debugMsg = true;
                break;
            case 'm': midiInputTrace = true;
                break;
            case 'M': midiOutputTrace = true;
                break;
            // XXX remove var
            //case 's': debugSync = true;
            //    break;
            case 'P': realTimePriority = atoi(optarg);
                break;
            case 'Y': midiRTPrioOverride = atoi(optarg);
                break;
            case 'l': locale_override = QString(optarg);
                break;
            case 'h': usage(argv[0], argv[1]);
                return -1;
            default: usage(argv[0], "bad argument");
                return -1;
        }
    }

    if (initJackAudio())
    {
        QMessageBox::critical(NULL, "LOS fatal error", "LOS <b>failed</b> to find a <b>Jack audio server</b>.<br><br>");
        return 1;
    }

    realTimeScheduling = audioDevice->isRealtime();

    fifoLength = 131072 / segmentSize;

    argc -= optind;
    ++argc;

    {//TODO: Change all these debugMsg stuff to just use qDebug
        printf("global lib:       <%s>\n", losGlobalLib.toLatin1().constData());
        printf("global share:     <%s>\n", losGlobalShare.toLatin1().constData());
        printf("los home:         <%s>\n", losUser.toLatin1().constData());
        printf("project dir:      <%s>\n", losProject.toLatin1().constData());
        printf("user instruments: <%s>\n", losUserInstruments.toLatin1().constData());
    }

    static QTranslator translator(0);
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    QString locale(qApp->inputMethod()->locale().name());
#else
    QString locale(QApplication::keyboardInputLocale().name());
#endif
    if (locale_override.length())
        locale = locale_override;
    if (locale != "C")
    {
        QString loc("los_");
        loc += locale;
        if (translator.load(loc, QString(".")) == false)
        {
            QString lp(losGlobalShare);
            lp += QString("/locale");
            if (translator.load(loc, lp) == false)
            {
                printf("no locale <%s>/<%s>\n", loc.toLatin1().constData(), lp.toLatin1().constData());
            }
        }
        app.installTranslator(&translator);
    }

    if (locale == "de")
    {
        printf("locale de\n");
        hIsB = false;
    }

    initIcons();
    initToolbars();

    QApplication::addLibraryPath(losGlobalLib + "/qtplugins");
    if (debugMsg)
    {
        QStringList list = app.libraryPaths();
        QStringList::Iterator it = list.begin();
        printf("QtLibraryPath:\n");
        while (it != list.end())
        {
            printf("  <%s>\n", (*it).toLatin1().constData());
            ++it;
        }
    }

    los = new LOS(argc, &argv[optind]);
    app.setLOS(los);
    los->setWindowIcon(*losIcon);

    if (!debugMsg)
    {
        if (mlockall(MCL_CURRENT | MCL_FUTURE))
            perror("WARNING: Cannot lock memory:");
    }

    los->show();
    los->seqStart();

    // ladish L1
    signal(SIGUSR1, ladish_l1_save);

    // LOS ports reconnect
    signal(SIGHUP, los_reconnect_default_ports);

    int rv = app.exec();
    if (debugMsg)
        printf("app.exec() returned:%d\nDeleting main LOS object\n", rv);

    delete los;
    if (debugMsg)
        printf("Finished! Exiting main, return value:%d\n", rv);
    return rv;
}
