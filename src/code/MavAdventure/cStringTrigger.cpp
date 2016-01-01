#include "cStringTrigger.h"
#include <cMainManager.h>
#include <cGameRoom.h>

#include <util.h>

cStringTrigger::cStringTrigger(QObject *parent) :
	cBaseTrigger(parent)
{
	m_Filter = "";
	
}

void cStringTrigger::CheckString(QString str)
{
	if (str == m_Filter)
	{
		emit Activated();
		
		
		//TODO: fire targets
//		QList<cBaseObject *> tgtList = MGR->m_pCurrentRoom->ObjectsByTargetname(m_Target);
//		int sz = tgtList.size();
//		if (!sz) return;
		
//		for(int i=0;i<sz;i++)
//		{
//			//tgtList[i].
//		}
	}
}
