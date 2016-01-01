#include "cLine.h"
#include "cSegment.h"
#include "cGameRoom.h"

#define S_L 0
#define S_R 1
#define S_U 2
#define S_D 3

cLine::cLine()
{
	m_JumpDown  =  false;
}
void cLine::CalcMe()
{
	if (m_E.x() == m_S.x())
		if (m_E.y() == m_S.y())
		{
			m_N = m_T = QVector2D(0,0);
			return;
		}
	m_T = m_E - m_S;
	m_T.normalize();
	
	m_N.setX( m_T.y());
	m_N.setY(-m_T.x());
}

bool cLine::ExpCaseHasPoint(const QVector2D &p)
{
	for(int i=m_Exps.size()-1;i>=0;i--)
	{
		if (dot(
				 p - QVector2D(m_Exps[i]->m_S.x(),m_Exps[i]->m_S.y()),
				 m_Exps[i]->m_N) >= 0.f) return false;
	}
	return true;
}

cLine::~cLine()
{
	//ClearExps();
	
	cLine *oseg;
	
	for(int i=m_Neighs.size()-1;i>=0;i--)
	{
		oseg = m_Neighs[i];
		int idx = oseg->m_Neighs.indexOf(this);
		
		if (idx != -1)
			oseg->m_Neighs.removeAt(idx);
	}
}
void cLine::ClearExps(void)
{
//	for(int i=m_Exps.size()-1;i>=0;i--)
//		m_Exps[i]->m_Flags |= FL_KILLME;
//	m_Exps.clear();
}

void cLine::ExpandMe(const QVector2D &half_size, cGameRoom *room)
{
	m_Exps.clear();
	m_Axials.clear();
	
	QVector2D snapN = SnapNormal(half_size);
	
	cParExpand *pParEx = (cParExpand *)(room->CreateExpand(false));//GET_OBJECT(par_expand);
	pParEx->m_S = m_S+snapN;
	pParEx->m_E = m_E+snapN;
	pParEx->CalcMe();	
	pParEx->m_pLine = this;
	m_Exps.push_back(pParEx);
	if (snapN.x() == 0.f)
	{
		pParEx->m_S -= m_T*half_size.x();
		pParEx->m_E += m_T*half_size.x();
	}
	if (snapN.y() == 0.f)
	{
		pParEx->m_S -= m_T*half_size.y();
		pParEx->m_E += m_T*half_size.y();
	}
	
	pParEx = (cParExpand *)room->CreateExpand(false);//GET_OBJECT(par_expand);
	pParEx->m_S = m_E-snapN;
	pParEx->m_E = m_S-snapN;
	pParEx->CalcMe();
	pParEx->m_pLine = this;
	m_Exps.push_back(pParEx);
	if (snapN.x() == 0.f)
	{
		pParEx->m_S -= m_T*half_size.x();
		pParEx->m_E += m_T*half_size.x();
	}
	if (snapN.y() == 0.f)
	{
		pParEx->m_S -= m_T*half_size.y();
		pParEx->m_E += m_T*half_size.y();
	}
	
	cAxialExpand *pAxialEx;
	
	bool sides[4] = {false,false,false,false};
	
	QVector2D exts[4][2] = 
	{
		{QVector2D(-half_size.x(), half_size.y()),QVector2D(-half_size.x(),-half_size.y())},
		{QVector2D( half_size.x(),-half_size.y()),QVector2D( half_size.x(), half_size.y())},
		{QVector2D(-half_size.x(),-half_size.y()),QVector2D( half_size.x(),-half_size.y())},
		{QVector2D( half_size.x(), half_size.y()),QVector2D(-half_size.x(), half_size.y())}
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
	
	for(int i=0;i<4;i++)
	{
		if (sides[i])
		{
			pAxialEx = (cAxialExpand *)room->CreateExpand(true);//GET_OBJECT(axial_expand);
			pAxialEx->m_S = m_S + exts[i][0];
			pAxialEx->m_E = m_S + exts[i][1];
			pAxialEx->CalcMe();
			pAxialEx->m_pLine = this;
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
			pAxialEx = (cAxialExpand *)room->CreateExpand(true);//GET_OBJECT(axial_expand);
			pAxialEx->m_S = m_E + exts[i][0];
			pAxialEx->m_E = m_E + exts[i][1];
			pAxialEx->CalcMe();
			pAxialEx->m_pLine = this;
			if (dot(pAxialEx->m_T,m_T) < 0.f) pAxialEx->m_ToRemove = true;
			m_Exps.push_back(pAxialEx);
			m_Axials.push_back(pAxialEx);
		}
	}
}

QVector2D cLine::SnapNormal(const QVector2D &half_size)
{
	QVector2D out(0,0);
	if (m_N.x() < 0) out.setX(-half_size.x());
	else if (m_N.x() > 0) out.setX(half_size.x());
	
	if (m_N.y() < 0) out.setY(-half_size.y());
	else if (m_N.y() > 0) out.setY(half_size.y());
	
	return out;
}
