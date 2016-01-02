#ifndef CSEGMENT_H
#define CSEGMENT_H

#include "cLine.h"
#include <QVector>
#include <math.h>

class cBaseExpanded;
class cAxialExpand;

class cSegment : public cLine
{
	Q_OBJECT
public:
	explicit cSegment(QObject *parent = 0);
	
	~cSegment();
	
	QVector<cBaseExpanded *>m_Exps;
	QVector<cAxialExpand *>m_Axials;
	QVector<cSegment *>m_Neighs;
	
	bool IsNeigh(cSegment *pO)
	{
		if (m_S == pO->m_E) return true;
		if (m_E == pO->m_S) return true;
		return false;
	}
	inline bool ExpCaseHasPoint(const QPoint &p) 
	{
		return ExpCaseHasPoint(QVector2D(p.x(),p.y()));
	}
	bool ExpCaseHasPoint(const QVector2D &p);
	
	
	bool RenderMe();
	void ClearExps();
	void ExpandMe(const QVector2D &half_size);
	QVector2D SnapNormal(const QVector2D &half_size);
signals:
	
public slots:
	
};

#endif // CSEGMENT_H
