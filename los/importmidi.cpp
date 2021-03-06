//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: importmidi.cpp,v 1.26.2.10 2009/11/05 03:14:35 terminator356 Exp $
//
//  (C) Copyright 1999-2003 Werner Schweer (ws@seh.de)
//=========================================================

#include <assert.h>
#include <errno.h>
#include <values.h>

#include <QMessageBox>

#include "app.h"
#include "song.h"
#include "globals.h"
#include "filedialog.h"
#include "midi.h"
#include "midifile.h"
#include "midiport.h"
#include "transport.h"
#include "Arranger.h"
#include "mpevent.h"
#include "event.h"
#include "midictrl.h"
#include "instruments/minstrument.h"
#include "xml.h"
#include "audio.h"
#include "gconfig.h"
#include "utils.h"

//---------------------------------------------------------
//   importMidi
//---------------------------------------------------------

void LOS::importMidi()
{
    QString empty("");
    importMidi(empty);
}

void LOS::importMidi(const QString &file)
{
    QString fn;
    if (file.isEmpty())
    {
        fn = getOpenFileName(lastMidiPath, midi_file_pattern, this,
                tr("LOS: Import Midi"), 0);
        if (fn.isEmpty())
            return;
        lastMidiPath = fn;
    }
    else
        fn = file;

    int n = QMessageBox::question(this, appName,
            tr("Add midi file to current project?\n"),
            tr("&Add to Project"),
            tr("&Replace"),
            tr("&Cancel"), 0, 2);

    switch (n)
    {
        case 0:
            song->invalid = true;
            importMidi(fn, true);
            song->invalid = false;
            song->update();
            break;
        case 1:
            song->invalid = true;
            loadProjectFile(fn, false, false); // replace
            song->invalid = false;
            break;
        default:
            return;
    }
}

//---------------------------------------------------------
//   importMidi
//    return true on error
//---------------------------------------------------------

