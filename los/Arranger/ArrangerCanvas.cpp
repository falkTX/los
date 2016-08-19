//=========================================================
//  LOS
//  Libre Octave Studio

//  (C) Copyright 2011 Andrew Williams
//  (C) Copyright 2011 Christopher Cherrett
//  (C) Copyright 2011 Remon Sijrier
//	14-feb:	Automation Curves:
//		Fixed multiple drawing issues, highlight lazy selected node,
//		implemented CurveNodeSelection to move a selection of nodes.
//
//    $Id: $
//  (C) Copyright 1999 Werner Schweer (ws@seh.de)
//=========================================================

#include <stdio.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <values.h>
#include <uuid/uuid.h>
#include <math.h>

#include <QClipboard>
#include <QToolTip>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QUrl>
#include <QComboBox>
#include <QDomDocument>
#include <QDomElement>
#include <QDomNodeList>
#include <QDomNode>
#include <QMimeData>
#include <QDrag>

#include "widgets/toolbars/tools.h"
#include "ArrangerCanvas.h"
#include "AbstractMidiEditor.h"
#include "globals.h"
#include "icons.h"
#include "event.h"
#include "xml.h"
#include "audio.h"
#include "shortcuts.h"
#include "gconfig.h"
#include "app.h"
#include "filedialog.h"
#include "marker/marker.h"
#include "Arranger.h"
#include "utils.h"
#include "midimonitor.h"
#include "ctrl.h"
#include "traverso_shared/AddRemoveCtrlValues.h"
#include "traverso_shared/CommandGroup.h"
#include "CreateTrackDialog.h"

//---------------------------------------------------------
//   NPart
//---------------------------------------------------------

NPart::NPart(Part* e) : CItem(Event(), e)
{
    int th = track()->height();
    int y = track()->y();
    //printf("NPart::NPart track name:%s, y:%d h:%d\n", track()->name().toLatin1().constData(), y, th);

    setPos(QPoint(e->tick(), y));

    // NOTE: For adjustable border size: If using a two-pixel border width while drawing, use second line.
    //       If one-pixel width, use first line. Tim.
    //setBBox(QRect(e->tick(), y, e->lenTick(), th));
    setBBox(QRect(e->tick(), y + 1, e->lenTick(), th));
}

//---------------------------------------------------------
//   ArrangerCanvas
//---------------------------------------------------------

ArrangerCanvas::ArrangerCanvas(int* r, QWidget* parent, int sx, int sy)
: Canvas(parent, sx, sy)
{
    setAcceptDrops(true);
    _raster = r;
    m_PartZIndex = true;
    build_icons = true;

    setFocusPolicy(Qt::StrongFocus);
    // Defaults:
    lineEditor = 0;
    editMode = false;
    unselectNodes = false;
    trackOffset = 0;
    show_tip = false;

    tracks = song->visibletracks();
    setMouseTracking(true);
    _drag = DRAG_OFF;
    curColorIndex = 0;
    partsChanged();
}

//---------------------------------------------------------
//   y2pitch
//---------------------------------------------------------

int ArrangerCanvas::y2pitch(int y) const
{
    MidiTrackList* tl = song->visibletracks();
    int yy = 0;
    int idx = 0;
    for (iMidiTrack it = tl->begin(); it != tl->end(); ++it, ++idx)
    {
        int h = (*it)->height();
        if (y < yy + h)
            break;
        yy += h;
    }
    return idx;
}

//---------------------------------------------------------
//   pitch2y
//---------------------------------------------------------

int ArrangerCanvas::pitch2y(int p) const
{
    MidiTrackList* tl = song->visibletracks();
    int yy = 0;
    int idx = 0;
    for (iMidiTrack it = tl->begin(); it != tl->end(); ++it, ++idx)
    {
        if (idx == p)
            break;
        yy += (*it)->height();
    }
    return yy;
}

//---------------------------------------------------------
//   leaveEvent
//---------------------------------------------------------

void ArrangerCanvas::leaveEvent(QEvent*)
{
    emit timeChanged(MAXINT);
}

//---------------------------------------------------------
//   returnPressed
//---------------------------------------------------------

void ArrangerCanvas::returnPressed()
{
    if(editMode)
    {
        lineEditor->hide();
        Part* oldPart = editPart->part();
        //Part* newPart = oldPart->clone();
        Part* renamedPart = editPart->part();

        //-- RENAME ALL CLONES IF A CLONE IS RENAMED ----------------------------------
        // Traverse and process the clone chain ring until we arrive at the same part again.
        // The loop is a safety net.
        Part* p = renamedPart;
        int j = renamedPart->cevents()->arefCount();
        if (j > 1) // We have clones. Rename them all
        {
            for (int i = 0; i < j; ++i)
            {
                p->setName(lineEditor->text());
                p = p->nextClone();
                if (p == renamedPart)
                    break;
            }
            // TODO: Fix undo for renaming cloned parts by making a new function to handle it.
            // See undo.cpp and "UndoOp"
            // Possibly the color could use the same function with different args
            song->update();
        }
        else
        {
            // Do it the old way if there are no clones, so undo still works:
            // Indicate do undo, and do port controller values but not clone parts.
            // (This does not work with cloned parts, and also causes lockups)
            Part* newPart = oldPart->clone();
            newPart->setName(lineEditor->text());
            audio->msgChangePart(oldPart, newPart, true, true, false);
            song->update(SC_PART_MODIFIED);
        }

    }
    editMode = false;
}

//---------------------------------------------------------
//   viewMouseDoubleClick
//---------------------------------------------------------

void ArrangerCanvas::viewMouseDoubleClickEvent(QMouseEvent* event)
{
    if (_tool != PointerTool)
    {
        viewMousePressEvent(event);
        return;
    }
    QPoint cpos = event->pos();
    _curItem = _items.find(cpos);
    bool shift = event->modifiers() & Qt::ShiftModifier;
    if (_curItem)
    {
        if (event->button() == Qt::LeftButton && shift)
        {
        }
        else if (event->button() == Qt::LeftButton)
        {
            deselectAll();
            selectItem(_curItem, true);
            emit dclickPart(((NPart*) (_curItem))->track());
        }
    }
        //
        // double click creates new part between left and
        // right mark

    else
    {
    //This changes to song->visibletracks()
        MidiTrackList* tl = song->visibletracks();
        iMidiTrack it;
        int yy = 0;
        int y = event->y();
        for (it = tl->begin(); it != tl->end(); ++it)
        {
            int h = (*it)->height();
            if (y >= yy && y < (yy + h))
                break;
            yy += h;
        }
        if (_pos[2] - _pos[1] > 0 && it != tl->end())
        {
            MidiTrack* track = *it;

                {
                    MidiPart* part = new MidiPart((MidiTrack*) track);
                    part->setTick(_pos[1]);
                    part->setLenTick(_pos[2] - _pos[1]);
                    part->setName(track->name());
                    NPart* np = new NPart(part);
                    _items.add(np);
                    deselectAll();
                    part->setSelected(true);
                    audio->msgAddPart(part);
                }

        }
    }
}

//---------------------------------------------------------
//   startUndo
//---------------------------------------------------------

void ArrangerCanvas::startUndo(DragType)
{
    song->startUndo();
}

//---------------------------------------------------------
//   endUndo
//---------------------------------------------------------

void ArrangerCanvas::endUndo(DragType t, int flags)
{
    song->endUndo(flags | ((t == MOVE_COPY || t == MOVE_CLONE)
            ? SC_PART_INSERTED : SC_PART_MODIFIED));
}

//---------------------------------------------------------
//   moveCanvasItems
//---------------------------------------------------------

void ArrangerCanvas::moveCanvasItems(CItemList& items, int dp, int dx, DragType dtype, int*)
{
    for (iCItem ici = items.begin(); ici != items.end(); ++ici)
    {
        CItem* ci = ici->second;

        int x = ci->pos().x();
        int y = ci->pos().y();
        int nx = x + dx;
        int ny = pitch2y(y2pitch(y) + dp);
        QPoint newpos = raster(QPoint(nx, ny));
        selectItem(ci, true);

        if (moveItem(ci, newpos, dtype))
            ci->move(newpos);
        if (_moving.size() == 1)
        {
            itemReleased(_curItem, newpos);
        }
        if (dtype == MOVE_COPY || dtype == MOVE_CLONE)
            selectItem(ci, false);
    }
}

//---------------------------------------------------------
//   moveItem
//    return false, if copy/move not allowed
//---------------------------------------------------------

bool ArrangerCanvas::moveItem(CItem* item, const QPoint& newpos, DragType t)
{
    tracks = song->visibletracks();
    NPart* npart = (NPart*) item;
    Part* spart = npart->part();
    MidiTrack* track = npart->track();
    unsigned dtick = newpos.x();
    unsigned ntrack = y2pitch(item->mp().y());

    if (tracks->index(track) == ntrack && (dtick == spart->tick()))
    {
        return false;
    }
    //printf("ArrangerCanvas::moveItem ntrack: %d  tracks->size(): %d\n",ntrack,(int)tracks->size());
    MidiTrack* dtrack = 0;
    bool newdest = false;
    if (ntrack >= tracks->size())
    {
        ntrack = tracks->size();
        MidiTrack* newTrack = 0;// = song->addTrack(int(type));

        {
            VirtualTrack* vt;
            CreateTrackDialog *ctdialog = new CreateTrackDialog(&vt, -1, this);
            if(ctdialog->exec() && vt)
            {
                qint64 nid = trackManager->addTrack(vt, -1);
                newTrack = song->findTrackById(nid);
            }
        }

        if(newTrack)
        {
            newdest = true;
            dtrack = newTrack;
            //midiMonitor->msgAddMonitoredTrack(newTrack);
        }
        else
        {
            printf("ArrangerCanvas::moveItem failed to create new track\n");
            return false;
        }

        //printf("ArrangerCanvas::moveItem new track type: %d\n",newTrack->type());

    }
    //FIXME: for some reason we are getting back an invalid track on this next call
    //This is a bug caused by removing the track view update from the undo section of song I think
    //the breakage happens in commit 4484dc5
    //it looks like cloning a part is calling the endMsgCmd in song.cpp and the track is
    //not making it into the undo system or something like that
    if(!dtrack)
        dtrack = tracks->index(ntrack);

    //printf("ArrangerCanvas::moveItem track type is: %d\n",dtrack->type());

    //printf("ArrangerCanvas::moveItem did not crash\n");

    Part* dpart;
    bool clone = (t == MOVE_CLONE || (t == MOVE_COPY && spart->events()->arefCount() > 1));

    if (t == MOVE_MOVE)
    {
        // This doesn't increment aref count, and doesn't chain clones.
        // It also gives the new part a new serial number, but it is
        //  overwritten with the old one by Song::changePart(), from Audio::msgChangePart() below.
        dpart = spart->clone();
        dpart->setTrack(dtrack);
    }
    else
        // This increments aref count if cloned, and chains clones.
        // It also gives the new part a new serial number.
        dpart = dtrack->newPart(spart, clone);

    if(track->name() != dtrack->name() && dpart->name().contains(track->name()))
    {
        QString name = dpart->name();
        dpart->setName(name.replace(track->name(), dtrack->name()));
    }
    dpart->setTick(dtick);

    //printf("ArrangerCanvas::moveItem before add/changePart clone:%d spart:%p events:%p refs:%d Arefs:%d sn:%d dpart:%p events:%p refs:%d Arefs:%d sn:%d\n", clone, spart, spart->events(), spart->events()->refCount(), spart->events()->arefCount(), spart->sn(), dpart, dpart->events(), dpart->events()->refCount(), dpart->events()->arefCount(), dpart->sn());

    if (t == MOVE_MOVE)
        item->setPart(dpart);
    if (t == MOVE_COPY && !clone)
    {
        //
        // Copy Events
        //
        EventList* se = spart->events();
        EventList* de = dpart->events();
        for (iEvent i = se->begin(); i != se->end(); ++i)
        {
            Event oldEvent = i->second;
            Event ev = oldEvent.clone();
            ev.setRightClip(oldEvent.rightClip());
            de->add(ev);
        }
    }
    if (t == MOVE_COPY || t == MOVE_CLONE)
    {
        // These will not increment ref count, and will not chain clones...
        audio->msgAddPart(dpart, false);
    }
    else if (t == MOVE_MOVE)
    {
        dpart->setSelected(spart->selected());
        // These will increment ref count if not a clone, and will chain clones...
            // Indicate no undo, and do port controller values but not clone parts.
            audio->msgChangePart(spart, dpart, false, true, false);

        spart->setSelected(false);
    }
    //printf("ArrangerCanvas::moveItem after add/changePart spart:%p events:%p refs:%d Arefs:%d dpart:%p events:%p refs:%d Arefs:%d\n", spart, spart->events(), spart->events()->refCount(), spart->events()->arefCount(), dpart, dpart->events(), dpart->events()->refCount(), dpart->events()->arefCount());

    if (song->len() < (dpart->lenTick() + dpart->tick()))
        song->setLen(dpart->lenTick() + dpart->tick());
    //endUndo(t);
    if(newdest || track->parts()->empty())
        song->updateTrackViews();
        //emit tracklistChanged();
    return true;
}

