//=========================================================
//  LOS
//  Libre Octave Studio
//  $Id: track.h,v 1.39.2.17 2009/12/20 05:00:35 terminator356 Exp $
//
//  (C) Copyright 1999-2004 Werner Schweer (ws@seh.de)
//=========================================================

#ifndef __TRACK_H__
#define __TRACK_H__

#include <QString>
#include <QHash>
#include <QPair>
#include <QUuid>

#include <vector>
#include <algorithm>

#include "part.h"
#include "route.h"
#include "ctrl.h"
#include "globaldefs.hpp"

class Xml;
class MPEventList;
class MidiAssignData;
class MidiPort;
class CCInfo;

struct MonitorLog {
    unsigned pos;
    int value;
};

struct MidiAssignData {/*{{{*/
    MidiTrack* track;
    QHash<int, CCInfo*> midimap;
    int port;
    int preset;
    int channel;
    bool enabled;
    void read(Xml&, MidiTrack*);
    //void write(int, Xml&);
    inline bool operator==(MidiAssignData mad)
    {
        return mad.port == port && mad.track == track;
    }
    inline uint qHash(MidiAssignData m)
    {
        return (m.channel ^ m.port)*qrand();
    }
};/*}}}*/

//---------------------------------------------------------
//   Track
//---------------------------------------------------------

static const int DEFAULT_TRACKHEIGHT = 78;
static const int MIN_TRACKHEIGHT = 50;
static const int MIN_TRACKHEIGHT_SLIDER = 68;
static const int MIN_TRACKHEIGHT_VU = 78;
static const int MIN_TRACKHEIGHT_TOOLS = 150;

class Track
{
private:
    QString _comment;

    PartList _parts;

    void init();
    bool _reminder1;
    bool _reminder2;
    bool _reminder3;

protected:
    qint64 m_id;
    static unsigned int _soloRefCnt;
    static Track* _tmpSoloChainTrack;
    static bool _tmpSoloChainDoIns;
    static bool _tmpSoloChainNoDec;

    RouteList _inRoutes;
    RouteList _outRoutes;

    QString _name;
    int _partDefaultColor;
    bool _recordFlag;
    bool _mute;
    bool _solo;
    unsigned int _internalSolo;
    bool _off;
    int _channels; // 1 - mono, 2 - stereo

    bool _volumeEnCtrl;
    bool _volumeEn2Ctrl;
    bool _panEnCtrl;
    bool _panEn2Ctrl;
    bool _collapsed;
    int _mixerTab;
    int m_maxZIndex;

    int _activity;
    int _lastActivity;
    double _meter[kMaxAudioChannels];
    double _peak[kMaxAudioChannels];

    int _y;
    int _height; // visual height in Composer

    bool _locked;
    bool _selected;
    qint64 m_chainMaster;
    bool m_masterFlag;

    MidiAssignData m_midiassign;
    QList<qint64> m_audioChain;

    bool readProperties(Xml& xml, const QString& tag);
    void writeProperties(int level, Xml& xml) const;

public:
    Track();
    Track(const Track&, bool cloneParts);

    virtual ~Track()
    {
    };
    virtual Track & operator=(const Track& t);

    bool getReminder1()
    {
        return _reminder1;
    }
    void setReminder1(bool r)
    {
        _reminder1 = r;
    }
    bool getReminder2()
    {
        return _reminder2;
    }
    void setReminder2(bool r)
    {
        _reminder2 = r;
    }
    bool getReminder3()
    {
        return _reminder3;
    }
    void setReminder3(bool r)
    {
        _reminder3 = r;
    }
    bool collapsed() { return _collapsed; }
    void setCollapsed(bool f) { _collapsed = f; }
    int mixerTab(){return _mixerTab;}
    void setMixerTab(int i){_mixerTab = i;}
    void setMaxZIndex(int i)
    {
        if(i > m_maxZIndex)
            m_maxZIndex = i;
    }
    int maxZIndex() { return m_maxZIndex; }