#if 0
bool LOS::importMidi(const QString name, bool merge)/*{{{*/
{
    bool popenFlag;
    FILE* fp = fileOpen(this, name, QString(".mid"), "r", popenFlag);
    if (fp == 0)
        return true;
    MidiFile mf(fp);
    bool rv = mf.read();
    popenFlag ? pclose(fp) : fclose(fp);
    if (rv)
    {
        QString s(tr("reading midifile\n  "));
        s += name;
        s += tr("\nfailed: ");
        s += mf.error();
        QMessageBox::critical(this, QString("LOS"), s);
        return rv;
    }
    //
    //  evaluate song Type (GM, XG, GS, unknown)
    //
    MType t = song->midiType();
    if (!merge)
    {
        t = mf.mtype();
        song->setMType(t);
    }
    MidiInstrument* instr = 0;
    for (iMidiInstrument i = midiInstruments.begin(); i != midiInstruments.end(); ++i)
    {
        MidiInstrument* mi = *i;
        if ((mi->iname() == "GM" && ((t == MT_UNKNOWN) || (t == MIDI_TYPE_GM)))
                || ((mi->iname() == "GS") && (t == MT_GS))
                || ((mi->iname() == "XG") && (t == MT_XG)))
        {
            instr = mi;
            break;
        }
    }
    if (instr == 0)
    {
        // the standard instrument files (GM, GS, XG) must be present
        printf("no instrument, type %d\n", t);
        return true;
        //abort();
    }

    MidiFileTrackList* etl = mf.trackList();
    int division = mf.division();

    //
    // create MidiTrack and copy events to ->events()
    //    - combine note on/off events
    //    - calculate tick value for internal resolution
    //
    for (iMidiFileTrack t = etl->begin(); t != etl->end(); ++t)
    {
        MPEventList* el = &((*t)->events);
        if (el->empty())
            continue;
        //
        // if we split the track, SYSEX and META events go into
        // the first target track

        bool first = true;
        // somewhat silly and slooow:
        for (int port = 0; port < kMaxMidiPorts; ++port)
        {
            for (int channel = 0; channel < kMaxMidiChannels; ++channel)
            {
                //
                // check if there are any events for port/channel in track:
                //
                iMPEvent i;
                for (i = el->begin(); i != el->end(); ++i)
                {
                    MidiPlayEvent ev = *i;
                    if (ev.type() != ME_SYSEX && ev.type() != ME_META
                            && ev.channel() == channel && ev.port() == port)
                        break;
                }
                if (i == el->end())
                    continue;
                MidiTrack* track = new MidiTrack();
                if ((*t)->isDrumTrack)
                {
                    track->setType(Track::DRUM);
                }

                track->setOutChannel(channel);
                track->setOutPort(port);

                MidiPort* mport = &midiPorts[track->outPort()];
                // this overwrites any instrument set for this port:
                mport->setInstrument(instr);

                EventList* mel = track->events();
                //buildMidiEventList(mel, el, track, division, first);
                // Don't do loops.
                buildMidiEventList(mel, el, track, division, first, false);
                first = false;

                // Hmm. buildMidiEventList already takes care of this.
                // But it seems to work. How? Must test.
                if (channel == 9 && song->midiType() != MT_UNKNOWN)
                {
                    track->setType(Track::DRUM);
                    //
                    // remap drum pitch with drumInmap
                    //
                    EventList* tevents = track->events();
                    for (iEvent i = tevents->begin(); i != tevents->end(); ++i)
                    {
                        Event ev = i->second;
                        if (ev.isNote())
                        {
                            int pitch = drumInmap[ev.pitch()];
                            ev.setPitch(pitch);
                        }
                        else
                            if (ev.type() == Controller)
                        {
                            int ctl = ev.dataA();
                            MidiController *mc = mport->drumController(ctl);
                            if (mc)
                                ev.setA((ctl & ~0xff) | drumInmap[ctl & 0x7f]);
                        }
                    }
                }

                processTrack(track);

                song->insertTrack(track, -1);
            }
        }
        if (first)
        {
            //
            // track does only contain non-channel messages
            // (SYSEX or META)
            //
            MidiTrack* track = new MidiTrack();
            track->setOutChannel(0);
            track->setOutPort(0);
            EventList* mel = track->events();
            //buildMidiEventList(mel, el, track, division, true);
            // Do SysexMeta. Don't do loops.
            buildMidiEventList(mel, el, track, division, true, false);
            processTrack(track);
            song->insertTrack(track, -1);
        }
    }

    if (!merge)
    {
        TrackList* tl = song->tracks();
        if (!tl->empty())
        {
            Track* track = tl->front();
            track->setSelected(true);
        }
        song->initLen();

        int z, n;
        ///sigmap.timesig(0, z, n);
        sigmap.timesig(0, z, n);

        int tempo = tempomap.tempo(0);
        transport->setTimesig(z, n);
        transport->setTempo(tempo);

        bool masterF = !tempomap.empty();
        song->setMasterFlag(masterF);
        transport->setMasterFlag(masterF);

        song->updatePos();

        arranger->reset();
        ///arranger->setMode(int(song->midiType())); // p4.0.7 Tim
    }
    else
    {
        song->initLen();
    }

    return false;
}/*}}}*/
#endif
#if 0
bool LOS::importMidi(const QString name, bool merge)/*{{{*/
{
    bool popenFlag;
    FILE* fp = fileOpen(this, name, QString(".mid"), "r", popenFlag);
    if (fp == 0)
        return true;
    MidiFile mf(fp);
    bool rv = mf.read();
    popenFlag ? pclose(fp) : fclose(fp);
    if (rv)
    {
        QString s(tr("reading midifile\n  "));
        s += name;
        s += tr("\nfailed: ");
        s += mf.error();
        QMessageBox::critical(this, QString("LOS"), s);
        return rv;
    }
    //
    //  evaluate song Type (GM, XG, GS, unknown)
    //
    MType t = song->midiType();
    if (!merge)
    {
        t = mf.mtype();
        song->setMType(t);
    }
    MidiInstrument* instr = 0;
    for (iMidiInstrument i = midiInstruments.begin(); i != midiInstruments.end(); ++i)
    {
        MidiInstrument* mi = *i;
        if ((mi->iname() == "GM" && ((t == MT_UNKNOWN) || (t == MIDI_TYPE_GM)))
                || ((mi->iname() == "GS") && (t == MT_GS))
                || ((mi->iname() == "XG") && (t == MT_XG)))
        {
            instr = mi;
            break;
        }
    }
    if (instr == 0)
    {
        // the standard instrument files (GM, GS, XG) must be present
        printf("no instrument, type %d\n", t);
        return true;
        //abort();
    }

    MidiFileTrackList* etl = mf.trackList();
    int division = mf.division();

    //
    // create MidiTrack and copy events to ->events()
    //    - combine note on/off events
    //    - calculate tick value for internal resolution
    //
    int mPort = getFreeMidiPort();
    for (iMidiFileTrack t = etl->begin(); t != etl->end(); ++t)
    {
        MPEventList* el = &((*t)->events);
        if (el->empty())
            continue;
        //
        // if we split the track, SYSEX and META events go into
        // the first target track

        bool first = true;
        // somewhat silly and slooow:
        QList<QPair<int, int> > eventChannelList;
        if(mPort >= 0 && mPort < kMaxMidiPorts)
        {
            for (int channel = 0; channel < kMaxMidiChannels; ++channel)
            {
                //
                // check if there are any events for port/channel in track:
                //
                iMPEvent i;
                for (i = el->begin(); i != el->end(); ++i)
                {
                    MidiPlayEvent ev = *i;
                    if (ev.type() != ME_SYSEX && ev.type() != ME_META && ev.channel() == channel)
                        break;
                }
                if (i == el->end())
                    continue;
                MidiTrack* track = new MidiTrack();
                track->setDefaultName();
                track->setMasterFlag(true);

                track->setOutChannel(channel);
                track->setOutPort(mPort);

                MidiPort* mport = &midiPorts[track->outPort()];
                // this overwrites any instrument set for this port:
                mport->setInstrument(instr);

                EventList* mel = track->events();
                buildMidiEventList(mel, el, track, division, first, false, false);

                first = false;

                processTrack(track);

                song->insertTrack(track, -1);
                //Create the Audio input side of the track
                Track* input = song->addTrackByName(QString("i").append(track->name()), Track::AUDIO_INPUT, -1, false, false);
                if(input)
                {
                    input->setMasterFlag(false);
                    input->setChainMaster(track->id());
                    track->addManagedTrack(input->id());
                }
            }
        }
        if (first)
        {
            //
            // track does only contain non-channel messages
            // (SYSEX or META)
            //
            MidiTrack* track = new MidiTrack();
            track->setDefaultName();
            track->setMasterFlag(true);
            track->setOutChannel(0);
            track->setOutPort(mPort);
            EventList* mel = track->events();
            //buildMidiEventList(mel, el, track, division, true);
            // Do SysexMeta. Don't do loops.
            buildMidiEventList(mel, el, track, division, true, false, false);
            processTrack(track);
            song->insertTrack(track, -1);
            //Create the Audio input side of the track
            Track* input = song->addTrackByName(QString("i").append(track->name()), Track::AUDIO_INPUT, -1, false, false);
            if(input)
            {
                input->setMasterFlag(false);
                input->setChainMaster(track->id());
                track->addManagedTrack(input->id());
            }
        }
        mPort++;
        //FIXME: Provice a non-iterative way to do this using the new losMidiPorts hash
        //Or maintain a list of configured or inuse ports
        while((&midiPorts[mPort])->device() && mPort < kMaxMidiPorts)
            mPort++;//Just incase we have a configured port after an empty one
    }

    if (!merge)
    {
        TrackList* tl = song->tracks();
        if (!tl->empty())
        {
            Track* track = tl->front();
            track->setSelected(true);
        }
        song->initLen();

        int z, n;
        sigmap.timesig(0, z, n);

        int tempo = tempomap.tempo(0);
        transport->setTimesig(z, n);
        transport->setTempo(tempo);

        bool masterF = !tempomap.empty();
        song->setMasterFlag(masterF);
        transport->setMasterFlag(masterF);

        song->updatePos();

        arranger->reset();
    }
    else
    {
        song->initLen();
    }

    return false;
}/*}}}*/
#endif

