//===========================================================
//  LOS
//  Libre Octave Studio
//  (C) Copyright 2011 Andrew Williams & Christopher Cherrett
//===========================================================

#include "FadeCurve.h"
#include "part.h"

FadeCurve::FadeCurve(CurveType type, CurveMode mode, WavePart* p, QObject* parent)
: QObject(parent)
{
    m_type = type;
    m_mode = mode;
    m_part = p;
    m_active = false;
    m_width = 0;
    m_frame = 0;
}

FadeCurve::~FadeCurve()
{
}

bool FadeCurve::active()
{
    return m_active;
}

void FadeCurve::setActive(bool act)
{
    if(m_active != act)
    {
        m_active = act;
        emit stateChanged();
    }
}

void FadeCurve::setWidth(long)
{
}

long FadeCurve::width()
{
    return m_width;
}
