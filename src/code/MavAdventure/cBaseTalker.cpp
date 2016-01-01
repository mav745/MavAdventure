#include "cBaseTalker.h"
#include "cBigDialog.h"
#include <math.h>

cBaseTalker::cBaseTalker(QObject *parent) : cBaseAnimating(parent)
{
	m_Emotions = new int[ETE_MAX];
	for(int i=0;i<ETE_MAX-1;i++)
		m_Emotions[i] = 0;
}

void cBaseTalker::Talk(TALKER_EMOTION emote, const QString &text)
{
	
}

float cBaseTalker::getSpeed()
{
	return fabs(m_Velocity.x());
}

cBaseTalker::~cBaseTalker()
{
	delete [] m_Emotions;
}