bool LOS::importMidi(const QString name, bool merge)/*{{{*/
{
    bool popenFlag;
    FILE* fp = fileOpen(this, name, QString(".mid"), "r", popenFlag);
    if (fp == 0)
        return true;
    MidiFile mf(fp);
    bool rv = mf.read();
    popenFlag ? pclose(fp) : fclose(fp);
    if (rv)
    {
        QString s(tr("reading midifile\n  "));
        s += name;
        s += tr("\nfailed: ");
        s += mf.error();
        QMessageBox::critical(this, QString("LOS"), s);
        return rv;
    }
    //
    //  evaluate song Type (GM, XG, GS, unknown)
    //
    MidiType t = song->midiType();
    if (!merge)
    {
        t = mf.midiType();
        song->setMidiType(t);
    }
    MidiInstrument* instr = 0;
    for (iMidiInstrument i = midiInstruments.begin(); i != midiInstruments.end(); ++i)
    {
        MidiInstrument* mi = *i;
        if ((mi->iname() == "GM" && ((t == MIDI_TYPE_NULL) || (t == MIDI_TYPE_GM))) ||
            (mi->iname() == "GS" &&   t == MIDI_TYPE_GS) ||
            (mi->iname() == "XG" &&   t == MIDI_TYPE_XG))
        {
            instr = mi;
            break;
        }
    }
    if (instr == 0)
    {
        // the standard instrument files (GM, GS, XG) must be present
        printf("no instrument, type %d\n", t);
        return true;
        //abort();
    }

    MidiFileTrackList* etl = mf.trackList();
    int division = mf.division();

    //
    // create MidiTrack and copy events to ->events()
    //    - combine note on/off events
    //    - calculate tick value for internal resolution
    //
    int mPort = getFreeMidiPort();
    for (iMidiFileTrack t = etl->begin(); t != etl->end(); ++t)
    {
        MPEventList* el = &((*t)->events);
        if (el->empty())
            continue;
        //
        // if we split the track, SYSEX and META events go into
        // the first target track

        bool first = true;
        QList<QPair<int, int> > portChannelList;
        iMPEvent i;
        for(i = el->begin(); i != el->end(); i++)
        {
            if (i->type() != ME_SYSEX && i->type() != ME_META)
            {
                int chan = i->channel();
                int port = i->port();
                if(portChannelList.isEmpty() || !portChannelList.contains(qMakePair(chan, port)))
                {
                    portChannelList.append(qMakePair(chan, port));

                    MidiTrack* track = new MidiTrack();
                    track->setDefaultName();
                    track->setMasterFlag(true);

                    if(config.partColorNames[lastTrackPartColorIndex].contains("menu:", Qt::CaseSensitive))
                        lastTrackPartColorIndex ++;

                    track->setDefaultPartColor(lastTrackPartColorIndex);
                    lastTrackPartColorIndex ++;

                    if(lastTrackPartColorIndex == NUM_PARTCOLORS)
                        lastTrackPartColorIndex = 1;

                    //Set track channel so buildMidiEventList can match the event to a channel
                    track->setOutChannel(chan);
                    track->setOutPort(mPort);

                    MidiPort* mport = &midiPorts[track->outPort()];
                    // this overwrites any instrument set for this port:
                    mport->setInstrument(instr);

                    EventList* mel = track->events();
                    buildMidiEventList(mel, el, track, division, first, false, false);

                    first = false;

                    processTrack(track);

                    //Update track to channel 1
                    //99% of all midi we import will be alien to our setup anyway,
                    //so I'm making it easy for the user to just set the Instrument and go
                    track->setOutChannel(0);

                    song->insertTrack(track, -1);
                    mPort++;
                    //FIXME: Provice a non-iterative way to do this using the new losMidiPorts hash
                    //Or maintain a list of configured or inuse ports
                    while((&midiPorts[mPort])->device() && mPort < kMaxMidiPorts)
                        mPort++;//Just incase we have a configured port after an empty one
                }
            }
        }
        if (first)
        {
            //
            // track does only contain non-channel messages
            // (SYSEX or META)
            //
            MidiTrack* track = new MidiTrack();
            track->setDefaultName();
            track->setMasterFlag(true);
            track->setOutChannel(0);
            track->setOutPort(mPort);

            if(config.partColorNames[lastTrackPartColorIndex].contains("menu:", Qt::CaseSensitive))
                lastTrackPartColorIndex ++;

            track->setDefaultPartColor(lastTrackPartColorIndex);
            lastTrackPartColorIndex ++;

            if(lastTrackPartColorIndex == NUM_PARTCOLORS)
                lastTrackPartColorIndex = 1;

            EventList* mel = track->events();
            // Do SysexMeta. Don't do loops.
            // TODO: Properly support sysex dumps
            buildMidiEventList(mel, el, track, division, true, false, false);
            processTrack(track);
            song->insertTrack(track, -1);
            mPort++;
            while((&midiPorts[mPort])->device() && mPort < kMaxMidiPorts)
                mPort++;
        }
    }

    if (!merge)
    {
        MidiTrackList* tl = song->tracks();
        if (!tl->empty())
        {
            MidiTrack* track = tl->front();
            track->setSelected(true);
        }
        song->initLen();

        int z, n;
        sigmap.timesig(0, z, n);

        int tempo = tempomap.tempo(0);
        transport->setTimesig(z, n);
        transport->setTempo(tempo);

        bool masterF = !tempomap.empty();
        song->setMasterFlag(masterF);
        transport->setMasterFlag(masterF);

        song->updatePos();

        arranger->reset();
    }
    else
    {
        song->initLen();
    }

    return false;
}/*}}}*/

