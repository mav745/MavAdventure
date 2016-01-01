#include "cFader.h"

#include <cMainManager.h>
#include <util.h>

#include <cGameRoom.h>

#include <QDebug>

int g_NumFades;

cFader::cFader(QObject *parent) :
	cBaseObject(parent)
{
	m_Color = QVector3D(0,0,0);
	
	m_FadeSec   =     0;
	m_FadeStart =     0;
//	m_MsecHold  =     0;
//	m_FadeSpeed =   0.f;
//	m_FadeCoef  =   0.f;
	m_Fading    = false;
	
	m_Layer = FDL_UPPER;
	m_FadeFlags = FFADE_OUT;
	
	idx = g_NumFades;
	g_NumFades++;
}

cFader::~cFader()
{
	qDebug()<<"~cFader";
}

void cFader::RenderMe()
{
	//qDebug()<<"m_Fading"<<m_Fading;
	if (!m_Fading) return;
	//qDebug()<<"m_FadeSec"<<m_FadeSec;
	if (m_FadeSec == 0.f) return;
	
	//TODO:
	//if fading - increase faded coef by speed*frametime
	//when coef reaches >= 1 - stop fading, emit fadeDone signal
	float coef = (MGR->m_Time - m_FadeStart)/m_FadeSec;
	//qDebug()<<"coef"<<coef;
	float realCoef = coef;
	//qDebug()<<"realCoef"<<realCoef<<idx;
	if (m_FadeFlags & FFADE_IN)
		realCoef = 1.f - realCoef;
	if (m_Layer == FDL_MIDDLE)
	{
		MGR->m_MiddleFade = 
				QVector4D(
					m_Color.x(),
					m_Color.y(),
					m_Color.z(),
					realCoef);
	}
	else if (m_Layer == FDL_UPPER)
	{
		MGR->m_UpperFade = 
				QVector4D(
					m_Color.x(),
					m_Color.y(),
					m_Color.z(),
					realCoef);
	}
	if (coef >= 1.f)
	{
		if (!(m_FadeFlags & FFADE_STAYOUT))
		{
			if (m_Layer == FDL_MIDDLE)
			{
				MGR->m_MiddleFade = 
						QVector4D(0,0,0,0);
			}
			else if (m_Layer == FDL_UPPER)
			{
				MGR->m_UpperFade = 
						QVector4D(0,0,0,0);
			}
		}
		m_Fading = false;
		emit fadeDone();
		
		m_Flags |= FL_KILLME;
	}
}

void cFader::Fade()
{
	m_Fading = true;
	m_FadeStart = MGR->m_Time;
	//m_FadeSpeed = ((float)some_dist)/(float)m_MsecFade;
}
