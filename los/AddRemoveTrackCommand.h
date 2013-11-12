//========================================================================
//  LOS
//  Libre Octave Studio
//
//    Add Track commands
//
//  (C) Copyright 2012 The Open Octave Project http://www.openoctave.org
//========================================================================

#ifndef ADD_TRACK_H
#define ADD_TRACK_H

#include "traverso_shared/OOMCommand.h"

#include "track.h"

class VirtualTrack;

class AddRemoveTrackCommand : public LOSCommand
{
    Q_OBJECT

public:
    AddRemoveTrackCommand(Track* t, int type, int index = -1);
    AddRemoveTrackCommand(VirtualTrack* t, int type, int index = -1);

    int doAction() override;
    int undoAction() override;

private:
    int fIndex;
    int fType;
    Track* fTrack;
    VirtualTrack* fVirTrack;
};

#endif // ADD_TRACK_H