//---------------------------------------------------------
//   processTrack
//    divide events into parts
//---------------------------------------------------------

void LOS::processTrack(MidiTrack* track)
{
    EventList* tevents = track->events();
    if (tevents->empty())
        return;

    //---------------------------------------------------
    // Identify Parts
    // The MIDI tracks are broken up into parts
    // A new Part will be located based on track.
    // Events will be aligned on new track
    //---------------------------------------------------

    PartList* pl = track->parts();

    int lastTick = 0;
    for (iEvent i = tevents->begin(); i != tevents->end(); ++i)
    {
        Event event = i->second;
        int epos = event.tick() + event.lenTick();
        if (epos > lastTick)
            lastTick = epos;
    }

    QString partname = track->name();
    int len = song->roundUpBar(lastTick + 1);

    // p3.3.27
    if (config.importMidiSplitParts)
    {

        int bar2, beat;
        unsigned tick;
        sigmap.tickValues(len, &bar2, &beat, &tick);

        int lastOff = 0;
        int st = -1; // start tick current part
        int x1 = 0; // start tick current measure
        int x2 = 0; // end tick current measure

        for (int bar = 0; bar < bar2; ++bar, x1 = x2)
        {
            ///x2 = sigmap.bar2tick(bar+1, 0, 0);
            x2 = sigmap.bar2tick(bar + 1, 0, 0);
            if (lastOff > x2)
            {
                // this measure is busy!
                continue;
            }
            iEvent i1 = tevents->lower_bound(x1);
            iEvent i2 = tevents->lower_bound(x2);

            if (i1 == i2)
            { // empty?
                if (st != -1)
                {
                    MidiPart* part = new MidiPart(track);
                    part->setTick(st);
                    part->setLenTick(x1 - st);
                    // printf("new part %d len: %d\n", st, x1-st);
                    part->setName(partname);
                    pl->add(part);
                    st = -1;
                }
            }
            else
            {
                if (st == -1)
                    st = x1; // begin new  part
                //HACK:
                //lastOff:
                for (iEvent i = i1; i != i2; ++i)
                {
                    Event event = i->second;
                    if (event.type() == Note)
                    {
                        int off = event.tick() + event.lenTick();
                        if (off > lastOff)
                            lastOff = off;
                    }
                }
            }
        }
        if (st != -1)
        {
            MidiPart* part = new MidiPart(track);
            part->setTick(st);
            // printf("new part %d len: %d\n", st, x2-st);
            part->setLenTick(x2 - st);
            part->setName(partname);
            pl->add(part);
        }
    }
    else
    {
        // Just one long part...
        MidiPart* part = new MidiPart(track);
        //part->setTick(st);
        part->setTick(0);
        part->setLenTick(len);
        part->setName(partname);
        pl->add(part);
    }

    //-------------------------------------------------------------
    //    assign events to parts
    //-------------------------------------------------------------

    for (iPart p = pl->begin(); p != pl->end(); ++p)
    {
        MidiPart* part = (MidiPart*) (p->second);
        int stick = part->tick();
        int etick = part->tick() + part->lenTick();
        iEvent r1 = tevents->lower_bound(stick);
        iEvent r2 = tevents->lower_bound(etick);
        int startTick = part->tick();

        EventList* el = part->events();
        for (iEvent i = r1; i != r2; ++i)
        {
            Event ev = i->second;
            int ntick = ev.tick() - startTick;
            ev.setTick(ntick);
            el->add(ev);
        }
        tevents->erase(r1, r2);
    }

    if (tevents->size())
        printf("-----------events left: %zd\n", tevents->size());
    for (iEvent i = tevents->begin(); i != tevents->end(); ++i)
    {
        printf("%d===\n", i->first);
        i->second.dump();
    }
    // all events should be processed:
    assert(tevents->empty());
}

