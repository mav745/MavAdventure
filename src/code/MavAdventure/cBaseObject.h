#ifndef CBASEOBJECT_H
#define CBASEOBJECT_H

#include <QObject>
#include <QVector2D>
//#include <QVector3D>
#include <QPoint>
#include <QPointF>

#include "cBaseRenderable.h"

//#include <hpint.h>

class cSprite;


class cBaseObject : public cBaseRenderable
{
	Q_OBJECT
	
protected:
	int m_Id; //read-only
	//QPointF m_Tail;
	
public:
	explicit cBaseObject(QObject *parent = 0);
	
	QString m_Target;
	QString m_TargetName;
	
	bool m_Persist;
	//bool m_Visible;
	//int m_Depth;
	
	cSprite *m_pTexture;
	
	QVector2D m_ScrollFac;
	QVector2D m_Orig;
	QVector2D m_Velocity;
	QVector2D m_ImageScale;
	float m_ImageAlpha;
	float m_ImageAngle;
	float m_ImageIndex;
	float m_ImageSpeed;
	
	inline int Id(void) {return m_Id;}
	inline virtual bool IsPlayer() {return false;}
	
	//virtual void Create(void);
	void SetSpeedDirection(float s, float d);
	void Step(void);
	virtual void Use(cBaseObject *pOther);
	virtual void Draw(void);
	
	void CalcCoords(const QVector2D &add);
	//void ScrollMe(QPoint scr);
	
	bool operator < (const cBaseObject &o) const 
	{
		if (m_Depth != o.m_Depth) return m_Depth < o.m_Depth;
		return m_Id < o.m_Id;
	} 
	
	virtual void ParseKeyValue(const QString &key, const QString &value);
	
signals:
	
public slots:
	void SetX(float x) {m_Orig.setX(x);}
	void SetY(float y) {m_Orig.setX(y);}
};

#endif // CBASEOBJECT_H