//---------------------------------------------------------
//   raster
//---------------------------------------------------------

QPoint ArrangerCanvas::raster(const QPoint& p) const
{
    int y = pitch2y(y2pitch(p.y()));
    int x = p.x();
    if (x < 0)
        x = 0;
    x = sigmap.raster(x, *_raster);
    if (x < 0)
        x = 0;
    return QPoint(x, y);
}

//---------------------------------------------------------
//   partsChanged
//---------------------------------------------------------

void ArrangerCanvas::partsChanged()
{
    tracks = song->visibletracks();
    _items.clear();
    int idx = 0;
    for (iMidiTrack t = tracks->begin(); t != tracks->end(); ++t)
    {
        PartList* pl = (*t)->parts();
        for (iPart i = pl->begin(); i != pl->end(); ++i)
        {
            NPart* np = new NPart(i->second);
            _items.add(np);
            if (i->second->selected())
            {
                selectItem(np, true);
            }
        }
        ++idx;
    }
    redraw();
}

//---------------------------------------------------------
//   updateSelection
//---------------------------------------------------------

void ArrangerCanvas::updateSelection()
{
    for (iCItem i = _items.begin(); i != _items.end(); ++i)
    {
        NPart* part = (NPart*) (i->second);
        part->part()->setSelected(i->second->isSelected());
    }
    emit selectionChanged();
    redraw();
}

//---------------------------------------------------------
//   resizeItem
//---------------------------------------------------------

void ArrangerCanvas::resizeItem(CItem* i, bool noSnap)/*{{{*/
{
    Track* t = ((NPart*) (i))->track();
    Part* p = ((NPart*) (i))->part();

    int pos = p->tick() + i->width();
    int snappedpos = sigmap.raster(pos, *_raster);
    if (noSnap)
    {
        snappedpos = p->tick();
    }
    unsigned int newwidth = snappedpos - p->tick();
    if (newwidth == 0)
        newwidth = sigmap.rasterStep(p->tick(), *_raster);

    song->cmdResizePart(t, p, newwidth);
}/*}}}*/

void ArrangerCanvas::resizeItemLeft(CItem* i, QPoint pos, bool noSnap)/*{{{*/
{
    Track* t = ((NPart*) (i))->track();
    Part* p = ((NPart*) (i))->part();

    int endtick = (p->tick() + p->lenTick());
    int snappedpos = sigmap.raster(i->x(), *_raster);
    //printf("ArrangerCanvas::resizeItemLeft snap pos:%d , nosnap pos: %d\n", snappedpos, i->x());
    if (noSnap)
    {
        snappedpos = i->x();
    }
    //song->cmdResizePartLeft(t, p, snappedpos, endtick, pos);
    //redraw();
}/*}}}*/

CItem* ArrangerCanvas::addPartAtCursor(MidiTrack* track)
{
    if (!track)
        return 0;

    unsigned pos = song->cpos();
    Part* pa = 0;
    NPart* np = 0;
    {
            pa = new MidiPart(track);
            pa->setTick(pos);
            pa->setLenTick(8000);
    }
    pa->setName(track->name());
    pa->setColorIndex(track->getDefaultPartColor());
    np = new NPart(pa);
    return np;
}

//---------------------------------------------------------
//   newItem
//    first create local Item
//---------------------------------------------------------

CItem* ArrangerCanvas::newItem(const QPoint& pos, int)
{
    tracks = song->visibletracks();
    int x = pos.x();
    if (x < 0)
        x = 0;
    x = sigmap.raster(x, *_raster);
    unsigned trackIndex = y2pitch(pos.y());
    if (trackIndex >= tracks->size())
        return 0;
    Track* track = tracks->index(trackIndex);
    if (!track)
        return 0;

    Part* pa = 0;
    NPart* np = 0;

    {
            pa = new MidiPart((MidiTrack*) track);
            pa->setTick(x);
            pa->setLenTick(0);
    }
    pa->setName(track->name());
    pa->setColorIndex(track->getDefaultPartColor());
    np = new NPart(pa);
    return np;
}

//---------------------------------------------------------
//   newItem
//---------------------------------------------------------

void ArrangerCanvas::newItem(CItem* i, bool noSnap)
{
    Part* p = ((NPart*) (i))->part();

    int len = i->width();
    if (!noSnap)
        len = sigmap.raster(len, *_raster);
    if (len == 0)
        len = sigmap.rasterStep(p->tick(), *_raster);
    p->setLenTick(len);
    p->setSelected(true);
    audio->msgAddPart(p);
}

//---------------------------------------------------------
//   deleteItem
//---------------------------------------------------------

bool ArrangerCanvas::deleteItem(CItem* i)
{
    Part* p = ((NPart*) (i))->part();
    audio->msgRemovePart(p); //Invokes songChanged which calls partsChanged which makes it difficult to delete them there
    return true;
}

//---------------------------------------------------------
//   splitItem
//---------------------------------------------------------

void ArrangerCanvas::splitItem(CItem* item, const QPoint& pt)
{
    NPart* np = (NPart*) item;
    Track* t = np->track();
    Part* p = np->part();
    int x = pt.x();
    if (x < 0)
        x = 0;
    song->cmdSplitPart(t, p, sigmap.raster(x, *_raster));
}

//---------------------------------------------------------
//   glueItem
//---------------------------------------------------------

void ArrangerCanvas::glueItem(CItem* item)
{
    NPart* np = (NPart*) item;
    Track* t = np->track();
    Part* p = np->part();
    song->cmdGluePart(t, p);
}

//---------------------------------------------------------
//   genItemPopup
//---------------------------------------------------------

QMenu* ArrangerCanvas::genItemPopup(CItem* item)/*{{{*/
{
    NPart* npart = (NPart*) item;
    QMenu* partPopup = new QMenu(this);
    QMenu* colorPopup = partPopup->addMenu(tr("Part Color"));

    QMenu* colorSub;
    for (int i = 0; i < NUM_PARTCOLORS; ++i)
    {
        QString colorname(config.partColorNames[i]);
        if(colorname.contains("menu:", Qt::CaseSensitive))
        {
            colorSub = colorPopup->addMenu(colorname.replace("menu:", ""));
        }
        else
        {
            if(npart->part()->colorIndex() == i)
            {
                colorname = QString(config.partColorNames[i]);
                colorPopup->setIcon(partColorIconsSelected.at(i));
                colorPopup->setTitle(colorSub->title()+": "+colorname);

                colorname = QString("* "+config.partColorNames[i]);
                QAction *act_color = colorSub->addAction(partColorIconsSelected.at(i), colorname);
                act_color->setData(20 + i);
            }
            else
            {
                colorname = QString("     "+config.partColorNames[i]);
                QAction *act_color = colorSub->addAction(partColorIcons.at(i), colorname);
                act_color->setData(20 + i);
            }
        }
    }
    QString zvalue = QString::number(item->zValue(true));
    QMenu* layerMenu = partPopup->addMenu(tr("Part Layers: ")+zvalue);
    QAction *act_front = layerMenu->addAction(tr("Top"));
    act_front->setData(4003);
    QAction *act_up = layerMenu->addAction(tr("Up"));
    act_up->setData(4002);
    QAction *act_down = layerMenu->addAction(tr("Down"));
    act_down->setData(4001);
    QAction *act_back = layerMenu->addAction(tr("Bottom"));
    act_back->setData(4000);

    QAction *act_cut = partPopup->addAction(*editcutIconSet, tr("C&ut"));
    act_cut->setData(4);
    act_cut->setShortcut(Qt::CTRL + Qt::Key_X);

    QAction *act_copy = partPopup->addAction(*editcopyIconSet, tr("&Copy"));
    act_copy->setData(5);
    act_copy->setShortcut(Qt::CTRL + Qt::Key_C);

    partPopup->addSeparator();
    int rc = npart->part()->events()->arefCount();
    QString st = QString(tr("S&elect "));
    if (rc > 1)
        st += (QString().setNum(rc) + QString(" "));
    st += QString(tr("clones"));
    QAction *act_select = partPopup->addAction(st);
    act_select->setData(18);

    partPopup->addSeparator();

    QAction *act_rename = partPopup->addAction(tr("Rename"));
    act_rename->setData(0);



    QAction *act_delete = partPopup->addAction(QIcon(*deleteIcon), tr("Delete")); // ddskrjo added QIcon to all
    act_delete->setData(1);
    QAction *act_split = partPopup->addAction(QIcon(*cutIcon), tr("Split"));
    act_split->setData(2);
    QAction *act_glue = partPopup->addAction(QIcon(*glueIcon), tr("Glue"));
    act_glue->setData(3);
    QAction *act_declone = partPopup->addAction(tr("De-clone"));
    act_declone->setData(15);

    partPopup->addSeparator();

    {
        {
            QAction *act_pianoroll = partPopup->addAction(QIcon(*pianoIconSet), tr("Piano roll"));
            act_pianoroll->setData(10);
            QAction *act_mlist = partPopup->addAction(QIcon(*edit_listIcon), tr("List editor"));
            act_mlist->setData(12);
            QAction *act_mexport = partPopup->addAction(tr("Export part"));
            act_mexport->setData(16);
        }
    }

    act_select->setEnabled(rc > 1);
    act_delete->setEnabled(true);
    act_cut->setEnabled(true);
    act_declone->setEnabled(rc > 1);

    return partPopup;
}/*}}}*/