//---------------------------------------------------------
//   importController
//---------------------------------------------------------

void LOS::importController(int channel, MidiPort* mport, int n)
{
    MidiInstrument* instr = mport->instrument();
    MidiCtrlValListList* vll = mport->controller();
    iMidiCtrlValList i = vll->find(channel, n);
    if (i != vll->end())
        return; // controller does already exist
    MidiController* ctrl = 0;
    MidiControllerList* mcl = instr->controller();
    // printf("import Ctrl\n");
    for (iMidiController i = mcl->begin(); i != mcl->end(); ++i)
    {
        MidiController* mc = i->second;
        int cn = mc->num();
        // printf("   %x %x\n", n, cn);
        if (cn == n)
        {
            ctrl = mc;
            break;
        }
        // wildcard?
        if (((cn & 0xff) == 0xff) && ((cn & ~0xff) == (n & ~0xff)))
        {
            ctrl = i->second;
            break;
        }
    }
    if (ctrl == 0)
    {
        printf("controller 0x%x not defined for instrument %s, channel %d\n",
                n, instr->iname().toLatin1().constData(), channel);
        // TODO: register default Controller
        //      MidiController* MidiPort::midiController(int num) const
    }
    MidiCtrlValList* newValList = new MidiCtrlValList(n);
    vll->add(channel, newValList);
}


