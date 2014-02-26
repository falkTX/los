//=========================================================
//  LOS
//  Libre Octave Studio
//    $Id: tools.h,v 1.1.1.1 2003/10/27 18:54:49 wschweer Exp $
//  (C) Copyright 1999 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef TOOLBARS_TOOLS_H_INCLUDED
#define TOOLBARS_TOOLS_H_INCLUDED

#include <QActionGroup>
#include <QFrame>
#include <QHBoxLayout>
#include <QList>

class Action;
class QIcon;

//---------------------------------------------------------

enum Tool {
    PointerTool=1,
    PencilTool=2,
    RubberTool=4,
    CutTool=8,
    ScoreTool=16,
    GlueTool=32,
    QuantTool=64,
    DrawTool=128,
    StretchTool=256,
    MuteTool=512,
    AutomationTool=1024,
    MasterTool=2048
};

//const int composerTools = PointerTool | PencilTool | RubberTool | CutTool | GlueTool | StretchTool | MuteTool | AutomationTool;
const int composerTools = PointerTool | PencilTool | RubberTool | CutTool | GlueTool | MuteTool | AutomationTool;
const int masterTools = PointerTool | PencilTool | RubberTool | MasterTool;

struct ToolB {
    QIcon* icon;
    const char* tip;
    const char* ltip;
};

extern ToolB toolList[];

//---------------------------------------------------------
//   EditToolBar
//---------------------------------------------------------

class EditToolBar : public QFrame
{
    Q_OBJECT

public:
    EditToolBar(QWidget*, int, bool addSpacer = false, const char* name = nullptr);
    ~EditToolBar() override;

    int curTool() const;
    QList<QAction*> getActions() const;

signals:
    void toolChanged(int);

public slots:
    void set(int id);
    void setNoUpdate(int id);

private:
    QHBoxLayout  layout;
    QActionGroup action;

    Action** actions;
    int     nactions;

private slots:
    void toolChanged(QAction* action);
};

#endif // TOOLBARS_TOOLS_H_INCLUDED
