#ifndef _TRACKVIEWTOOLS_H_
#define _TRACKVIEWTOOLS_H_
#include <QFrame>
#include <QList>

class QHBoxLayout;
class QToolButton;
class QAction;

class TrackViewToolbar : public QFrame
{
	Q_OBJECT
	QHBoxLayout* m_layout;
	QToolButton* m_btnSelect;
	QToolButton* m_btnDraw;
	QToolButton* m_btnRecord;
	QToolButton* m_btnParts;

private slots:
	void songChanged(int);
public slots:
public:
    TrackViewToolbar(QList<QAction*> actions, QWidget* parent = 0);
    virtual ~TrackViewToolbar(){}
	//void setSoloAction(QAction*);
	//void setMuteAction(QAction*);
};

#endif
