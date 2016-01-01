#include "cChangelevel.h"

#include <cMainManager.h>
#include <cGameRoom.h>
#include <cConsole.h>

#include <util.h>
#include <QDir>

cChangelevel::cChangelevel(QObject *parent) :
	cBaseTrigger(parent)
{
	m_NextRoom.clear();
	m_LandMark.clear();
}

void cChangelevel::ChangeLevel(void)
{
	
	//TODO:
	//build object list to transfer through level change
	//transfer them all relative to landmark object origin
	
	MGR->Map(m_NextRoom);
	
//	cGameRoom *pRoom = MGR->RoomByName(m_NextRoom);
//	if (pRoom)
//	{
//		MGR->m_pCurrentRoom = pRoom;
//		//HACKHACK: move this somewhere better whenever room loading code will work
//		emit pRoom->roomStart();
//	}
}