//---------------------------------------------------------
//   itemPopup
//---------------------------------------------------------

void ArrangerCanvas::itemPopup(CItem* item, int n, const QPoint& pt)/*{{{*/
{
    PartList* pl = new PartList;
    NPart* npart = (NPart*) (item);
    pl->add(npart->part());
    int zvalue = item->zValue(true);
    switch (n)
    {
        case 0: // rename
        {
            editPart = npart;
            QRect r = map(_curItem->bbox());
            if (lineEditor == 0)
            {
                lineEditor = new QLineEdit(this);
                lineEditor->setFrame(true);
            }
            lineEditor->setText(editPart->name());
            lineEditor->setFocus();
            lineEditor->show();
            lineEditor->setGeometry(r);
            editMode = true;
            }
            break;
        case 1: // delete
            deleteItem(item);
            break;
        case 2: // split
            splitItem(item, pt);
            break;
        case 3: // glue
            glueItem(item);
            break;
        case 4:
            copy(pl);
            audio->msgRemovePart(npart->part());
            break;
        case 5:
            copy(pl);
            break;
        case 10: // pianoroll edit
            emit startEditor(pl, 0);
            return;
        case 12: // list edit
            emit startEditor(pl, 1);
            return;
        case 15: // declone
        {
            Part* spart = npart->part();
            Track* track = npart->track();
            Part* dpart = track->newPart(spart, false);
            //printf("ArrangerCanvas::itemPopup: #1 spart %s %p next:%s %p prev:%s %p\n", spart->name().toLatin1().constData(), spart, spart->nextClone()->name().toLatin1().constData(), spart->nextClone(), spart->prevClone()->name().toLatin1().constData(), spart->prevClone());
            //printf("ArrangerCanvas::itemPopup: #1 dpart %s %p next:%s %p prev:%s %p\n", dpart->name().toLatin1().constData(), dpart, dpart->nextClone()->name().toLatin1().constData(), dpart->nextClone(), dpart->prevClone()->name().toLatin1().constData(), dpart->prevClone());

            EventList* se = spart->events();
            EventList* de = dpart->events();
            for (iEvent i = se->begin(); i != se->end(); ++i)
            {
                Event oldEvent = i->second;
                Event ev = oldEvent.clone();
                de->add(ev);
            }
            song->startUndo();
            // Indicate no undo, and do port controller values but not clone parts.
            audio->msgChangePart(spart, dpart, false, true, false);
            //printf("ArrangerCanvas::itemPopup: #2 spart %s %p next:%s %p prev:%s %p\n", spart->name().toLatin1().constData(), spart, spart->nextClone()->name().toLatin1().constData(), spart->nextClone(), spart->prevClone()->name().toLatin1().constData(), spart->prevClone());
            //printf("ArrangerCanvas::itemPopup: #2 dpart %s %p next:%s %p prev:%s %p\n", dpart->name().toLatin1().constData(), dpart, dpart->nextClone()->name().toLatin1().constData(), dpart->nextClone(), dpart->prevClone()->name().toLatin1().constData(), dpart->prevClone());

            song->endUndo(SC_PART_MODIFIED);
            break; // Has to be break here, right?
        }
        case 16: // Export to file
        {
            const Part* part = item->part();
            bool popenFlag = false;
            //QString fn = getSaveFileName(QString(""), part_file_pattern, this, tr("LOS: save part"));
            QString fn = getSaveFileName(QString(""), part_file_save_pattern, this, tr("LOS: save part"));
            if (!fn.isEmpty())
            {
                FILE* fp = fileOpen(this, fn, ".mpt", "w", popenFlag, false, false);
                if (fp)
                {
                    Xml tmpXml = Xml(fp);
                    //part->write(0, tmpXml);
                    // Write the part. Indicate that it's a copy operation - to add special markers,
                    //  and force full wave paths.
                    part->write(0, tmpXml, true, true);
                    fclose(fp);
                }
            }
            break;
        }

        case 17: // Unused - Do something fun here
        {
            Part* p = item->part();
/*
            EventList* el = p->events();
            QString str = tr("Part name") + ": " + p->name() + "\n" + tr("Files") + ":";
            // This was for the "File Info" option when right-clicking on
            // an audio part in the multitrack view. (not used)
            for (iEvent e = el->begin(); e != el->end(); ++e)
            {
                Event event = e->second;
                SndFileR f = event.sndFile();
                if (f.isNull())
                    continue;
                //str.append("\n" + f.path());
                str.append(QString("\n@") + QString().setNum(event.tick()) + QString(" len:") +
                        QString().setNum(event.lenTick()) + QString(" ") + f.path());
            }
            QMessageBox::information(this, "File info", str, "Ok", 0);
            break;
*/
        }
        case 18: // Select clones
        {
            Part* part = item->part();

            // Traverse and process the clone chain ring until we arrive at the same part again.
            // The loop is a safety net.
            Part* p = part;
            int j = part->cevents()->arefCount();
            if (j > 0)
            {
                for (int i = 0; i < j; ++i)
                {
                    //printf("ArrangerCanvas::itemPopup i:%d %s %p events %p refs:%d arefs:%d\n", i, p->name().toLatin1().constData(), p, part->cevents(), part->cevents()->refCount(), j);

                    p->setSelected(true);
                    p = p->nextClone();
                    if (p == part)
                        break;
                }
                //song->update();
                song->update(SC_SELECTION);
            }

            break;
        }
        case 20 ... NUM_PARTCOLORS + 20:
        {
            curColorIndex = n - 20;
            bool single = true;
            //Loop through all parts and set color on selected:
            CItemList list = getSelectedItems();
            if(list.size() && list.size() > 1)
            {
                single = false;
                for (iCItem i = list.begin(); i != list.end(); i++)
                {
                    i->second->part()->setColorIndex(curColorIndex);
                }
            }

            // If no items selected, use the one clicked on.
            if (single)
                item->part()->setColorIndex(curColorIndex);

            song->update(SC_PART_COLOR_MODIFIED);
            redraw();
            break;
        }
        case 4000: //Move to zero
        {
            item->setZValue(0, true);
            break;
        }
        case 4001: //down one layer
        {
            if(item->zValue(true))
                item->setZValue(zvalue-1, true);
            break;
        }
        case 4002: //up one layer
        {
            item->setZValue(zvalue+1, true);
            break;
        }
        case 4003: // move to top layer
        {
            zvalue = item->part()->track()->maxZIndex();

            item->setZValue(zvalue+1, true);
            break;
        }
        default:
            printf("unknown action %d\n", n);
            break;
    }
    delete pl;
}/*}}}*/

CItemList ArrangerCanvas::getSelectedItems()
{
    CItemList list;
    for (iCItem i = _items.begin(); i != _items.end(); i++)/*{{{*/
    {
        if (i->second->isSelected())
        {
            list.add(i->second);
        }
    }/*}}}*/
    return list;
}

//---------------------------------------------------------
//   viewMousePressEvent
//---------------------------------------------------------

void ArrangerCanvas::mousePress(QMouseEvent* event)/*{{{*/
{
//	qDebug("ArrangerCanvas::mousePress: One pixel = %d ticks", sigmap.raster(1, *_raster));
//	qDebug("ArrangerCanvas::mousePress: width = %d ticks, length: %d", sigmap.raster(width(), *_raster), song->len());
    if (event->modifiers() & Qt::ShiftModifier && _tool != AutomationTool)
    {
        return;
    }

    QPoint pt = event->pos();
    CItem* item = _items.find(pt);

    if (item == 0 && _tool!=AutomationTool)
    {
        return;
    }

    switch (_tool)
    {
        default:
            emit trackChanged(item->part()->track());
            break;
        case PointerTool:
        { //Find out if we are clicking on a fade controll node and override drag move
            emit trackChanged(item->part()->track());
            if(_drag == DRAG_MOVE_START)
            {
            }
        }
        break;
        case CutTool:
            splitItem(item, pt);
            break;
        case GlueTool:
            glueItem(item);
            break;
        case MuteTool:
        {
            NPart* np = (NPart*) item;
            Part* p = np->part();
            p->setMute(!p->mute());
            song->update(SC_MUTE);
            redraw();
            break;
        }
        case AutomationTool:
        {
            break;
        }
    }
}/*}}}*/

//---------------------------------------------------------
//   viewMouseReleaseEvent
//---------------------------------------------------------

void ArrangerCanvas::mouseRelease(const QPoint& pos)/*{{{*/
{
    _drag = DRAG_OFF;
}/*}}}*/

//---------------------------------------------------------
//   viewMouseMoveEvent
//---------------------------------------------------------

void ArrangerCanvas::mouseMove(QMouseEvent* event)/*{{{*/
{
    int x = event->pos().x();
    int y = event->pos().y();
    if (x < 0)
        x = 0;

    emit timeChanged(sigmap.raster(x, *_raster));
}/*}}}*/

//---------------------------------------------------------
//   y2Track
//---------------------------------------------------------

MidiTrack* ArrangerCanvas::y2Track(int y) const
{
    //This changes to song->visibletracks()
    MidiTrackList* l = song->visibletracks();
    int ty = 0;
    for (iMidiTrack it = l->begin(); it != l->end(); ++it)
    {
        int h = (*it)->height();
        if (y >= ty && y < ty + h)
            return *it;
        ty += h;
    }
    return 0;
}


// Return the track Y position, if track was not found, return -1
int ArrangerCanvas::track2Y(MidiTrack * track) const
{
    //This changes to song->visibletracks()
    MidiTrackList* l = song->visibletracks();
    int trackYPos = -1;

    for (iMidiTrack it = l->begin(); it != l->end(); ++it)
    {
        if ((*it) == track)
        {
            return trackYPos;
        }

        int h = (*it)->height();
        trackYPos += h;
    }

    return trackYPos;
}


//---------------------------------------------------------
//   keyPress
//---------------------------------------------------------

