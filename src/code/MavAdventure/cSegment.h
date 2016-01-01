#ifndef CSEGMENT_H
#define CSEGMENT_H
#include <QDebug>
#include <QVector2D>
#include "cBaseObject.h"
#include <math.h>
#include "util.h"

#define TRACE_EPSILON (1.0f/16.f)
#define FRAC_EPSILON (1.0f/1024.f)

class cGameRoom;
class cSegment;
class cLine;

struct traceresult_s
{
	QVector2D tracedir;
	QVector2D endpos;
	float fraction;
	QVector2D normal;
	cSegment *wall;
	
	traceresult_s()
	{
		Reset();
	}
	void Reset()
	{
		fraction =            1.f;
		endpos   = QVector2D(0,0);
		normal   = QVector2D(0,0);
	}
	inline void UpdateResult(const QVector2D &end, float frac, const QVector2D &norm, cSegment *in_wall)
	{
		if (frac < 0.0001f) frac = 0.f;
		
		if (frac > fraction) return;
		
		fraction  =     frac;
		endpos    =      end;
		//endpos   +=     norm*TRACE_EPSILON;
		normal    =     norm;
		wall      =  in_wall;
	}
};

class cSegment : public cBaseObject
{
public:
	cSegment();
	cSegment(const QVector2D &s, const QVector2D &e);
	~cSegment();
	
	int m_Idx;
	cGameRoom *m_pMyRoom;
	
	cLine *m_pLine;
	
	QVector2D m_S;
	QVector2D m_E;
	QVector2D m_N;
	QVector2D m_T;
	float m_Dist;
	bool m_ToRemove;
	
	void Step();
	void RenderMe();
	
	void CalcMe()
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
		
		//qDebug()<<"m_S"<<m_S<<"m_N"<<m_N;
		m_Dist = QVector2D::dotProduct(m_S,m_N);
	}
	
	void TraceAgainstMe(cSegment &ray, traceresult_s *ptr);
	
	bool ContaintsPoint(const QVector2D &pP, float w = 0.5f, float l = 0.f)
	{
		QVector2D s(m_S.x(),m_S.y()),e(m_E.x(),m_E.y());
		
		if (QVector2D::dotProduct(pP-(s-m_T*l), m_T)  <  0.f ) return false; //after start
		if (QVector2D::dotProduct(pP-(e+m_T*l),-m_T)  <  0.f ) return false; //before end
        if (fabs(QVector2D::dotProduct(pP-s, m_N)) >= w) return false; //side to side
		return true;
	}
	bool ContaintsLine(cSegment *pO)
	{
		if (		QVector2D::dotProduct(pO->m_S-m_S, m_T)  <  0.f ) return false; //after start
		if (		QVector2D::dotProduct(pO->m_E-m_E,-m_T)  <  0.f ) return false; //before end
        if (fabs(QVector2D::dotProduct(pO->m_S-m_S, m_N)) >= 0.5f) return false; //side to side
        if (fabs(QVector2D::dotProduct(pO->m_E-m_S, m_N)) >= 0.5f) return false; //side to side
		return true;
	}
	//void TraceAgainstMe_GM(const cSegment &ray, traceresult_s *ptr);
};

class cAxialExpand  : public cSegment{};
class cParExpand    : public cSegment{};

#endif // CSEGMENT_H
