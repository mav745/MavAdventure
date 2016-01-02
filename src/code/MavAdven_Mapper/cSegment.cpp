#include "cSegment.h"
#include "cGLManager.h"
#include "cBaseExpanded.h"
#include "cAxialExpand.h"
#include "cParExpand.h"

#include <QDebug>

#define S_L 0
#define S_R 1
#define S_U 2
#define S_D 3

cSegment::cSegment(QObject *parent) :
	cLine(parent)
{
	m_Depth = INT_MIN;
}
cSegment::~cSegment()
{
	ClearExps();
	
	cSegment *oseg;
	
	for(int i=m_Neighs.size()-1;i>=0;i--)
	{
		oseg = m_Neighs[i];
		int idx = oseg->m_Neighs.indexOf(this);
		
		if (idx != -1)
			oseg->m_Neighs.removeAt(idx);
	}
}
void cSegment::ClearExps(void)
{
	for(int i=m_Exps.size()-1;i>=0;i--)
		m_Exps[i]->m_Flags |= FL_KILLME;
	m_Exps.clear();
}
bool cSegment::ExpCaseHasPoint(const QVector2D &p)
{
	for(int i=m_Exps.size()-1;i>=0;i--)
	{
		if (dot(
				 p - QVector2D(m_Exps[i]->m_S.x(),m_Exps[i]->m_S.y()),
				 m_Exps[i]->m_N) >= 0.f) return false;
	}
	return true;
}
bool cSegment::RenderMe()
{
	if (!cLine::RenderMe()) return false;
	
//	MGR->drawText(m_S.x(),m_S.y(),QString().number(m_Neighs.size()));
	
//	glColor3f(0.f,1.f,0.f);
//	QVector2D c,oc;
//	c = QVector2D(m_S.x(),m_S.y())+QVector2D(m_E.x(),m_E.y());
//	c *= 0.5f;
//	for(int i=m_Neighs.size()-1;i>=0;i--)
//	{
//		oc = QVector2D(m_Neighs[i]->m_S.x(),m_Neighs[i]->m_S.y())+QVector2D(m_Neighs[i]->m_E.x(),m_Neighs[i]->m_E.y());
//		oc *= 0.5f;
//		MGR->DrawLine(c.x(),c.y(),oc.x(),oc.y());

//	}
	
	return true;
}
QVector2D cSegment::SnapNormal(const QVector2D &half_size)
{
	QVector2D out(0,0);
	if (m_N.x() < 0) out.setX(-half_size.x());
	else if (m_N.x() > 0) out.setX(half_size.x());
	
	if (m_N.y() < 0) out.setY(-half_size.y());
	else if (m_N.y() > 0) out.setY(half_size.y());
	
	return out;
}
void cSegment::ExpandMe(const QVector2D &half_size)
{
	m_Exps.clear();
	m_Axials.clear();
	
	QVector2D snapN = SnapNormal(half_size);
	
	cParExpand *pParEx = (cParExpand *)GET_OBJECT(par_expand);
	pParEx->m_S = m_S+QPoint(snapN.x(),snapN.y());
	pParEx->m_E = m_E+QPoint(snapN.x(),snapN.y());
	pParEx->CalcMe();	
	m_Exps.push_back(pParEx);
	if (snapN.x() == 0.f)
	{
		pParEx->m_S -= QPoint(m_T.x(),m_T.y())*half_size.x();
		pParEx->m_E += QPoint(m_T.x(),m_T.y())*half_size.x();
	}
	if (snapN.y() == 0.f)
	{
		pParEx->m_S -= QPoint(m_T.x(),m_T.y())*half_size.y();
		pParEx->m_E += QPoint(m_T.x(),m_T.y())*half_size.y();
	}
	
	pParEx = (cParExpand *)GET_OBJECT(par_expand);
	pParEx->m_S = m_E-QPoint(snapN.x(),snapN.y());
	pParEx->m_E = m_S-QPoint(snapN.x(),snapN.y());
	pParEx->CalcMe();
	m_Exps.push_back(pParEx);
	if (snapN.x() == 0.f)
	{
		pParEx->m_S -= QPoint(m_T.x(),m_T.y())*half_size.x();
		pParEx->m_E += QPoint(m_T.x(),m_T.y())*half_size.x();
	}
	if (snapN.y() == 0.f)
	{
		pParEx->m_S -= QPoint(m_T.x(),m_T.y())*half_size.y();
		pParEx->m_E += QPoint(m_T.x(),m_T.y())*half_size.y();
	}
	
	//pParEx->m_ToRemove = true;
	
	cAxialExpand *pAxialEx;
	
	bool sides[4] = {false,false,false,false};

	QPoint exts[4][2] = 
	{
		{QPoint(-half_size.x(), half_size.y()),QPoint(-half_size.x(),-half_size.y())},
		{QPoint( half_size.x(),-half_size.y()),QPoint( half_size.x(), half_size.y())},
		{QPoint(-half_size.x(),-half_size.y()),QPoint( half_size.x(),-half_size.y())},
		{QPoint( half_size.x(), half_size.y()),QPoint(-half_size.x(), half_size.y())}
	};
	
	if (snapN.y() != 0.f)
	{
		if (m_S.x() < m_E.x()) sides[S_L] = true;
		else if (m_S.x() > m_E.x()) sides[S_R] = true;
		else sides[S_L] = sides[S_R] = true;
	}
	if (snapN.x() != 0.f)
	{
		if (m_S.y() < m_E.y()) sides[S_U] = true;
		else if (m_S.y() > m_E.y()) sides[S_D] = true;
		else sides[S_U] = sides[S_D] = true;
	}
	
	//qDebug()<<sides[0]<<sides[1]<<sides[2]<<sides[3];
	
	for(int i=0;i<4;i++)
	{
		if (sides[i])
		{
			pAxialEx = (cAxialExpand *)GET_OBJECT(axial_expand);
			pAxialEx->m_S = m_S + exts[i][0];
			pAxialEx->m_E = m_S + exts[i][1];
			pAxialEx->CalcMe();
			if (dot(pAxialEx->m_T,m_T) < 0.f) pAxialEx->m_ToRemove = true;
			m_Exps.push_back(pAxialEx);
			m_Axials.push_back(pAxialEx);
		}
	}
	sides[0] = sides[1] = sides[2] = sides[3] = false;
	
	if (snapN.y() != 0.f)
	{
		if (m_S.x() > m_E.x()) sides[S_L] = true;
		else if (m_S.x() < m_E.x()) sides[S_R] = true;
		else sides[S_L] = sides[S_R] = true;
	}
	if (snapN.x() != 0.f)
	{
		if (m_S.y() > m_E.y()) sides[S_U] = true;
		else if (m_S.y() < m_E.y()) sides[S_D] = true;
		else sides[S_U] = sides[S_D] = true;
	}
	
	for(int i=0;i<4;i++)
	{
		if (sides[i])
		{
			pAxialEx = (cAxialExpand *)GET_OBJECT(axial_expand);
			pAxialEx->m_S = m_E + exts[i][0];
			pAxialEx->m_E = m_E + exts[i][1];
			pAxialEx->CalcMe();
			if (dot(pAxialEx->m_T,m_T) < 0.f) pAxialEx->m_ToRemove = true;
			m_Exps.push_back(pAxialEx);
			m_Axials.push_back(pAxialEx);
		}
	}
}