void ArrangerCanvas::keyPress(QKeyEvent* event)/*{{{*/
{
    int key = event->key();
    if (editMode)
    {
        if (key == Qt::Key_Return || key == Qt::Key_Enter)
        {
            //qDebug("ArrangerCanvas::keyPress Qt::Key_Return pressed");
            returnPressed();
            return;
        }
        else if (key == Qt::Key_Escape)
        {
            lineEditor->hide();
            editMode = false;
            return;
        }
    }
    //qDebug("ArrangerCanvas::keyPress Not editing");

    if (event->modifiers() & Qt::ShiftModifier)
        key += Qt::SHIFT;
    if (event->modifiers() & Qt::AltModifier)
        key += Qt::ALT;
    if (event->modifiers() & Qt::ControlModifier)
        key += Qt::CTRL;
    if (((QInputEvent*) event)->modifiers() & Qt::MetaModifier)
        key += Qt::META;


    if (key == shortcuts[SHRT_DELETE].key)
    {
        if (getCurrentDrag())
        {
            //printf("dragging!!\n");
            return;
        }

        if (_tool == AutomationTool)
        {
            return;
        }

        song->startUndo();
        audio->msgRemoveParts(song->selectedParts());
        song->endUndo(SC_PART_REMOVED);

        return;
    }
    else if (key == shortcuts[SHRT_POS_DEC].key)
    {
        int spos = _pos[0];
        if (spos > 0)
        {
            spos -= 1; // Nudge by -1, then snap down with raster1.
            spos = sigmap.raster1(spos, *_raster);
        }
        if (spos < 0)
            spos = 0;
        Pos p(spos, true);
        song->setPos(0, p, true, true, true);
        return;
    }
    else if (key == shortcuts[SHRT_POS_INC].key)
    {
        int spos = sigmap.raster2(_pos[0] + 1, *_raster); // Nudge by +1, then snap up with raster2.
        Pos p(spos, true);
        song->setPos(0, p, true, true, true);
        return;
    }
    else if (key == shortcuts[SHRT_POS_DEC_NOSNAP].key)
    {
        int spos = _pos[0] - sigmap.rasterStep(_pos[0], *_raster);
        if (spos < 0)
            spos = 0;
        Pos p(spos, true);
        song->setPos(0, p, true, true, true);
        return;
    }
    else if (key == shortcuts[SHRT_POS_INC_NOSNAP].key)
    {
                Pos p(_pos[0] + sigmap.rasterStep(_pos[0], *_raster), true);
        song->setPos(0, p, true, true, true);
        return;
    }
    else if (key == shortcuts[SHRT_TOOL_POINTER].key)
    {
        emit setUsedTool(PointerTool);
        return;
    }
    else if (key == shortcuts[SHRT_TOOL_PENCIL].key)
    {
        emit setUsedTool(PencilTool);
        return;
    }
    else if (key == shortcuts[SHRT_TOOL_LINEDRAW].key)
    {
        emit setUsedTool(AutomationTool);
        return;
    }
    else if (key == shortcuts[SHRT_TOOL_RUBBER].key)
    {
        emit setUsedTool(RubberTool);
        return;
    }
    else if (key == shortcuts[SHRT_TOOL_SCISSORS].key)
    {
        emit setUsedTool(CutTool);
        return;
    }
    else if (key == shortcuts[SHRT_TOOL_GLUE].key)
    {
        emit setUsedTool(GlueTool);
        return;
    }
    else if (key == shortcuts[SHRT_TOOL_MUTE].key)
    {
        emit setUsedTool(MuteTool);
        return;
    }
    else if (key == shortcuts[SHRT_SEL_TRACK_ABOVE].key)
    {
        emit selectTrackAbove();
        return;
    }
    else if (key == shortcuts[SHRT_SEL_TRACK_BELOW].key)
    {
        emit selectTrackBelow();
        return;
    }
    else if (key == shortcuts[SHRT_SEL_TRACK_ABOVE_ADD].key)
    {
        MidiTrackList* tl = song->visibletracks();
        MidiTrackList selectedTracks = song->getSelectedTracks();
        if (!selectedTracks.size())
        {
            return;
        }

        iMidiTrack t = tl->end();
        while (t != tl->begin())
        {
            --t;

            if ((*t) == *(selectedTracks.begin()))
            {
                if (*t != *(tl->begin()))
                {
                    Track* previous = *(--t);
                    previous->setSelected(true);
                    song->update(SC_SELECTION);
                    return;
                }

            }
        }

        return;
    }
    else if (key == shortcuts[SHRT_SEL_TRACK_BELOW_ADD].key)
    {
        MidiTrackList* tl = song->visibletracks();
        MidiTrackList selectedTracks = song->getSelectedTracks();
        if (!selectedTracks.size())
        {
            return;
        }

        for (iMidiTrack t = tl->begin(); t != tl->end(); ++t)
        {
            if (*t == *(--selectedTracks.end()))
            {
                if (*t != *(--tl->end()))
                {
                    Track* next = *(++t);
                    next->setSelected(true);
                    song->update(SC_SELECTION);
                    return;
                }

            }
        }
        return;
    }
    else if (key == shortcuts[SHRT_SEL_ALL_TRACK].key)
    {
        //printf("Select all tracks called\n");
        MidiTrackList* tl = song->visibletracks();
        MidiTrackList selectedTracks = song->getSelectedTracks();
        bool select = true;
        if (selectedTracks.size() == tl->size())
        {
            select = false;
        }

        for (iMidiTrack t = tl->begin(); t != tl->end(); ++t)
        {
            (*t)->setSelected(select);
        }
        song->update(SC_SELECTION);
        return;
    }
    else if (key == shortcuts[SHRT_TRACK_TOGGLE_SOLO].key)
    {
        MidiTrack* t =los->arranger->curTrack();
        if (t)
        {
            audio->msgSetSolo(t, !t->solo());
            song->update(SC_SOLO);
        }
        return;
    }
    else if (key == shortcuts[SHRT_TRACK_TOGGLE_MUTE].key)
    {
        MidiTrack* t =los->arranger->curTrack();
        if (t)
        {
            t->setMute(!t->mute());
            song->update(SC_MUTE);
        }
        return;
    }
    else if (key == shortcuts[SHRT_MIDI_PANIC].key)
    {
        song->panic();
        return;
    }
    else if (key == shortcuts[SHRT_SET_QUANT_0].key)
    {
        los->arranger->_setRaster(0);
        los->arranger->raster->setCurrentIndex(0);
        return;
    }
    else if (key == shortcuts[SHRT_SET_QUANT_1].key)
    {
        los->arranger->_setRaster(1);
        los->arranger->raster->setCurrentIndex(1);
        return;
    }
    else if (key == shortcuts[SHRT_SET_QUANT_2].key)
    {
        los->arranger->_setRaster(2);
        los->arranger->raster->setCurrentIndex(2);
        return;
    }
    else if (key == shortcuts[SHRT_SET_QUANT_3].key)
    {
        los->arranger->_setRaster(3);
        los->arranger->raster->setCurrentIndex(3);
        return;
    }
    else if (key == shortcuts[SHRT_SET_QUANT_4].key)
    {
        los->arranger->_setRaster(4);
        los->arranger->raster->setCurrentIndex(4);
        return;
    }
    else if (key == shortcuts[SHRT_SET_QUANT_5].key)
    {
        los->arranger->_setRaster(5);
        los->arranger->raster->setCurrentIndex(5);
        return;
    }
    else if (key == shortcuts[SHRT_TRACK_HEIGHT_DEFAULT].key)
    {
        MidiTrackList* tl = song->visibletracks();
        for (iMidiTrack t = tl->begin(); t != tl->end(); ++t)
        {
            MidiTrack* tr = *t;
            if (tr->selected())
            {
                tr->setHeight(DEFAULT_TRACKHEIGHT);
            }
        }
        emit trackHeightChanged();
        song->update(SC_TRACK_MODIFIED);
        return;
    }
    else if (key == shortcuts[SHRT_TRACK_HEIGHT_FULL_SCREEN].key)
    {
        MidiTrackList tl = song->getSelectedTracks();
        for (iMidiTrack t = tl.begin(); t != tl.end(); ++t)
        {
            MidiTrack* tr = *t;
            tr->setHeight(height());
        }
        if (tl.size())
        {
            MidiTrack* tr = *tl.begin();
            los->arranger->verticalScrollSetYpos(track2Y(tr));
        }
        emit trackHeightChanged();
        song->update(SC_TRACK_MODIFIED);
        return;
    }
    else if (key == shortcuts[SHRT_TRACK_HEIGHT_SELECTION_FITS_IN_VIEW].key)
    {
        MidiTrackList tl = song->getSelectedTracks();
        for (iMidiTrack t = tl.begin(); t != tl.end(); ++t)
        {
            MidiTrack* tr = *t;
            tr->setHeight(height() / tl.size());
        }
        if (tl.size())
        {
            MidiTrack* tr = *tl.begin();
            los->arranger->verticalScrollSetYpos(track2Y(tr));
        }
        emit trackHeightChanged();
        song->update(SC_TRACK_MODIFIED);
        return;
    }

    else if (key == shortcuts[SHRT_TRACK_HEIGHT_2].key)
    {
        MidiTrackList* tl = song->visibletracks();
        for (iMidiTrack t = tl->begin(); t != tl->end(); ++t)
        {
            MidiTrack* tr = *t;
            if (tr->selected())
            {
                tr->setHeight(MIN_TRACKHEIGHT);
            }
        }
        emit trackHeightChanged();
        song->update(SC_TRACK_MODIFIED);
        return;
    }
    else if (key == shortcuts[SHRT_TRACK_HEIGHT_3].key)
    {
        MidiTrackList* tl = song->visibletracks();
        for (iMidiTrack t = tl->begin(); t != tl->end(); ++t)
        {
            MidiTrack* tr = *t;
            if (tr->selected())
            {
                tr->setHeight(100);
            }
        }
        emit trackHeightChanged();
        song->update(SC_TRACK_MODIFIED);
        return;
    }
    else if (key == shortcuts[SHRT_TRACK_HEIGHT_4].key)
    {
        MidiTrackList* tl = song->visibletracks();
        for (iMidiTrack t = tl->begin(); t != tl->end(); ++t)
        {
            MidiTrack* tr = *t;
            if (tr->selected())
            {
                tr->setHeight(180);
            }
        }
        emit trackHeightChanged();
        song->update(SC_TRACK_MODIFIED);
        return;
    }
    else if (key == shortcuts[SHRT_TRACK_HEIGHT_5].key)
    {
        MidiTrackList* tl = song->visibletracks();
        for (iMidiTrack t = tl->begin(); t != tl->end(); ++t)
        {
            MidiTrack* tr = *t;
            if (tr->selected())
            {
                tr->setHeight(320);
            }
        }
        emit trackHeightChanged();
        song->update(SC_TRACK_MODIFIED);
        return;
    }
    else if (key == shortcuts[SHRT_TRACK_HEIGHT_6].key)
    {
        MidiTrackList* tl = song->visibletracks();
        for (iMidiTrack t = tl->begin(); t != tl->end(); ++t)
        {
            MidiTrack* tr = *t;
            if (tr->selected())
            {
                tr->setHeight(640);
            }
        }
        emit trackHeightChanged();
        song->update(SC_TRACK_MODIFIED);
        return;
    }
    else if(key == shortcuts[SHRT_INSERT_PART].key)
    {
        MidiTrackList sel = song->getSelectedTracks();
        if(sel.size() == 1)
        {
            MidiTrack* trk = sel.front();
            if(trk)
            {
                //printf("Found one track selected: %s\n", trk->name().toUtf8().constData());
                CItem* ci = addPartAtCursor(trk);
                if(ci)
                {
                    newItem(ci, false);
                }
            }
        }
        return;
    }
    else if(key == shortcuts[SHRT_RENAME_TRACK].key)
    {
        MidiTrackList sel = song->getSelectedTracks();
        if(sel.size() == 1)
        {
            MidiTrack* trk = sel.front();
            if(trk)
            {
                //printf("Found one track selected: %s\n", trk->name().toUtf8().constData());
                emit renameTrack(trk);
            }
        }
        return;
    }
    else if(key == shortcuts[SHRT_MOVE_TRACK_UP].key)
    {
        emit moveSelectedTracks(1);
        return;
    }
    else if(key == shortcuts[SHRT_MOVE_TRACK_DOWN].key)
    {
        emit moveSelectedTracks(-1);
        return;
    }

    //
    // Shortcuts that require selected parts from here
    //
    if (!_curItem)
    {
        if (_items.size() == 0)
        {
            event->ignore(); // give global accelerators a chance
            return;
        }
        for (iCItem i = _items.begin(); i != _items.end(); ++i)
        {
            NPart* part = (NPart*) (i->second);
            if (part->isSelected())
            {
                _curItem = part;
                break;
            }
        }
        if (!_curItem)
            _curItem = (NPart*) _items.begin()->second; // just grab the first part
    }

    CItem* newItem = 0;
    bool singleSelection = isSingleSelection();
    bool add = false;
    //Locators to selection
    if (key == shortcuts[SHRT_LOCATORS_TO_SELECTION].key)
    {
        CItem *leftmost = 0, *rightmost = 0;
        for (iCItem i = _items.begin(); i != _items.end(); i++)
        {
            if (i->second->isSelected())
            {
                // Check leftmost:
                if (!leftmost)
                    leftmost = i->second;
                else
                    if (leftmost->x() > i->second->x())
                        leftmost = i->second;

                // Check rightmost:
                if (!rightmost)
                    rightmost = i->second;
                else
                    if (rightmost->x() < i->second->x())
                        rightmost = i->second;
            }
        }

        int left_tick = leftmost->part()->tick();
        int right_tick = rightmost->part()->tick() + rightmost->part()->lenTick();
        Pos p1(left_tick, true);
        Pos p2(right_tick, true);
        song->setPos(1, p1);
        song->setPos(2, p2);
        return;
    }

        // Select part to the right
    else if (key == shortcuts[SHRT_SEL_RIGHT].key || key == shortcuts[SHRT_SEL_RIGHT_ADD].key)
    {
        if (key == shortcuts[SHRT_SEL_RIGHT_ADD].key)
            add = true;

                Part* part = _curItem->part();
        Track* track = part->track();
        unsigned int tick = part->tick();
        bool afterthis = false;
        for (iCItem i = _items.begin(); i != _items.end(); ++i)
        {
            NPart* npart = (NPart*) (i->second);
            Part* ipart = npart->part();
            if (ipart->track() != track)
                continue;
            if (ipart->tick() < tick)
                continue;
            if (ipart == part)
            {
                afterthis = true;
                continue;
            }
            if (afterthis)
            {
                newItem = i->second;
                break;
            }
        }
    }
        // Select part to the left
    else if (key == shortcuts[SHRT_SEL_LEFT].key || key == shortcuts[SHRT_SEL_LEFT_ADD].key)
    {
        if (key == shortcuts[SHRT_SEL_LEFT_ADD].key)
            add = true;

                Part* part = _curItem->part();
        Track* track = part->track();
        unsigned int tick = part->tick();

        for (iCItem i = _items.begin(); i != _items.end(); ++i)
        {
            NPart* npart = (NPart*) (i->second);
            Part* ipart = npart->part();

            if (ipart->track() != track)
                continue;
            if (ipart->tick() > tick)
                continue;
            if (ipart == part)
                break;
            newItem = i->second;
        }
    }

        // Select nearest part on track above
    else if (key == shortcuts[SHRT_SEL_ABOVE].key || key == shortcuts[SHRT_SEL_ABOVE_ADD].key)
    {
        if (key == shortcuts[SHRT_SEL_ABOVE_ADD].key)
            add = true;
        //To get an idea of which track is above us:
        int stepsize = rmapxDev(1);
        MidiTrack* track = _curItem->part()->track(); //top->part()->track();
        track = y2Track(track->y() - 1);

        //If we're at topmost, leave
        if (!track)
        {
            printf("no track above!\n");
            return;
        }
        int middle = _curItem->x() + _curItem->part()->lenTick() / 2;
        CItem *aboveL = 0, *aboveR = 0;
        //Upper limit: song end, lower limit: song start
        int ulimit = song->len();
        int llimit = 0;

        while (newItem == 0)
        {
            int y = track->y() + 2;
            int xoffset = 0;
            int xleft = middle - xoffset;
            int xright = middle + xoffset;
            while ((xleft > llimit || xright < ulimit) && (aboveL == 0) && (aboveR == 0))
            {
                xoffset += stepsize;
                xleft = middle - xoffset;
                xright = middle + xoffset;
                if (xleft >= 0)
                                        aboveL = _items.find(QPoint(xleft, y));
                if (xright <= ulimit)
                                        aboveR = _items.find(QPoint(xright, y));
            }

            if ((aboveL || aboveR) != 0)
            { //We've hit something
                CItem* above = 0;
                above = (aboveL != 0) ? aboveL : aboveR;
                newItem = above;
            }
            else
            { //We didn't hit anything. Move to track above, if there is one
                track = y2Track(track->y() - 1);
                if (track == 0)
                    return;
            }
        }
        emit trackChanged(track);
    }
        // Select nearest part on track below
    else if (key == shortcuts[SHRT_SEL_BELOW].key || key == shortcuts[SHRT_SEL_BELOW_ADD].key)
    {
        if (key == shortcuts[SHRT_SEL_BELOW_ADD].key)
            add = true;

        //To get an idea of which track is below us:
        int stepsize = rmapxDev(1);
        MidiTrack* track = _curItem->part()->track(); //bottom->part()->track();
        track = y2Track(track->y() + track->height() + 1);
                int middle = _curItem->x() + _curItem->part()->lenTick() / 2;
        //If we're at bottommost, leave
        if (!track)
            return;

        CItem *belowL = 0, *belowR = 0;
        //Upper limit: song end , lower limit: song start
        int ulimit = song->len();
        int llimit = 0;
        while (newItem == 0)
        {
            int y = track->y() + 1;
            int xoffset = 0;
            int xleft = middle - xoffset;
            int xright = middle + xoffset;
            while ((xleft > llimit || xright < ulimit) && (belowL == 0) && (belowR == 0))
            {
                xoffset += stepsize;
                xleft = middle - xoffset;
                xright = middle + xoffset;
                if (xleft >= 0)
                                        belowL = _items.find(QPoint(xleft, y));
                if (xright <= ulimit)
                                        belowR = _items.find(QPoint(xright, y));
            }

            if ((belowL || belowR) != 0)
            { //We've hit something
                CItem* below = 0;
                below = (belowL != 0) ? belowL : belowR;
                newItem = below;
            }
            else
            {
                //Get next track below, or abort if this is the lowest
                track = y2Track(track->y() + track->height() + 1);
                if (track == 0)
                    return;
            }
        }
        emit trackChanged(track);
    }
    else if (key == shortcuts[SHRT_EDIT_PART].key && _curItem)
    { //This should be the other way around - singleSelection first.
        if (!singleSelection)
        {
            event->ignore();
            return;
        }
        PartList* pl = new PartList;
        NPart* npart = (NPart*) (_curItem);
        pl->add(npart->part());

        emit startEditor(pl, 0);
    }
    else if (key == shortcuts[SHRT_INC_POS].key)
    {
            _curItem->setWidth(_curItem->width() + 200);
            redraw();
            resizeItem(_curItem, false);
    }
    else if (key == shortcuts[SHRT_DEC_POS].key)
    {
            Part* part = _curItem->part();
            if(part->lenTick() > 200)
                part->setLenTick(part->lenTick() - 200);
    }
    else if (key == shortcuts[SHRT_SELECT_ALL_NODES].key)
    {
        //printf("Should be select all here\n");
        if (_tool == AutomationTool)
        {
        }
        return;
    }
    else
    {
        event->ignore(); // give global accelerators a chance
        return;
    }


    // Check if anything happened to the selected parts
    if (newItem)
    {
        //If this is a single selection, toggle previous item
        if (singleSelection && !add)
                        selectItem(_curItem, false);
        else if (!add)
            deselectAll();

                _curItem = newItem;
        selectItem(newItem, true);

        //Check if we've hit the upper or lower boundaries of the window. If so, set a new position
        if (newItem->x() < mapxDev(0))
        {
                        int curpos = _pos[0];
            setPos(0, newItem->x(), true);
            setPos(0, curpos, false); //Dummy to put the current position back once we've scrolled
        }
        else if (newItem->x() > mapxDev(width()))
        {
                        int curpos = _pos[0];
            setPos(0, newItem->x(), true);
            setPos(0, curpos, false); //Dummy to put the current position back once we've scrolled
        }
        redraw();
        emit selectionChanged();
    }
}/*}}}*/