    QString comment() const
    {
        return _comment;
    }

    void setComment(const QString& s)
    {
        _comment = s;
    }

    void setChainMaster(qint64 val){m_chainMaster = val;}
    qint64 chainMaster(){return m_chainMaster;}
    void setMasterFlag(bool val){m_masterFlag = val;}
    bool masterFlag() {return m_masterFlag;}
    void addManagedTrack(qint64 id)
    {
        if(!chainContains(id))
            m_audioChain.append(id);
    }
    QList<qint64>* audioChain() {return &m_audioChain;}
    bool chainContains(qint64 id)
    {
        return (!m_audioChain.isEmpty() && m_audioChain.contains(id));
    }
    bool hasChildren()
    {
        return !m_audioChain.isEmpty();
    }

    MidiAssignData* midiAssign() { return &m_midiassign; }

    qint64 id()
    {
        return  m_id;
    }

    int y() const;

    void setY(int n)
    {
        _y = n;
    }

    int height() const
    {
        return _height;
    }

    void setHeight(int n)
    {
        _height = n;
    }

    bool selected() const
    {
        return _selected;
    }

    void setSelected(bool f);

    void deselectParts();

    bool locked() const
    {
        return _locked;
    }

    void setLocked(bool b)
    {
        _locked = b;
    }

    bool volumeControllerEnabled() const
    {
        return _volumeEnCtrl;
    }

    bool volumeControllerEnabled2() const
    {
        return _volumeEn2Ctrl;
    }

    bool panControllerEnabled() const
    {
        return _panEnCtrl;
    }

    bool panControllerEnabled2() const
    {
        return _panEn2Ctrl;
    }

    void enableVolumeController(bool b)
    {
        _volumeEnCtrl = b;
    }

    void enable2VolumeController(bool b)
    {
        _volumeEn2Ctrl = b;
    }

    void enablePanController(bool b)
    {
        _panEnCtrl = b;
    }

    void enable2PanController(bool b)
    {
        _panEn2Ctrl = b;
    }
    void clearRecAutomation(bool clearList);

    const QString& name() const
    {
        return _name;
    }

    virtual void setName(const QString& s)
    {
        _name = s;
    }

    // routing

    RouteList* inRoutes()
    {
        return &_inRoutes;
    }

    RouteList* outRoutes()
    {
        return &_outRoutes;
    }

    bool noInRoute() const
    {
        return _inRoutes.empty();
    }

    bool noOutRoute() const
    {
        return _outRoutes.empty();
    }
    void writeRouting(int, Xml&) const;

    PartList* parts()
    {
        return &_parts;
    }

    const PartList* cparts() const
    {
        return &_parts;
    }
    Part* findPart(unsigned tick);
    iPart addPart(Part* p);

    virtual void write(int, Xml&) const = 0;

    virtual Track* newTrack() const = 0;
    virtual Track* clone(bool CloneParts) const = 0;

    virtual bool setRecordFlag1(bool f, bool monitor = false) = 0;
    virtual void setRecordFlag2(bool f, bool monitor = false) = 0;

    virtual Part* newPart(Part*p = 0, bool clone = false) = 0;
    void dump() const;
    virtual void splitPart(Part*, int, Part*&, Part*&);

    virtual void setMute(bool val, bool monitor = false);
    virtual void setOff(bool val);
    virtual void updateSoloStates(bool noDec) = 0;
    virtual void updateInternalSoloStates();
    void updateSoloState();
    void setInternalSolo(unsigned int val);
    static void clearSoloRefCounts();
    virtual void setSolo(bool val, bool monitor = false) = 0;
    virtual bool isMute() const = 0;

    unsigned int internalSolo() const
    {
        return _internalSolo;
    }

    bool soloMode() const
    {
        return _soloRefCnt;
    }

    bool solo() const
    {
        return _solo;
    }

    bool mute() const
    {
        return _mute;
    }

    bool off() const
    {
        return _off;
    }

