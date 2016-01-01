#include "cBaseTrigger.h"
#include <cMainManager.h>
#include <cGameRoom.h>

#include <util.h>

#include <QDebug>

cBaseTrigger::cBaseTrigger(QObject *parent) :
	cBaseObject(parent)
{
	m_Target.clear();
	m_Visible = !false;
	m_Orig = QVector2D(0,0);
	m_Maxs = QVector2D(-999999,-999999);
	m_Mins = QVector2D( 999999, 999999);
}

cBaseTrigger::~cBaseTrigger()
{
	for(int i=m_Parts.size()-1;i>=0;i--)
		delete m_Parts[i];
	
	m_pMyRoom->m_pTriggers.remove(m_pMyRoom->m_pTriggers.indexOf(this));
}

void cBaseTrigger::Spawn()
{
	m_pMyRoom->m_pTriggers.push_back(this);
}

void cBaseTrigger::ScrollMe(const QVector2D &scrll)
{
	m_Orig = scrll;
	
	m_Maxs += m_Orig;
	m_Mins += m_Orig;
	
	int R,L,HMarg, U,D,VMarg;
	if (!m_pMyRoom->m_Block[EBS_LEFT] && !m_pMyRoom->m_Block[EBS_RIGHT])
	{
		HMarg = m_pMyRoom->m_Size[XX]-640;
		HMarg /= 2;
		R = 640+HMarg;
		L = -HMarg;
	
		if (std::min(m_Maxs[XX],m_Mins[XX]) > R)
		{
			m_Mins[XX] -=  m_pMyRoom->m_Size[XX];
			m_Maxs[XX] -=  m_pMyRoom->m_Size[XX];
			m_Orig[XX] -=  m_pMyRoom->m_Size[XX];
		}
		if (std::max(m_Maxs[XX],m_Mins[XX]) < L)
		{
			m_Mins[XX] += m_pMyRoom->m_Size[XX];
			m_Maxs[XX] += m_pMyRoom->m_Size[XX];
			m_Orig[XX] += m_pMyRoom->m_Size[XX];
		}
	}
	if (!m_pMyRoom->m_Block[EBS_DOWN] && !m_pMyRoom->m_Block[EBS_UP])
	{
		VMarg = m_pMyRoom->m_Size[YY]-480;
		VMarg /= 2;
		D = 480+VMarg;
		U = -VMarg;
		if (std::min(m_Mins[YY],m_Maxs[YY]) > D)
		{
			//qDebug()<<"far down"<<std::min(m_Mins[Y],m_Maxs[Y])<<D<<m_pMyRoom->m_Size[Y];
			m_Mins[YY] -= m_pMyRoom->m_Size[YY];
			m_Maxs[YY] -= m_pMyRoom->m_Size[YY];
			m_Orig[YY] -= m_pMyRoom->m_Size[YY];
		}
		if (std::max(m_Mins[YY],m_Maxs[YY]) < U)
		{
			//qDebug()<<"far up"<<std::max(m_Mins[Y],m_Maxs[Y])<<U<<m_pMyRoom->m_Size[Y];
			m_Mins[YY] += m_pMyRoom->m_Size[YY];
			m_Maxs[YY] += m_pMyRoom->m_Size[YY];
			m_Orig[YY] += m_pMyRoom->m_Size[YY];
		}
	}
	
	if (m_Orig[0] != 0 || m_Orig[1] != 0)
	{
		//qDebug()<<"scrolled"<<m_Orig;
		for(int i=m_Parts.size()-1;i>=0;i--)
		{
			m_Parts[i]->s += m_Orig;
			m_Parts[i]->e += m_Orig;
		}
		
		m_Orig[0] = 0;
		m_Orig[1] = 0;
	}
}

void cBaseTrigger::Step()
{
	//cBaseObject::Step();
	
	
}

void cBaseTrigger::RenderMe()
{
//	glColor4f(1.f,0.65f,0.f,0.5f);
//	glDisable(GL_TEXTURE_2D);
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	glBegin(GL_QUADS);
//	for(int i=m_Parts.size()-1;i>=0;i--)
//	{
//		glVertex2f(m_Parts[i]->s.x(),m_Parts[i]->s.y());
//		glVertex2f(m_Parts[i]->s.x(),m_Parts[i]->e.y());
//		glVertex2f(m_Parts[i]->e.x(),m_Parts[i]->e.y());
//		glVertex2f(m_Parts[i]->e.x(),m_Parts[i]->s.y());
//	}
//	glEnd();
//	glEnable(GL_TEXTURE_2D);
}

bool cBaseTrigger::Crosses(const QVector2D &os, const QVector2D &oe)
{
	if (!m_Parts.size()) return false;
		
	for (int i=m_Parts.size()-1;i>=0;i--)
		if (m_Parts[i]->Crosses(os,oe)) return true;
	
	return false;
}

void cBaseTrigger::Touching(cBaseObject *pOther)
{
	
}

void cBaseTrigger::AddPart(const QVector2D &s, const QVector2D &e)
{
	for(int i=0;i<2;i++)
	{
		m_Maxs[i] = std::max(m_Maxs[i],std::max(s[i],e[i]));
		m_Mins[i] = std::min(m_Mins[i],std::min(s[i],e[i]));
	}
	m_Parts.push_back(new trigger_part_s(s,e));
}

trigger_part_s::trigger_part_s(const QVector2D &v1, const QVector2D &v2)
{
	for(int i=0;i<2;i++)
	{
		s[i] = std::min(v1[i],v2[i]);//+1.f;
		e[i] = std::max(v1[i],v2[i]);//-1.f;
	}
}

bool trigger_part_s::HasPoint(const QVector2D &p)
{
	if (p.x() < s.x()) return false;
	if (p.x() > e.x()) return false;
	
	if (p.y() < s.y()) return false;
	if (p.y() > e.y()) return false;
	
	return true;
}

bool trigger_part_s::Crosses(const QVector2D &os, const QVector2D &oe)
{
	trigger_part_s o(os,oe);
	
	if (HasPoint(o.s)) return true;
	if (HasPoint(o.e)) return true;
	if (HasPoint(QVector2D(o.s.x(),o.e.y()))) return true;
	if (HasPoint(QVector2D(o.e.x(),o.s.y()))) return true;
	
	if (o.HasPoint(s)) return true;
	if (o.HasPoint(e)) return true;
	if (o.HasPoint(QVector2D(s.x(),e.y()))) return true;
	if (o.HasPoint(QVector2D(e.x(),s.y()))) return true;
	
	if ((s.y() < o.s.y() && e.y() > o.e.y()) && (s.x() >= o.s.x() && e.x() <= o.e.x()))
			return true;
	
	if ((o.s.y() < s.y() && o.e.y() > e.y()) && (o.s.x() >= s.x() && o.e.x() <= e.x()))
			return true;
	
	return false;
}
