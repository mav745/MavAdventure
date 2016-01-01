#include "cBaseRenderable.h"
#include <util.h>

cBaseRenderable::cBaseRenderable(QObject *parent) :
	QObject(parent)
{
	m_Flags = 0;
	m_Depth = 0;
	m_Visible = true;
	m_pMyRoom = (class cGameRoom*)0;
}

void cBaseRenderable::Spawn()
{
	//m_Flags &= ~FL_TRANSFER;
}

