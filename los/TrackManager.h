//===========================================================
//  LOS
//  Libre Octave Studio
//  (C) Copyright 2011 Andrew Williams & Christopher Cherrett
//===========================================================

#ifndef _OOM_TRACK_MANAGER_
#define _OOM_TRACKS_MANAGER_

#include <QMap>
#include <QString>
#include <QPair>
#include <QObject>

class MidiTrack;
class Xml;

struct PortConfig
{
    int first;
    qint64 id;
    QString left;
    QString right;
};

struct VirtualTrack {

    VirtualTrack();
    qint64 id;
    QString name;
    QString instrumentName;
    bool createMidiInputDevice;
    bool createMidiOutputDevice;
    bool useInput;
    bool useGlobalInputs;
    bool useOutput;
    int inputChannel;
    int outputChannel;
    QPair<int, QString> inputConfig;
    QPair<int, QString> outputConfig;
    QPair<int, QString> instrumentConfig;

    void write(int level, Xml&) const;
    void read(Xml&);
};

class TrackManager : public QObject{
    Q_OBJECT

    int m_insertPosition;

    int m_allChannelBit;

    int m_midiInPort;
    int m_midiOutPort;

    MidiTrack* m_track;

    QMap<int, QString> m_currentMidiInputList;
    QMap<int, QString> m_currentMidiOutputList;

    QMap<qint64, VirtualTrack*> m_virtualTracks;

signals:
    void trackAdded(qint64);

public:
    TrackManager();
    virtual ~TrackManager() {}

    void setPosition(int);
    qint64 addTrack(VirtualTrack*, int index = -1);
    bool removeTrack(VirtualTrack*);

    void addVirtualTrack(VirtualTrack* vt)
    {
        if(vt)
            m_virtualTracks.insert(vt->id, vt);
    }
    void removeVirtualTrack(qint64 id)
    {
        m_virtualTracks.erase(m_virtualTracks.find(id));
    }
    QMap<qint64, VirtualTrack*> virtualTracks()
    {
        QMap<qint64, VirtualTrack*> rv;
        return rv.unite(m_virtualTracks);
    }
    void setTrackInstrument(qint64, const QString&, int);
    void removeTrack(qint64 id);
    void removeSelectedTracks();
    void write(int level, Xml&) const;
    void read(Xml&);
};

#endif