//---------------------------------------------------------
//   importPart
//---------------------------------------------------------

void LOS::importPart()
{
    unsigned curPos = song->cpos();
    MidiTrackList* tracks = song->tracks();
    MidiTrack* track = 0;
    // Get first selected track:
    for (iMidiTrack i = tracks->begin(); i != tracks->end(); i++)
    {
        MidiTrack* t = *i;
        if (t->selected())
        {
            // Changed by T356. Support mixed .mpt files.
            {
                track = t;
                break;
            }
        }
    }

    if (track)
    {
        bool loadAll;
        QString filename = getOpenFileName(QString(""), part_file_pattern, this, tr("LOS: load part"), &loadAll);
        if (!filename.isEmpty())
        {
            // Make a backup of the current clone list, to retain any 'copy' items,
            //  so that pasting works properly after.
            CloneList copyCloneList = cloneList;
            // Clear the clone list to prevent any dangerous associations with
            //  current non-original parts.
            cloneList.clear();

            importPartToTrack(filename, curPos, track);

            // Restore backup of the clone list, to retain any 'copy' items,
            //  so that pasting works properly after.
            cloneList.clear();
            cloneList = copyCloneList;
        }
    }
    else
    {
        QMessageBox::warning(this, QString("LOS"), tr("No track selected for import"));
    }
}

//---------------------------------------------------------
//   importPartToTrack
//---------------------------------------------------------

void LOS::importPartToTrack(QString& filename, unsigned tick, MidiTrack* track)
{
    bool popenFlag = false;
    FILE* fp = fileOpen(this, filename, ".mpt", "r", popenFlag, false, false);

    if (fp)
    {
        Xml xml = Xml(fp);
        bool firstPart = true;
        int posOffset = 0;
        int notDone = 0;
        int done = 0;

        bool end = false;
        song->startUndo();
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

                        // Read the part.
                        Part* p = 0;
                        p = readXmlPart(xml, track);
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
                            posOffset = tick - p->tick();
                        }
                        p->setTick(p->tick() + posOffset);
                        p->setColorIndex(track->getDefaultPartColor());
                        audio->msgAddPart(p, false);
                    }
                    else
                        xml.unknown("LOS::importPartToTrack");
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
        fclose(fp);
        song->endUndo(SC_PART_INSERTED);

        if (notDone)
        {
            int tot = notDone + done;
            QMessageBox::critical(this, QString("LOS"),
                    QString().setNum(notDone) + (tot > 1 ? (tr(" out of ") + QString().setNum(tot)) : QString("")) +
                    (tot > 1 ? tr(" parts") : tr(" part")) +
                    tr(" could not be imported.\nLikely the track is the wrong type."));
        }

        return;
    }
}
