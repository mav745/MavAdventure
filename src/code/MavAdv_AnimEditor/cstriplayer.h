#ifndef CSTRIPLAYER_H
#define CSTRIPLAYER_H

#include "cbasenode.h"


class cStripLayer : public cBaseNode
{
	Q_OBJECT
	
public:
	explicit cStripLayer(QObject *parent = 0);
	~cStripLayer();
	
	int m_FrameH;
	int m_FrameW;
	int m_NumFrames;
	imgList_t m_StripFrames;
	
	static cStripLayer* GetNewStrip(QString filename_numframes);
};

#endif // CSTRIPLAYER_H
