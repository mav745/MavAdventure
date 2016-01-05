#ifndef CSTRIPLAYER_H
#define CSTRIPLAYER_H

#include "cbasenode.h"


class cStripLayer : public cBaseNode
{
	Q_OBJECT
	
public:
	explicit cStripLayer(QObject *parent = 0);
	~cStripLayer();
	
	void LoadStrip(QString filename_numframes);
	
	imgList_t m_StripFrames;
};

#endif // CSTRIPLAYER_H
