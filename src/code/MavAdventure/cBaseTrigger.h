#ifndef CBASETRIGGER_H
#define CBASETRIGGER_H

#include <QVector>
#include <QVector2D>
#include <cBaseObject.h>

struct trigger_part_s
{
public:
	
	QVector2D s;
	QVector2D e;
	
	trigger_part_s(const QVector2D &v1, const QVector2D &v2);
	bool HasPoint(const QVector2D &p);
	bool Crosses(const QVector2D &os, const QVector2D &oe);
};

class cBaseTrigger : public cBaseObject
{
	Q_OBJECT
public:
	explicit cBaseTrigger(QObject *parent = 0);
	
	~cBaseTrigger();
	
	void Spawn();
	
	inline QVector2D GetCenter()
	{
		return (m_Mins+m_Maxs)*.5f;
	}
	
	QVector2D m_Mins;
	QVector2D m_Maxs;
	
	QVector<trigger_part_s*> m_Parts;
	
	QString m_ClassFilter;
	QString m_NameFilter;
	
	void ScrollMe(const QVector2D &scrll);
	void Step();
	void RenderMe();
	bool Crosses(const QVector2D &os, const QVector2D &oe);
	virtual void Touching(cBaseObject *pOther);
	void AddPart(const QVector2D &s, const QVector2D &e);
signals:
	
public slots:
	
};

#endif // CBASETRIGGER_H
