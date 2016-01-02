#ifndef CBASEENTITY_H
#define CBASEENTITY_H

#include "cBase.h"
#include <QVector3D>

class cBaseEntity : public cBase
{
	Q_OBJECT
public:
	explicit cBaseEntity(QObject *parent = 0);
	
	int m_Texture;
	float m_FrameW;
	float m_FrameH;
	float m_SprW;
	float m_SprH;
	float m_NumFrames;
	float m_AnimSpeed;
	QVector3D m_Color;
	QVector2D m_Orig;
	QVector2D m_ImageScale;
	QVector2D m_ScrollFac;
	
	bool RenderMe(void);
	void SetEntityType(QString type);
	bool HasPoint(const QVector2D &v);
};

#endif // CBASEENTITY_H
