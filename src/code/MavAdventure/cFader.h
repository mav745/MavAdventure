#ifndef CFADER_H
#define CFADER_H

#include "cBaseObject.h"
#include <QVector3D>
//#include <QTimer>

enum FADE_LAYER
{
	FDL_UPPER = 0,
	FDL_MIDDLE
};

#define FFADE_IN      (1<<0)
#define FFADE_OUT     (1<<1)
#define FFADE_STAYOUT (1<<2)

class cFader : public cBaseObject//cBaseRenderable
{
	Q_OBJECT
	
private:
	//float m_FadeSpeed;
	float m_FadeStart;
	
public:
	explicit cFader(QObject *parent = 0);
	~cFader();
	
	QVector3D m_Color;
	
	int m_FadeFlags;
	
	float m_FadeSec;
	FADE_LAYER m_Layer;
	//int m_MsecHold;
	
	int idx;
	
	bool m_Fading;
	
	void RenderMe();
	
signals:
	void fadeDone(void);
public slots:
	void Fade(void);
};

#endif // CFADER_H
