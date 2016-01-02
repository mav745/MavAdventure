#include "cGLManager.h"
#include "cLine.h"
#include "cLineTool.h"

#include <algorithm>
using namespace std;

extern cLineTool *g_pLineTool;

float dot(const QVector2D &v1, const QVector2D &v2)
{
	return QVector2D::dotProduct(v1,v2);
}
float dot(const QPoint &v1, const QVector2D &v2)
{
	return QVector2D::dotProduct(QVector2D(v1.x(),v1.y()),v2);
}
float dot(const QVector2D &v1, const QPoint &v2)
{
	return QVector2D::dotProduct(v1, QVector2D(v2.x(),v2.y()));
}
float dot(const QPoint &v1, const QPoint &v2)
{
	return QVector2D::dotProduct(QVector2D(v1.x(),v1.y()), QVector2D(v2.x(),v2.y()));
}
cLine::cLine(QObject *parent) :
	cBase(parent)
{
	m_Classname = "line";
	m_JumpDown  =  false;
}
void cLine::CalcMe()
{
	if (m_E.x() == m_S.x())
		if (m_E.y() == m_S.y())
		{
			m_N = m_T = QVector2D(0,0);
			return;
		}
	
	float d, dx,dy;
	dx = m_E.x() - m_S.x();
	dy = m_E.y() - m_S.y();
	d  = 1.f/sqrt(dx*dx +dy*dy);
	
	m_T[0] = dx*d;
	m_T[1] = dy*d;
	m_N[0] =  m_T[1];
	m_N[1] = -m_T[0];
	
	m_Mins = QVector2D(min(m_S.x(),m_E.x()),min(m_S.y(),m_E.y()));
	m_Maxs = QVector2D(max(m_S.x(),m_E.x()),max(m_S.y(),m_E.y()));
}
bool cLine::RenderMe()
{
	if (!g_pLineTool->m_Visible)
		return false;
	
	if (!cBase::RenderMe())
		return false;
	
	if (!m_Selected)
	{
		if (!m_JumpDown)
			glColor3f(0.0f,1.0f,0.f);
		else
			glColor3f(1.0f,0.2f,1.0f);
	}
	else
		glColor3f(1,0,0);
	
	if (!m_JumpDown)
	{
		glLineWidth(2.f);
		glEnable(GL_LINE_SMOOTH);
	}
	QVector2D s(m_S.x(),m_S.y()),e(m_E.x(),m_E.y());
	QVector2D c = s+e;
	c *= 0.5f;

	MGR->DrawLine(s,e);
	
	glLineWidth(1.f);
	glDisable(GL_LINE_SMOOTH);
	
	MGR->DrawLine(c,c+m_N*8);
	
	//if (g_pLineTool->m_Selecting)
	//{
		//glColor3f(1,1,1);
		MGR->FillRect(s-QVector2D(2,2),s+QVector2D(2,2));
		MGR->FillRect(e-QVector2D(2,2),e+QVector2D(2,2));
	//}
	
	return true;
}
