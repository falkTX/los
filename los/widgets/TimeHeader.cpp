#include <stdio.h>
#include <values.h>

#include <QtGui>

#include "globals.h"
#include "TimeHeader.h"
#include "song.h"
#include "app.h"
#include "gconfig.h"
#include "poslabel.h"

//
// the bigtime widget
// display is split into several parts to avoid flickering.
//

//---------------------------------------------------------
//   TimeHeader
//---------------------------------------------------------

TimeHeader::TimeHeader(QWidget* parent)
: QFrame(parent)
{
    setObjectName("timeHeader");
    tickmode = true;
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0,0,0,0);
    m_layout->setSpacing(0);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
    setFixedHeight(98);

    QHBoxLayout* timeBox = new QHBoxLayout;
    timeBox->setContentsMargins(18,0,0,0);
    timeBox->setSpacing(0);
    QHBoxLayout* infoBox = new QHBoxLayout;
    infoBox->setContentsMargins(18,0,0,0);
    infoBox->setSpacing(0);

    cursorPos = new PosLabel(this);
    cursorPos->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    cursorPos->setObjectName("thTimeLabel");

    timeLabel = new QLabel(this);
    timeLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    timeLabel->setObjectName("thLengthLabel");

    timeBox->addWidget(cursorPos);
    timeBox->addWidget(timeLabel);

    posLabel = new QLabel(this);
    posLabel->setAlignment(Qt::AlignHCenter|Qt::AlignVCenter);
    posLabel->setObjectName("thBigTimeLabel");
    posLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    infoBox->addWidget(posLabel, 1);

    m_layout->addLayout(infoBox, 1);
    m_layout->addLayout(timeBox);

    setString(MAXINT);
}

//---------------------------------------------------------
//   setString
//---------------------------------------------------------

bool TimeHeader::setString(unsigned v)
{
    if (v == MAXINT)
    {
        timeLabel->setText(QString("----.--.---"));
        posLabel->setText(QString("---:--"));
        return true;
    }

    unsigned absFrame = tempomap.tick2frame(v);
    int bar, beat;
    unsigned tick;
    sigmap.tickValues(v, &bar, &beat, &tick);
    double time = double(absFrame) / double(sampleRate);
    int min = int(time) / 60;
    int sec = int(time) % 60;

    QString s;

    s.sprintf("%04d.%02d.%03d", bar + 1, beat + 1, tick);
    timeLabel->setText(s);

    //s.sprintf("%d:%02d <font color='#565a56' size='12px'>%02d</font>", min, sec, frame);
    s.sprintf("%d:%02d", min, sec);
    posLabel->setText(s);

    return false;
}

//---------------------------------------------------------
//   setPos
//---------------------------------------------------------

void TimeHeader::setPos(int idx, unsigned v, bool)
{
    if (idx == 0)
        setString(v);
}

void TimeHeader::setTime(unsigned tick)
{
    cursorPos->setValue(tick);
}
