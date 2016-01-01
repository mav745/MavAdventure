#include "cBaseTexture.h"

#ifndef CSPRITE_H
#define CSPRITE_H

#include <QPointF>
#include <QString>
#include <QVector2D>

enum FRAME_DIR
{
	EFD_HOR,
	EFD_VER
};

class cSprite : public cBaseTexture
{
public:
	cSprite();
	
	FRAME_DIR m_FrameDir;
	
	QPointF m_Orig;
	int m_numFrames;
	int m_W;
	int m_H;
	float m_Frame;
	float m_FrW;
	float m_RenderColor;
	
	
	float SetFrame(float frame, bool loop = true);
	void Render(void);
	static cSprite *getSprite(
			QString fname = QString("res/sprites/none.png"), 
			int numFr = 1, 
			FRAME_DIR dir = EFD_HOR,
			int w = -1, 
			int h = -1);
	void DrawFrame(const QVector2D &orig, int frame);
};

#endif // CSPRITE_H
