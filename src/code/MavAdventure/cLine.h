#ifndef CLINE_H
#define CLINE_H

#include <Qvector2D>
#include <Qvector>
#include <math.h>

#define dot(x,y) QVector2D::dotProduct(x,y)

//#include "cSegment.h"

class cSegment;
class cAxialExpand;
class cParExpand;
class cGameRoom;

class cLine
{
public:
	cLine();
	
	QVector2D m_S;
	QVector2D m_E;
	QVector2D m_N;
	QVector2D m_T;
	
	bool m_JumpDown;
	
	void CalcMe(void);
	bool ContaintsPoint(const QVector2D &pP, float w = 0.5f, float l = 0.f)
	{
		QVector2D s(m_S.x(),m_S.y()),e(m_E.x(),m_E.y());
		
		if (dot(pP-(s-m_T*l), m_T)  <  0.f ) return false; //after start
		if (dot(pP-(e+m_T*l),-m_T)  <  0.f ) return false; //before end
		if (abs(dot(pP-s, m_N)) >= w) return false; //side to side
		return true;
	}
	bool ContaintsLine(cLine *pO)
	{
		if (		dot(pO->m_S-m_S, m_T)  <  0.f ) return false; //after start
		if (		dot(pO->m_E-m_E,-m_T)  <  0.f ) return false; //before end
		if (fabs(dot(pO->m_S-m_S, m_N)) >= 0.5f) return false; //side to side
		if (fabs(dot(pO->m_E-m_S, m_N)) >= 0.5f) return false; //side to side
		return true;
	}
	QVector<cSegment *>m_Exps;
	QVector<cAxialExpand *>m_Axials;
	QVector<cLine *>m_Neighs;
	
	bool IsNeigh(cLine *pO)
	{
		if (m_S == pO->m_E) return true;
		if (m_E == pO->m_S) return true;
		return false;
	}
//	inline bool ExpCaseHasPoint(const QPoint &p) 
//	{
//		return ExpCaseHasPoint(QVector2D(p.x(),p.y()));
//	}
	bool ExpCaseHasPoint(const QVector2D &p);
	
	void ClearExps();
	void ExpandMe(const QVector2D &half_size, cGameRoom *room);
	QVector2D SnapNormal(const QVector2D &half_size);
	~cLine();
};

#endif // CLINE_H