//---------------------------------------------------------
//   drawPart
//    draws a part
//---------------------------------------------------------

void ArrangerCanvas::drawItem(QPainter& p, const CItem* item, const QRect& rect)/*{{{*/
{
    int from = rect.x();
    int to = from + rect.width();

    //printf("from %d to %d\n", from,to);
    Part* part = ((NPart*) item)->part();

    MidiPart* mp = 0;

    {
        mp = (MidiPart*) part;
    }

    int i = part->colorIndex();
    QColor partWaveColor(config.partWaveColors[i]);
    QColor partColor(config.partColors[i]);

    QColor partWaveColorAutomation(config.partWaveColorsAutomation[i]);
    QColor partColorAutomation(config.partColorsAutomation[i]);

    int pTick = part->tick();
    from -= pTick;
    to -= pTick;
    if (from < 0)
        from = 0;
    if ((unsigned int) to > part->lenTick())
        to = part->lenTick();

    // Item bounding box x is in tick coordinates, same as rectangle.
    if (item->bbox().intersected(rect).isNull())
    {
        //printf("ArrangerCanvas::drawItem rectangle is null\n");
        return;
    }

    QRect r = item->bbox();

    //printf("ArrangerCanvas::drawItem %s evRefs:%d pTick:%d pLen:%d\nbb.x:%d bb.y:%d bb.w:%d bb.h:%d\n"
    //       "rect.x:%d rect.y:%d rect.w:%d rect.h:%d\nr.x:%d r.y:%d r.w:%d r.h:%d\n",
    //  part->name().toLatin1().constData(), part->events()->arefCount(), pTick, part->lenTick(),
    //  bb.x(), bb.y(), bb.width(), bb.height(),
    //  rect.x(), rect.y(), rect.width(), rect.height(),
    //  r.x(), r.y(), r.width(), r.height());

    p.setPen(Qt::black);
    if(item->isMoving())
    {
        QColor c(Qt::gray);
        c.setAlpha(config.globalAlphaBlend);
        p.setBrush(c);
    }
    else if (part->selected())
    {
        partWaveColor.setAlpha(150);
        partWaveColorAutomation.setAlpha(150);

        p.setBrush(partWaveColor);
        if (mp)
            p.setPen(partColor);
    }
    else
    {
        partColor.setAlpha(150);
        partColorAutomation.setAlpha(150);

        p.setBrush(partColor);
        if (mp)
            p.setPen(partWaveColor);
    }
    
    // Draw clones with slanted corners. 
    
    if ((part->events()->arefCount() > 1)) // Part is a clone, so draw a polygon
    {
        int slant;
        if (xmag < -500)
        {
            slant = 4;
        }
        else
        {
            slant = 7;
        }

        // TODO: Don't  draw a slant that extends beyond the width of the actual part
        // while (slant >= item->width()) // (this doesn't quite work, math is off)
        // {
        //     slant = slant - 1;
        // }

        // Random debugging stuff to get my bearings
        // printf("Item Width: ");
        // printf("%g\n", item->width());
        // printf("H-Zoom Level (xmag): ");
        // printf("%g\n", xmag);
        // printf(" ")

        QPolygon polygon;            
        polygon << QPoint(item->x(), item->y() + slant)
                << QPoint(item->x() - slant * xmag, item->y())      
                << QPoint(item->x() + item->width(), item->y())
                << QPoint(item->x() + item->width(), item->y() + item->height() - slant)
                << QPoint(item->x() + item->width() + slant * xmag, item->y() + item->height())
                << QPoint(item->x(), item->y() + item->height());
        p.drawPolygon(polygon, Qt::OddEvenFill);
    }
    else // Part is not a clone, so draw a normal rectangle
    {
        p.drawRect(QRect(r.x(), r.y(), r.width(), mp ? r.height()-2 : r.height()-1));
    }
    
    if (part->mute() || part->track()->mute())
    {
        QBrush muteBrush;
        muteBrush.setStyle(Qt::HorPattern);
        if(part->selected())
        {
            partColor.setAlpha(120);
            muteBrush.setColor(partColor);
        }
        else
        {
            partWaveColor.setAlpha(120);
            muteBrush.setColor(partWaveColor);
        }
        p.setBrush(muteBrush);

        // NOTE: For one-pixel border use first line For two-pixel border use second.
        p.drawRect(QRect(r.x(), r.y(), r.width(), r.height()-1));
    }

    trackOffset += part->track()->height();
    partColor.setAlpha(255);
    partWaveColor.setAlpha(255);

    if (mp)
    {
        if(part->selected())
            drawMidiPart(p, rect, mp->events(), part->track(), r, mp->tick(), from, to, partColor);
        else
            drawMidiPart(p, rect, mp->events(), part->track(), r, mp->tick(), from, to, partWaveColor);
    }

    if (config.canvasShowPartType & 1)
    { // show names
        // draw name
        QRect rr = map(r);
        rr.setX(rr.x() + 3);
        rr.setHeight(rr.height()-2);
        p.save();
        p.setFont(config.fonts[1]);
        p.setWorldMatrixEnabled(false);
        if(part->selected())
        {
            if (_tool == AutomationTool)
                p.setPen(partColorAutomation);
            else
                p.setPen(partColor);
            p.setFont(QFont("fixed-width", 7, QFont::Bold));
        }
        else
        {
            if (_tool == AutomationTool)
                p.setPen(partWaveColorAutomation);
            else
                p.setPen(partWaveColor);
            p.setFont(QFont("fixed-width", 7, QFont::Bold));
        }
        p.drawText(rr, Qt::AlignBottom | Qt::AlignLeft, part->name());
        p.restore();
    }
}/*}}}*/

