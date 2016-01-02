#ifndef CBASETRIGGER_H
#define CBASETRIGGER_H

#include <cBase.h>
#include <QVector>

//struct trig_part_base_s 
//{
	
//	virtual void DrawMe() = 0;
//	virtual bool HasPoint(const QVector2D &p) = 0;
//};
struct trig_part_rect_s //: public trig_part_base_s
{
	trig_part_rect_s(const QVector2D &v1, const QVector2D &v2 );
	
	QVector2D mins;
	QVector2D maxs;
	QVector2D s;
	QVector2D e;
	
	void RecalcBounds();
	
	void DrawMe();
	bool HasPoint(const QVector2D &p);
	
	bool Crosses(const QVector2D &os, const QVector2D &oe);
};
//struct trig_part_circ_s : public trig_part_base_s
//{
//	trig_part_circ_s(const QVector2D &center, float r);
//	QVector2D c;
//	float radius;
//	void DrawMe();
//	bool HasPoint(const QVector2D &p);
//};

class cBaseTrigger : public cBase
{
	Q_OBJECT
public:

	explicit cBaseTrigger(QObject *parent = 0);
	~cBaseTrigger();
	
	QVector<trig_part_rect_s *>m_Parts;
	
	void Merge(cBaseTrigger *o);
	
	bool RenderMe();
	
	bool ShapeHasPoint(const QVector2D &p);
	
	inline void AddRectShape(const QVector2D &v1, const QVector2D &v2)
	{
		m_Parts.push_back(new trig_part_rect_s(v1,v2));
		RecountMinMax();
	}
//	inline void AddCircShape(const QVector2D &c, float r)
//	{
//		m_Parts.push_back(new trig_part_circ_s(c,r));
//		RecountMinMax();
//	}
	
	void RecountMinMax();
	
	bool Crosses (const QVector2D &os, const QVector2D &oe);
	void Shift(const QVector2D &ofs);
	void SetEntityType(QString type);
signals:
	
public slots:
	
};

#endif // CBASETRIGGER_H
