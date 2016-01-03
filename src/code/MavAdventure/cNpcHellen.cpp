#include <cGameRoom.h>
#include "cNpcHellen.h"

#include <util.h>

cNpcHellen::cNpcHellen(QObject *parent) : cBaseTalker(parent)
{
	AddAnim("res/sprites/charas/hellen/idle.png","idle");
	SetAnim("idle");
	
}

cNpcHellen::~cNpcHellen()
{
	
}

void cNpcHellen::Step()
{
	cBaseTalker::Step();
	
	QList<cBaseObject*> objs = m_pMyRoom->ObjectsByClassname("maverick");
	if (objs.size())
	{
		m_ImageScale[0] = UTIL_Sign(objs[0]->m_Orig.x() - m_Orig.x());
		if (m_ImageScale[0] == 0.f)
			m_ImageScale[0] = 1.f;
	}
}