//---------------------------------------------------------
//   drawMoving
//    draws moving items
//---------------------------------------------------------

void ArrangerCanvas::drawMoving(QPainter& p, const CItem* item, const QRect&)/*{{{*/
{
    p.setPen(Qt::black);

    Part* part = ((NPart*) item)->part();
    QColor c(config.partColors[part->colorIndex()]);

    c.setAlpha(128); // Fix this regardless of global setting. Should be OK.

    p.setBrush(c);

    // NOTE: For one-pixel border use second line. For two-pixel border use first.
    p.drawRect(item->mp().x(), item->mp().y(), item->width(), item->height());
}/*}}}*/

void ArrangerCanvas::drawMidiPart(QPainter& p, const QRect&, EventList* events, MidiTrack *mt, const QRect& r, int pTick, int from, int to, QColor c)/*{{{*/
{
    if (config.canvasShowPartType & 2) {      // show events
        // Do not allow this, causes segfault.
        if(from <= to)
        {
            p.setPen(c);
            iEvent ito(events->lower_bound(to));

            for (iEvent i = events->lower_bound(from); i != ito; ++i) {
                EventType type = i->second.type();
                if (
                        ((config.canvasShowPartEvent & 1) && (type == Note))
                        || ((config.canvasShowPartEvent & 2) && (type == PAfter))
                        || ((config.canvasShowPartEvent & 4) && (type == Controller))
                        || ((config.canvasShowPartEvent &16) && (type == CAfter))
                        || ((config.canvasShowPartEvent &64) && (type == Sysex || type == Meta))
                        ) {
                    int t = i->first + pTick;
                    int th = mt->height();
                    if(t >= r.left() && t <= r.right())
                        p.drawLine(t, r.y()+2, t, r.y()+th-4);
                }
            }
        }
    }
    else
    {      // show Cakewalk Style
        p.setPen(c);
        iEvent ito(events->lower_bound(to));

        for (iEvent i = events->begin(); i != ito; ++i)
        {
            int t  = i->first + pTick;
            int te = t + i->second.lenTick();

            if (t > (to + pTick))
                break;

            if (te < (from + pTick))
                continue;

            if (te > (to + pTick))
                te = to + pTick;

            EventType type = i->second.type();
            if (type == Note)
            {
                int pitch = i->second.pitch();
                int th = int(mt->height() * 0.75);
                int hoffset = (mt->height() - th ) / 2;
                int y     =  hoffset + (r.y() + th - (pitch * (th) / 127));
                p.drawLine(t, y, te, y);
            }
        }
    }
}/*}}}*/

//---------------------------------------------------------
//   cmd
//---------------------------------------------------------

void ArrangerCanvas::cmd(int cmd)/*{{{*/
{
    PartList pl;
        for (iCItem i = _items.begin(); i != _items.end(); ++i)
    {
        if (!i->second->isSelected())
            continue;
        NPart* npart = (NPart*) (i->second);
        pl.add(npart->part());
    }
    switch (cmd)
    {
        case CMD_CUT_PART:
            copy(&pl);
            song->startUndo();

            bool loop;
            do
            {
                loop = false;
                                for (iCItem i = _items.begin(); i != _items.end(); ++i)
                {
                    if (!i->second->isSelected())
                        continue;
                    NPart* p = (NPart*) (i->second);
                    Part* part = p->part();
                    audio->msgRemovePart(part);

                    loop = true;
                    break;
                }
            } while (loop);
            song->endUndo(SC_PART_REMOVED);
            break;
        case CMD_COPY_PART:
            copy(&pl);
            break;
        case CMD_PASTE_PART:
            paste(false, false);
            break;
        case CMD_PASTE_CLONE_PART:
            paste(true, false);
            break;
        case CMD_PASTE_PART_TO_TRACK:
            paste();
            break;
        case CMD_PASTE_CLONE_PART_TO_TRACK:
            paste(true);
            break;
        case CMD_INSERT_PART:
            paste(false, false, true);
            break;
        case CMD_INSERT_EMPTYMEAS:
        {
            song->startUndo();
            int startPos = song->vcpos();
            int oneMeas = sigmap.ticksMeasure(startPos);
            movePartsTotheRight(startPos, oneMeas);
            song->endUndo(SC_PART_INSERTED);
            break;
        }
        case CMD_REMOVE_SELECTED_AUTOMATION_NODES:
        case CMD_COPY_AUTOMATION_NODES:
        case CMD_PASTE_AUTOMATION_NODES:
        case CMD_SELECT_ALL_AUTOMATION:
        break;
    }
}/*}}}*/

//---------------------------------------------------------
//   copy
//    cut copy paste
//---------------------------------------------------------

void ArrangerCanvas::copy(PartList* pl)/*{{{*/
{
    //printf("void ArrangerCanvas::copy(PartList* pl)\n");
    if (pl->empty())
        return;

    //---------------------------------------------------
    //    write parts as XML into tmp file
    //---------------------------------------------------

    FILE* tmp = tmpfile();
    if (tmp == 0)
    {
        fprintf(stderr, "ArrangerCanvas::copy() fopen failed: %s\n",
                strerror(errno));
        return;
    }
    Xml xml(tmp);

    // Clear the copy clone list.
    cloneList.clear();

    int level = 0;
    int tick = 0;
    for (ciPart p = pl->begin(); p != pl->end(); ++p)
    {
        // Indicate this is a copy operation. Also force full wave paths.
        p->second->write(level, xml, true, true);

        int endTick = p->second->endTick();
        if (endTick > tick)
            tick = endTick;
    }
    Pos p(tick, true);
    song->setPos(0, p);

    //---------------------------------------------------
    //    read tmp file into QTextDrag Object
    //---------------------------------------------------

    fflush(tmp);
    struct stat f_stat;
    if (fstat(fileno(tmp), &f_stat) == -1)
    {
        fprintf(stderr, "ArrangerCanvas::copy() fstat failed:<%s>\n",
                strerror(errno));
        fclose(tmp);
        return;
    }
    int n = f_stat.st_size;
    char* fbuf = (char*) mmap(0, n + 1, PROT_READ | PROT_WRITE,
            MAP_PRIVATE, fileno(tmp), 0);
    fbuf[n] = 0;

    QByteArray data(fbuf);
    QMimeData* md = new QMimeData();

    md->setData("text/x-los-midipartlist", data);

    QApplication::clipboard()->setMimeData(md, QClipboard::Clipboard);

    munmap(fbuf, n);
    fclose(tmp);
}/*}}}*/

//---------------------------------------------------------
//   pasteAt
//---------------------------------------------------------

