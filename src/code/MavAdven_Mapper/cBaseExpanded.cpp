#include "cBaseExpanded.h"
#include "cGLManager.h"


cBaseExpanded::cBaseExpanded(QObject *parent) :
	cLine(parent)
{
	m_Classname = "base_expand";
	m_ToRemove = false;
}

bool cBaseExpanded::RenderMe(void)
{
	if (!cBase::RenderMe()) return false;
	
	if (!m_ToRemove)
		glColor4f(0.2f,0.5f,1.f,0.65f);
	else
		glColor4f(1.0f,0.2f,0.2f,0.3f);
	
	QVector2D s(m_S.x(),m_S.y()),e(m_E.x(),m_E.y());
	QVector2D c = s+e;
	c *= 0.5f;

	MGR->DrawLine(s,e);
	MGR->DrawLine(c,c+m_N*8);
	
	glColor3f(1,1,1);
	
	return true;
}