    bool recordFlag() const
    {
        return _recordFlag;
    }

    void setDefaultPartColor(int pc)
    {
        _partDefaultColor = pc;
    }
    int getDefaultPartColor()
    {
        return _partDefaultColor;
    }

    int activity()
    {
        return _activity;
    }

    void setActivity(int v)
    {
        _activity = v;
    }

    int lastActivity()
    {
        return _lastActivity;
    }

    void setLastActivity(int v)
    {
        _lastActivity = v;
    }

    void addActivity(int v)
    {
        _activity += v;
    }
    void resetPeaks();
    static void resetAllMeter();

    double meter(int ch) const
    {
        return _meter[ch];
    }

    double peak(int ch) const
    {
        return _peak[ch];
    }
    void resetMeter();

    bool readProperty(Xml& xml, const QString& tag);
    void setDefaultName();
    static QString getValidName(QString name, bool isdefault = false);

    int channels() const
    {
        return _channels;
    }
    virtual void setChannels(int n);

    virtual bool canRecord() const
    {
        return false;
    }
};

//---------------------------------------------------------
//   MidiTrack
//---------------------------------------------------------

class MidiTrack : public Track
{
    int _outPort;
    qint64 _outPortId;
    int _outChannel;
    bool _recEcho; // For midi (and audio). Whether to echo incoming record events to output device.

    EventList* _events; // tmp Events during midi import
    MPEventList* _mpevents; // tmp Events druring recording
    QHash<int, QList<MonitorLog> > m_monitorBuffer;
    SamplerData* m_samplerData;

public:
    MidiTrack();
    MidiTrack(const MidiTrack&, bool cloneParts);
    virtual ~MidiTrack();

    void init();

    bool transpose;
    int transposition;
    int velocity;
    int delay;
    int len;
    int compression;

    int getTransposition();
    QList<MonitorLog> getMonitorBuffer(int ctrl)
    {
        if(m_monitorBuffer.isEmpty() || !m_monitorBuffer.contains(ctrl))
        {
            QList<MonitorLog> list;
            m_monitorBuffer.insert(ctrl, list);
        }
        QList<MonitorLog> list1 = m_monitorBuffer.value(ctrl);
        return list1;
    }

    virtual bool setRecordFlag1(bool f, bool monitor = false);

    virtual void setRecordFlag2(bool, bool monitor = false);

    EventList* events() const
    {
        return _events;
    }

    MPEventList* mpevents() const
    {
        return _mpevents;
    }

    virtual void read(Xml&);
    virtual void write(int, Xml&) const;

    virtual MidiTrack* newTrack() const
    {
        return new MidiTrack();
    }

    virtual MidiTrack* clone(bool cloneParts) const
    {
        return new MidiTrack(*this, cloneParts);
    }
    virtual Part* newPart(Part*p = 0, bool clone = false);

    void setOutChannel(int i)
    {
        _outChannel = i;
    }

    void setOutPort(int i);
    void setOutPortId(qint64 i);
    void setOutChanAndUpdate(int i);
    void setOutPortAndUpdate(int i);
    void setOutPortIdAndUpdate(qint64 i);

    // Backward compatibility: For reading old songs.
    void setInPortAndChannelMask(unsigned int /*portmask*/, int /*chanmask*/);

    void setRecEcho(bool b)
    {
        _recEcho = b;
    }

    int outPort() const
    {
        return _outPort;
    }

    qint64 outPortId() const
    {
        return _outPortId;
    }

    int outChannel() const
    {
        return _outChannel;
    }

    bool recEcho() const
    {
        return _recEcho;
    }
    SamplerData* samplerData() {return m_samplerData;}
    void setSamplerData(SamplerData* data){ m_samplerData = data;}

    virtual bool isMute() const;
    virtual void setSolo(bool val, bool monitor = false);
    virtual void updateSoloStates(bool noDec);
    virtual void updateInternalSoloStates();