int ArrangerCanvas::pasteAt(const QString& pt, MidiTrack* track, unsigned int pos, bool clone, bool toTrack)/*{{{*/
{
    //printf("int ArrangerCanvas::pasteAt(const QString& pt, Track* track, int pos)\n");
    QByteArray ba = pt.toLatin1();
    const char* ptxt = ba.constData();
    Xml xml(ptxt);
    bool firstPart = true;
    int posOffset = 0;
    //int  finalPos=0;
    unsigned int finalPos = pos;
    int notDone = 0;
    int done = 0;
    bool end = false;

    //song->startUndo();
    for (;;)
    {
        Xml::Token token = xml.parse();
        const QString& tag = xml.s1();
        switch (token)
        {
            case Xml::Error:
            case Xml::End:
                end = true;
                break;
            case Xml::TagStart:
                if (tag == "part")
                {
                    /*
                    Part* p = 0;
                    if(clone)
                    {
                      if(!(p = readClone(xml, track, toTrack)))
                        break;
                    }
                    else
                    {
                      if (track->type() == Track::MIDI || track->type() == Track::DRUM)
                        p = new MidiPart((MidiTrack*)track);
                      else if (track->type() == Track::WAVE)
                        p = new WavePart((WaveTrack*)track);
                      else
                        break;
                      p->read(xml, 0, toTrack);
                    }
                     */

                    // Read the part.
                    Part* p = 0;
                    p = readXmlPart(xml, track, clone, toTrack);
                    // If it could not be created...
                    if (!p)
                    {
                        // Increment the number of parts not done and break.
                        ++notDone;
                        break;
                    }

                    // Increment the number of parts done.
                    ++done;

                    if (firstPart)
                    {
                        firstPart = false;
                        posOffset = pos - p->tick();
                    }
                    p->setTick(p->tick() + posOffset);
                    if (p->tick() + p->lenTick() > finalPos)
                    {
                        finalPos = p->tick() + p->lenTick();
                    }
                    //pos += p->lenTick();
                    audio->msgAddPart(p, false);
                }
                else
                    xml.unknown("ArrangerCanvas::pasteAt");
                break;
            case Xml::TagEnd:
                break;
            default:
                end = true;
                break;
        }
        if (end)
            break;
    }

    //song->endUndo(SC_PART_INSERTED);
    //return pos;

    if (notDone)
    {
        int tot = notDone + done;
        QMessageBox::critical(this, QString("LOS"),
                QString().setNum(notDone) + (tot > 1 ? (tr(" out of ") + QString().setNum(tot)) : QString("")) +
                (tot > 1 ? tr(" parts") : tr(" part")) +
                tr(" could not be pasted.\nLikely the selected track is the wrong type."));
    }

    return finalPos;
}/*}}}*/

//---------------------------------------------------------
//   paste
//    paste part to current selected track at cpos
//---------------------------------------------------------

void ArrangerCanvas::paste(bool clone, bool toTrack, bool doInsert)/*{{{*/
{
    MidiTrack* track = 0;

    if (doInsert) // logic depends on keeping track of newly selected tracks
        deselectAll();


    // If we want to paste to a selected track...
    if (toTrack)
    {
    //This changes to song->visibletracks()
        MidiTrackList* tl = song->visibletracks();
        for (iMidiTrack i = tl->begin(); i != tl->end(); ++i)
        {
            if ((*i)->selected())
            {
                if (track)
                {
                    QMessageBox::critical(this, QString("LOS"),
                            tr("Cannot paste: multiple tracks selected"));
                    return;
                }
                else
                    track = *i;
            }
        }
        if (track == 0)
        {
            QMessageBox::critical(this, QString("LOS"),
                    tr("Cannot paste: no track selected"));
            return;
        }
    }

    QClipboard* cb = QApplication::clipboard();
    const QMimeData* md = cb->mimeData(QClipboard::Clipboard);

    QString pfx("text/");
    QString mdpl("x-los-midipartlist");
    QString txt;

    if (md->hasFormat(pfx + mdpl))
    {
        txt = cb->text(mdpl, QClipboard::Clipboard);
    }
    else
    {
        QMessageBox::critical(this, QString("LOS"),
                tr("Cannot paste: wrong data type"));
        return;
    }

    int endPos = 0;
    unsigned int startPos = song->vcpos();
    if (!txt.isEmpty())
    {
        song->startUndo();
        endPos = pasteAt(txt, track, startPos, clone, toTrack);
        Pos p(endPos, true);
        song->setPos(0, p);
        if (!doInsert)
            song->endUndo(SC_PART_INSERTED);
    }

    if (doInsert)
    {
        int offset = endPos - startPos;
        movePartsTotheRight(startPos, offset);
        song->endUndo(SC_PART_INSERTED);
    }
}/*}}}*/

//---------------------------------------------------------
//   movePartsToTheRight
//---------------------------------------------------------

void ArrangerCanvas::movePartsTotheRight(unsigned int startTicks, int length)/*{{{*/
{
    // all parts that start after the pasted parts will be moved the entire length of the pasted parts
        for (iCItem i = _items.begin(); i != _items.end(); ++i)
    {
        if (!i->second->isSelected())
        {
            Part* part = i->second->part();
            if (part->tick() >= startTicks)
            {
                //void Audio::msgChangePart(Part* oldPart, Part* newPart, bool doUndoFlag, bool doCtrls, bool doClones)
                Part *newPart = part->clone();
                newPart->setTick(newPart->tick() + length);

                {
                    audio->msgChangePart(part, newPart, false, false, false);
                }

            }
        }
    }
    // perhaps ask if markers should be moved?
    MarkerList *markerlist = song->marker();
    for (iMarker i = markerlist->begin(); i != markerlist->end(); ++i)
    {
        Marker* m = &i->second;
        if (m->tick() >= startTicks)
        {
            Marker *oldMarker = new Marker();
            *oldMarker = *m;
            m->setTick(m->tick() + length);
            song->undoOp(UndoOp::ModifyMarker, oldMarker, m);
        }
    }
}/*}}}*/
//---------------------------------------------------------
//   startDrag
//---------------------------------------------------------

void ArrangerCanvas::startDrag(CItem* item, DragType t)/*{{{*/
{
    //printf("ArrangerCanvas::startDrag(CItem* item, DragType t)\n");
    NPart* p = (NPart*) (item);
    Part* part = p->part();

    //---------------------------------------------------
    //    write part as XML into tmp file
    //---------------------------------------------------

    FILE* tmp = tmpfile();
    if (tmp == 0)
    {
        fprintf(stderr, "ArrangerCanvas::startDrag() fopen failed: %s\n",
                strerror(errno));
        return;
    }
    Xml xml(tmp);
    int level = 0;
    part->write(level, xml);

    //---------------------------------------------------
    //    read tmp file into QTextDrag Object
    //---------------------------------------------------

    fflush(tmp);
    struct stat f_stat;
    if (fstat(fileno(tmp), &f_stat) == -1)
    {
        fprintf(stderr, "ArrangerCanvas::startDrag fstat failed:<%s>\n",
                strerror(errno));
        fclose(tmp);
        return;
    }
    int n = f_stat.st_size + 1;
    char* fbuf = (char*) mmap(0, n, PROT_READ | PROT_WRITE,
            MAP_PRIVATE, fileno(tmp), 0);
    fbuf[n] = 0;

    QByteArray data(fbuf);
    QMimeData* md = new QMimeData();

    md->setData("text/x-los-partlist", data);

    // "Note that setMimeData() assigns ownership of the QMimeData object to the QDrag object.
    //  The QDrag must be constructed on the heap with a parent QWidget to ensure that Qt can
    //  clean up after the drag and drop operation has been completed. "
    QDrag* drag = new QDrag(this);
    drag->setMimeData(md);

    if (t == MOVE_COPY || t == MOVE_CLONE)
        drag->exec(Qt::CopyAction);
    else
        drag->exec(Qt::MoveAction);

    munmap(fbuf, n);
    fclose(tmp);
}/*}}}*/

//---------------------------------------------------------
//   dragEnterEvent
//---------------------------------------------------------

void ArrangerCanvas::dragEnterEvent(QDragEnterEvent* event)
{
    QString text;
    if (event->mimeData()->hasFormat("text/partlist"))
    {
        //qDebug("ArrangerCanvas::dragEnterEvent: Found partList");
        event->acceptProposedAction();
    }
    else if (event->mimeData()->hasUrls())
    {
        text = event->mimeData()->urls()[0].path();

        if (text.endsWith(".wav", Qt::CaseInsensitive) ||
                text.endsWith(".ogg", Qt::CaseInsensitive) ||
                text.endsWith(".mpt", Qt::CaseInsensitive) ||
                text.endsWith(".los", Qt::CaseInsensitive) ||
                text.endsWith(".mid", Qt::CaseInsensitive))
        {
            //THIS IS WHERE .MPT PARTS GET IMPORTED FROM THE CLIP LIST
            //(Possibly also from file manager)
            /*if(text.endsWith(".mpt", Qt::CaseInsensitive))
            {
            }
            else
            {
                SndFile* f = getWave(text, true);
                if(f)
                {
                    int samples = f->samples();
                    //
                }
            }*/
            //qDebug("ArrangerCanvas::dragEnterEvent: Found Audio file");
            event->acceptProposedAction();
        }
        else
            event->ignore();
    }
    else
        event->ignore();
}

//---------------------------------------------------------
//   dragMoveEvent
//---------------------------------------------------------

void ArrangerCanvas::dragMoveEvent(QDragMoveEvent* ev)
{
    //      printf("drag move %x\n", this);
    QPoint p = mapDev(ev->pos());
    int x = sigmap.raster(p.x(), *_raster);
    if(x < 0)
        x = 0;
    emit timeChanged(x);
    //event->acceptProposedAction();
}

//---------------------------------------------------------
//   dragLeaveEvent
//---------------------------------------------------------

void ArrangerCanvas::dragLeaveEvent(QDragLeaveEvent*)
{
    //      printf("drag leave\n");
    //event->acceptProposedAction();
}

//---------------------------------------------------------
//   dropEvent
//---------------------------------------------------------

