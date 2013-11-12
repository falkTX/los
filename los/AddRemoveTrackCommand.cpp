//=========================================================
//  LOS
//  Libre Octave Studio
//
//    add ctrl value command class
//
//  (C) Copyright 2011 Remon Sijrier
//=========================================================

#include "AddRemoveTrackCommand.h"
#include "TrackManager.h"

AddRemoveTrackCommand::AddRemoveTrackCommand(Track* t, int type, int index)
    : LOSCommand(tr("Add Track")),
      fIndex(index),
      fType(type),
      fTrack(t),
      fVirTrack(nullptr)
{
}

AddRemoveTrackCommand::AddRemoveTrackCommand(VirtualTrack* vt, int type, int index)
    : LOSCommand(tr("Add Track")),
      fIndex(index),
      fType(type),
      fTrack(nullptr),
      fVirTrack(vt)
{
}

int AddRemoveTrackCommand::doAction()
{
    if (fType == ADD)
    {
        //Perform add track actions
    }
    else
    {
        //Perform delete track actions
    }

    return 1;
}

int AddRemoveTrackCommand::undoAction()
{
    if (fType == ADD)
    {
        //Perform delete track action
    }
    else
    {
        //Perform add track actions
    }

    return 1;
}