    virtual bool canRecord() const
    {
        return true;
    }
};

//---------------------------------------------------------
//   TrackList
//---------------------------------------------------------

template<class T> class tracklist : public std::vector<MidiTrack*>
{
    typedef std::vector<MidiTrack*> vlist;

public:

    class iterator : public vlist::iterator
    {
    public:

        iterator() : vlist::iterator()
        {
        }

        iterator(vlist::iterator i) : vlist::iterator(i)
        {
        }

        T operator*()
        {
            return (T) (**((vlist::iterator*)this));
        }

        iterator operator++(int)
        {
            return iterator((*(vlist::iterator*)this).operator++(0));
        }

        iterator & operator++()
        {
            return (iterator&) ((*(vlist::iterator*)this).operator++());
        }
    };

    class const_iterator : public vlist::const_iterator
    {
    public:

        const_iterator() : vlist::const_iterator()
        {
        }

        const_iterator(vlist::const_iterator i) : vlist::const_iterator(i)
        {
        }

        const_iterator(vlist::iterator i) : vlist::const_iterator(i)
        {
        }

        const T operator*() const
        {
            return (T) (**((vlist::const_iterator*)this));
        }
    };

    class reverse_iterator : public vlist::reverse_iterator
    {
    public:

        reverse_iterator() : vlist::reverse_iterator()
        {
        }

        reverse_iterator(vlist::reverse_iterator i) : vlist::reverse_iterator(i)
        {
        }

        T operator*()
        {
            return (T) (**((vlist::reverse_iterator*)this));
        }
    };

    tracklist() : vlist()
    {
    }

    virtual ~tracklist()
    {
    }

    void push_back(T v)
    {
        vlist::push_back(v);
    }

    iterator begin()
    {
        return vlist::begin();
    }

    iterator end()
    {
        return vlist::end();
    }

    const_iterator begin() const
    {
        return vlist::begin();
    }

    const_iterator end() const
    {
        return vlist::end();
    }

    reverse_iterator rbegin()
    {
        return vlist::rbegin();
    }

    reverse_iterator rend()
    {
        return vlist::rend();
    }

    T& back() const
    {
        return (T&) (vlist::back());
    }

    T& front() const
    {
        return (T&) (vlist::front());
    }

    iterator find(const MidiTrack* t)
    {
        return std::find(begin(), end(), t);
    }

    const_iterator find(const MidiTrack* t) const
    {
        return std::find(begin(), end(), t);
    }

    unsigned index(const MidiTrack* t) const
    {
        unsigned n = 0;
        for (vlist::const_iterator i = begin(); i != end(); ++i, ++n) {
            if (*i == t)
                return n;
        }
        return -1;
    }

    T index(int k) const
    {
        return (*this)[k];
    }

    iterator index2iterator(int k)
    {
        if ((unsigned) k >= size())
            return end();
        return begin() + k;
    }

    void erase(MidiTrack* t)
    {
        vlist::erase(find(t));
    }

    void clearDelete()
    {
        for (vlist::iterator i = begin(); i != end(); ++i)
            delete *i;
        vlist::clear();
    }

    void erase(vlist::iterator i)
    {
        vlist::erase(i);
    }

    void replace(MidiTrack* ot, MidiTrack* nt)
    {
        for (vlist::iterator i = begin(); i != end(); ++i) {
            if (*i == ot) {
                *i = nt;
                return;
            }
        }
    }
};

//typedef tracklist<Track*> TrackList;
//typedef TrackList::iterator iTrack;
//typedef TrackList::const_iterator ciTrack;

typedef tracklist<MidiTrack*> MidiTrackList;

typedef tracklist<MidiTrack*>::iterator iMidiTrack;
typedef tracklist<MidiTrack*>::const_iterator ciMidiTrack;
typedef tracklist<MidiTrack*> MidiTrackList;

extern void addPortCtrlEvents(MidiTrack* t);
extern void removePortCtrlEvents(MidiTrack* t);

#endif

