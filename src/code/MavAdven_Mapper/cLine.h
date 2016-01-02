#ifndef CLINE_H
#define CLINE_H

#include "cBase.h"
#include <QVector2D>

float dot(const QVector2D &v1, const QVector2D &v2);
float dot(const QPoint &v1, const QVector2D &v2);
float dot(const QVector2D &v1, const QPoint &v2);
float dot(const QPoint &v1, const QPoint &v2);


class cLine : public cBase
{
	Q_OBJECT
public:
	explicit cLine(QObject *parent = 0);
	
	QPoint    m_S;
	QPoint    m_E;
	QVector2D m_N;
	QVector2D m_T;
	
	bool m_JumpDown;
	
	void CalcMe(void);
	bool RenderMe(void);
	bool ContaintsPoint(const QVector2D &pP, float w = 0.5f, float l = 0.f)
	{
		QVector2D s(m_S.x(),m_S.y()),e(m_E.x(),m_E.y());
		//s2 = s - m_T*l;
		//e2 = e + m_T*l;
		
		if (dot(pP-(s-m_T*l), m_T)  <  0.f ) return false; //after start
		if (dot(pP-(e+m_T*l),-m_T)  <  0.f ) return false; //before end
		if (abs(dot(pP-s, m_N)) >= w) return false; //side to side
		return true;
	}
	bool ContaintsLine(cLine *pO)
	{
		
		if (	  dot(pO->m_S-m_S, m_T)  <  0.f ) return false; //after start
		if (	  dot(pO->m_E-m_E,-m_T)  <  0.f ) return false; //before end
		if (abs(dot(pO->m_S-m_S, m_N)) >= 0.5f) return false; //side to side
		if (abs(dot(pO->m_E-m_S, m_N)) >= 0.5f) return false; //side to side
		return true;
	}
	
signals:
	
public slots:
	
};

#endif // CLINE_H