void ArrangerCanvas::viewDropEvent(QDropEvent* event)
{
    tracks = song->visibletracks();
    //printf("void ArrangerCanvas::viewDropEvent(QDropEvent* event)\n");
    if (event->source() == this)
    {
        printf("local DROP\n");
        //event->ignore();
        return;
    }
    int type = 0; // 0 = unknown, 1 = partlist, 2 = uri-list
    QString text;

    if (event->mimeData()->hasFormat("text/partlist"))
        type = 1;
    else if (event->mimeData()->hasUrls())
        type = 2;
    else
    {
        if (debugMsg && event->mimeData()->formats().size() != 0)
            printf("Drop with unknown format. First format:<%s>\n", event->mimeData()->formats()[0].toLatin1().constData());
        event->ignore();
        return;
    }

    // Make a backup of the current clone list, to retain any 'copy' items,
    //  so that pasting works properly after.
    CloneList copyCloneList = cloneList;
    // Clear the clone list to prevent any dangerous associations with
    //  current non-original parts.
    cloneList.clear();

    if (type == 1)
    {
        text = QString(event->mimeData()->data("text/partlist"));

        int x = sigmap.raster(event->pos().x(), *_raster);
        if (x < 0)
            x = 0;
        unsigned trackNo = y2pitch(event->pos().y());
        MidiTrack* track = 0;
        if (trackNo < tracks->size())
            track = tracks->index(trackNo);
        if (track)
        {
            song->startUndo();
            pasteAt(text, track, x);
            song->endUndo(SC_PART_INSERTED);
        }
    }
    else if (type == 2)
    {
        // TODO:Multiple support. Grab the first one for now.
        text = event->mimeData()->urls()[0].path();

        if (text.endsWith(".mpt", Qt::CaseInsensitive))
        {
            int x = sigmap.raster(event->pos().x(), *_raster);
            if (x < 0)
                x = 0;
            //qDebug("ArrangerCanvas::dropEvent: x: %d cursor x: %d", x, sigmap.raster(QCursor::pos().x(), *_raster));
            unsigned trackNo = y2pitch(event->pos().y());
            Track* track = 0;
            if (trackNo < tracks->size())
                track = tracks->index(trackNo);
            else
            {//Create the track
                event->acceptProposedAction();
                {
                    VirtualTrack* vt;
                    CreateTrackDialog *ctdialog = new CreateTrackDialog(&vt, -1, this);
                    if(ctdialog->exec() && vt)
                    {
                        TrackManager* tman = new TrackManager();
                        qint64 nid = tman->addTrack(vt);
                        track = song->findTrackById(nid);
                    }
                }
            }
        }
        else if (text.endsWith(".los", Qt::CaseInsensitive))
        {
            emit dropSongFile(text);
        }
        else if (text.endsWith(".mid", Qt::CaseInsensitive))
        {
            emit dropMidiFile(text);
        }
        else
        {
            printf("dropped... something...  no hable...\n");
        }
    }

    // Restore backup of the clone list, to retain any 'copy' items,
    //  so that pasting works properly after.
    cloneList.clear();
    cloneList = copyCloneList;
}

//---------------------------------------------------------
//   drawCanvas
//---------------------------------------------------------

void ArrangerCanvas::drawCanvas(QPainter& p, const QRect& rect)
{
    int x = rect.x();
    int y = rect.y();
    int w = rect.width();
    int h = rect.height();

    //////////
    // GRID //
    //////////
    QColor baseColor(config.partCanvasBg.light(104));
    p.setPen(baseColor);

    //--------------------------------
    // vertical lines
    //-------------------------------
    //printf("raster=%d\n", *_raster);
    if (config.canvasShowGrid)
    {
        int bar, beat;
        unsigned tick;

        sigmap.tickValues(x, &bar, &beat, &tick);
        for (;;)
        {
            int xt = sigmap.bar2tick(bar++, 0, 0);
            if (xt >= x + w)
                break;
            if (!((bar - 1) % 4))
                p.setPen(baseColor.dark(115));
            else
                p.setPen(baseColor);
            p.drawLine(xt, y, xt, y + h);

            // append
            int noDivisors = 0;
            if (*_raster == config.division * 2) // 1/2
                noDivisors = 2;
            else if (*_raster == config.division) // 1/4
                noDivisors = 4;
            else if (*_raster == config.division / 2) // 1/8
                noDivisors = 8;
            else if (*_raster == config.division / 4) // 1/16
                noDivisors = 16;
            else if (*_raster == config.division / 8) // 1/16
                noDivisors = 32;
            else if (*_raster == config.division / 16) // 1/16
                noDivisors = 64;

            int r = *_raster;
            int rr = rmapx(r);
            if (*_raster > 1)
            {
                while (rr < 4)
                {
                    r *= 2;
                    rr = rmapx(r);
                    noDivisors = noDivisors / 2;
                }
                p.setPen(baseColor);
                for (int t = 1; t < noDivisors; t++)
                    p.drawLine(xt + r * t, y, xt + r * t, y + h);
            }
        }
    }
    //--------------------------------
    // horizontal lines
    //--------------------------------

    //This changes to song->visibletracks()
    MidiTrackList* tl = song->visibletracks();
    int yy = 0;
    int th;
    for (iMidiTrack it = tl->begin(); it != tl->end(); ++it)
    {
        if (yy > y + h)
            break;
        Track* track = *it;
        th = track->height();
        if (config.canvasShowGrid)
        {
            //printf("ArrangerCanvas::drawCanvas track name:%s, y:%d h:%d\n", track->name().toLatin1().constData(), yy, th);
            p.setPen(baseColor.dark(130));
            p.drawLine(x, yy + th, x + w, yy + th);
            p.setPen(baseColor);
        }
        yy += track->height();
    }

    //If this is the first run build the icons list
    if(build_icons)
    {
        partColorIcons.clear();
        partColorIconsSelected.clear();
        for (int i = 0; i < NUM_PARTCOLORS; ++i)
        {
            partColorIcons.append(colorRect(config.partColors[i], config.partWaveColors[i], 80, 80));
            partColorIconsSelected.append(colorRect(config.partWaveColors[i], config.partColors[i], 80, 80));
        }
        build_icons = false;
    }
}

void ArrangerCanvas::drawTopItem(QPainter& p, const QRect& rect)
{
    int x = rect.x();
    int y = rect.y();
    int w = rect.width();
    int h = rect.height();

    QColor baseColor(config.partCanvasBg.light(104));
    p.setPen(baseColor);

    //This changes to song->visibletracks()
    MidiTrackList* tl = song->visibletracks();

    show_tip = true;
    int yy = 0;
    int th;
    for (iMidiTrack it = tl->begin(); it != tl->end(); ++it)
    {
        if (yy > y + h)
            break;
        MidiTrack* track = *it;
        th = track->height();
        yy += track->height();
    }


    QRect rr = p.worldMatrix().mapRect(rect);

    unsigned int startPos = audio->getStartRecordPos().tick();
    if (song->punchin())
        startPos = song->lpos();
    int start = mapx(startPos);
    int ww = mapx(song->cpos()) - mapx(startPos);

    if (song->cpos() < startPos)
        return;

    if (song->punchout() && song->cpos() > song->rpos())
        return;

    p.save();
    p.resetTransform();
    //Draw part as recorded
    if (song->record() && audio->isPlaying())
    {
        for (iMidiTrack it = tl->begin(); it != tl->end(); ++it)
        {
            MidiTrack* track = *it;
            if (track && track->recordFlag())
            {
                int mypos = track2Y(track)-ypos;
                p.fillRect(start, mypos, ww, track->height(), config.partColors[track->getDefaultPartColor()]);
                p.setPen(Qt::black); //TODO: Fix colors
                p.drawLine(start, mypos, start+ww, mypos);
                p.drawLine(start, mypos+1, start+ww, mypos+1);
                p.drawLine(start, mypos+track->height(), start+ww, mypos+track->height());
                p.drawLine(start, mypos+track->height()-1, start+ww, mypos+track->height()-1);
            }
        }
    }
    p.restore();

    //Draw notes they are recorded
    if (song->record() && audio->isPlaying())
    {
        for (iMidiTrack it = tl->begin(); it != tl->end(); ++it)
        {
            MidiTrack* track = *it;

            if (track->recordFlag())
            {
                MidiTrack *mt = (MidiTrack*)track;
                int mypos = track2Y(track);
                QRect partRect(startPos, mypos, song->cpos()-startPos, track->height());
                EventList newEventList;
                MPEventList *el = mt->mpevents();
                if (!el->size())
                    continue;

                for (iMPEvent i = el->begin(); i != el->end(); ++i)
                {
                    MidiPlayEvent pe = *i;

                    if (pe.isNote() && !pe.isNoteOff()) {
                        Event e(Note);
                        e.setPitch(pe.dataA());
                        e.setTick(pe.time()-startPos);
                        e.setLenTick(song->cpos()-pe.time());
                        e.setC(1);
                        newEventList.add(e);
                    }
                    else if (pe.isNoteOff())
                    {
                        for (iEvent i = newEventList.begin(); i != newEventList.end(); ++i)
                        {
                            Event &e = i->second;
                            if (e.pitch() == pe.dataA() && e.dataC() == 1)
                            {
                                e.setLenTick(pe.time() - e.tick()- startPos);
                                e.setC(0);
                                continue;
                            }
                        }
                    }
                }
                QColor c(0,0,0);
                drawMidiPart(p, rect, &newEventList, mt, partRect, startPos, 0, (song->cpos() - startPos), c);
            }
        }
    }
}

void ArrangerCanvas::drawTooltipText(QPainter& p, /*{{{*/
        const QRect& rr,
        int height,
        double lazySelNodeVal,
        double lazySelNodePrevVal,
        int lazySelNodeFrame,
        bool useTooltip,
        CtrlList* cl)
{
    // calculate the dB value for the dB string.
    double vol = lazySelNodeVal;/*{{{*/
    QString dbString;
    //if(paintTextAsDb)
    if(cl && cl->id() == AC_VOLUME)
    {
        if (vol < 0.0001f)
        {
            vol = 0.0001f;
        }
        vol = 20.0f * log10 (vol);
        if(vol < -60.0f)
            vol = -60.0f;
         dbString += QString::number (vol, 'f', 2) + " dB";
    }
    else
    {
        dbString += QString::number(vol, 'f', 2);
    }
    if(cl->unit().isEmpty() == false)
        dbString.append(" "+cl->unit());
    if(cl->pluginName().isEmpty())
        dbString.append("  "+cl->name());
    else
        dbString.append("  "+cl->name()).append(" : ").append(cl->pluginName());/*}}}*/
    // Set the color for the dB text
    if(!useTooltip)
    {
        p.setPen(QColor(255,255,255,190));
    //p.drawText(mapx(tempomap.frame2tick(lazySelNodeFrame)) + 15, (rr.bottom()-2)-lazySelNodePrevVal*height, dbString);
        int top = (rr.bottom()-20)-lazySelNodePrevVal*height;
        if(top < 0)
            top = 0;
        p.setFont(QFont("fixed-width", 8, QFont::Bold));
        p.drawText(QRect(mapx(tempomap.frame2tick(lazySelNodeFrame)) + 10, top, 400, 60), Qt::TextWordWrap|Qt::AlignLeft, dbString);
    }
    else
    {
        QPoint cursorPos = QCursor::pos();
        QToolTip::showText(cursorPos, dbString, this, QRect(cursorPos.x(), cursorPos.y(), 2, 2));
    }
    //p.drawText(QRect(cursorPos.x() + 10, cursorPos.y(), 400, 60), Qt::TextWordWrap|Qt::AlignLeft, dbString);
}/*}}}*/

void ArrangerCanvas::trackViewChanged()
{
    //printf("ArrangerCanvas::trackViewChanged()\n");
    for (iCItem i = _items.begin(); i != _items.end(); ++i)
    {
        NPart* part = (NPart*) (i->second);
        QRect r = part->bbox();
        part->part()->setSelected(i->second->isSelected());
        Track* track = part->part()->track();
        int y = track->y();
        int th = track->height();

        part->setPos(QPoint(part->part()->tick(), y));

        part->setBBox(QRect(part->part()->tick(), y + 1, part->part()->lenTick(), th));
    }
    emit selectionChanged();
    redraw();
}

Part* ArrangerCanvas::currentCanvasPart()
{
    if(_curPart)
        return _curPart;
    return 0;
}
