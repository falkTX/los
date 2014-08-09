//=========================================================
//  LOS
//  Libre Octave Studio
//    $Id: $
//  (C) Copyright 2011 Andrew Williams and the LOS team
//=========================================================

#include <QWidgetAction>
#include <QKeyEvent>
#include <QCoreApplication>
#include <QVBoxLayout>
#include <QtGui>
#include <QLineEdit>
#include <QTextEdit>

#include "track.h"
#include "gconfig.h"
#include "config.h"
#include "globals.h"
#include "icons.h"
#include "song.h"
#include "node.h"
#include "audio.h"
#include "app.h"
#include "midiport.h"
#include "mididev.h"
#include "minstrument.h"
#include "TrackManager.h"
#include "TrackInstrumentMenu.h"

#include <QLabel>
#include <QListView>

TrackInstrumentMenu::TrackInstrumentMenu(QMenu* parent, MidiTrack *t) : QWidgetAction(parent)
{
    m_track = t;
    m_edit = false;
}

QWidget* TrackInstrumentMenu::createWidget(QWidget* parent)/*{{{*/
{
    if(!m_track)
        return 0;

    int baseHeight = 109;
    QVBoxLayout* layout = new QVBoxLayout();
    QWidget* w = new QWidget(parent);
    w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    QLabel* header = new QLabel();
    header->setPixmap(QPixmap(":/images/instrument_menu_title.png"));
    header->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    header->setObjectName("TrackInstrumentMenuHeader");
    QLabel* tvname = new QLabel(m_track->name());
    tvname->setAlignment(Qt::AlignVCenter | Qt::AlignHCenter);
    tvname->setObjectName("TrackInstrumentMenuLabel");
    layout->addWidget(header);
    layout->addWidget(tvname);
    m_listModel = new QStandardItemModel();
    list = new QListView();
    list->setObjectName("TrackInstrumentMenuList");
    list->setSelectionMode(QAbstractItemView::SingleSelection);
    list->setAlternatingRowColors(true);
    list->setEditTriggers(QAbstractItemView::NoEditTriggers);
    list->setModel(m_listModel);
    list->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    list->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    list->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    layout->addWidget(list);
    w->setLayout(layout);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    int desktopHeight = qApp->primaryScreen()->availableSize().height();
#else
    int desktopHeight = qApp->desktop()->height();
#endif
    int lstr = 0;
    QString longest;

    MidiPort *mp = losMidiPorts.value(((MidiTrack*)m_track)->outPortId());
    if (mp)
    {
        // add GM first, then LS, then SYNTH
        for (iMidiInstrument i = midiInstruments.begin(); i != midiInstruments.end(); ++i)
        {
            // XXX find resource and delete it
            //if(!(*i)->isLOSInstrument())
            {
                QString name(QString("(GM) ").append((*i)->iname()));/*{{{*/
                baseHeight += 18;
                if(name.length() > lstr)
                {
                    lstr = name.length();
                    longest = name;
                }
                QStandardItem* item = new QStandardItem(name);
                item->setCheckable(true);
                item->setData((*i)->iname());
                if (mp->instrument()->iname() == (*i)->iname())
                {
                    //item->setForeground(cl->color());
                    item->setCheckState(Qt::Checked);
                }
                m_listModel->appendRow(item);/*}}}*/
            }
        }

        if(baseHeight > desktopHeight)
            baseHeight = (desktopHeight-50);
        w->setFixedHeight(baseHeight);
        QFontMetrics fm(list->font());
        QRect rect = fm.boundingRect(longest);
        int width = rect.width()+100;
        if(width < 170)
            width = 170;
        w->setFixedWidth(width);
        connect(list, SIGNAL(clicked(const QModelIndex&)), this, SLOT(itemClicked(const QModelIndex&)));
        return w;
    }
    else
        return 0;
}/*}}}*/

void TrackInstrumentMenu::itemClicked(const QModelIndex& index)/*{{{*/
{
    if(m_edit || !m_track)
        return;
    m_edit = true;
    if(index.isValid())
    {
        QStandardItem *item = m_listModel->item(index.row());
        if(item)
        {
            //item->setCheckState(item->checkState() == Qt::Checked ? Qt::Unchecked : Qt::Checked);
            QString instrument = item->data().toString();
            int insType = item->data(Qt::UserRole+2).toInt();
            emit instrumentSelected(m_track->id(), instrument, insType);
            //trackManager->setTrackInstrument(m_track->id(), instrument, insType);

            trigger();
            //FIXME: This is a seriously brutal HACK but its the only way it can get it done
            QKeyEvent *e = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
            QCoreApplication::postEvent(this->parent(), e);

            QKeyEvent *e2 = new QKeyEvent(QEvent::KeyPress, Qt::Key_Enter, Qt::NoModifier);
            QCoreApplication::postEvent(this->parent(), e2);
        }
    }
    m_edit = false;

}/*}}}*/
