#include <cGLManager.h>
#include "cTriggsTool.h"
#include "triggers.h"
#include <algorithm>
#include <QDir>

#include <QDebug>

#include <QGLFramebufferObject>

using namespace std;

//QMap<QString, entity_type_s*>g_TriggEnts;

GLuint g_TrigTex;

QGLFramebufferObject *g_pFrameBuf, *g_pOutlineFrameBuf;

cBaseTrigger::cBaseTrigger(QObject *parent) :
	cBase(parent)
{
	if (!g_TrigTex)
		g_TrigTex = MGR->bindTexture(QImage("res/editor/trigger.png"));
	
	if (!g_pFrameBuf)
	{
		g_pFrameBuf = new QGLFramebufferObject(1280,1024);//640,480);
		g_pOutlineFrameBuf = new QGLFramebufferObject(1280,1024);//640,480);
	}
	//MGR->
}

cBaseTrigger::~cBaseTrigger()
{
	for(int i=m_Parts.size()-1;i>=0;i--)
		delete m_Parts[i];
}
void cBaseTrigger::SetEntityType(QString type)
{
	if (!g_pTriggsTool->m_EntsMap.contains(type)) return;
	if (m_pEntity) delete m_pEntity;
	
	m_pEntity = new entity_t(g_pTriggsTool->m_EntsMap[type]);
	m_pEntity->type_name = g_pTriggsTool->m_EntsMap[type]->type_name;
}
void cBaseTrigger::Shift(const QVector2D &ofs)
{
	for(int i=m_Parts.size()-1;i>=0;i--)
	{
		m_Parts[i]->s    += ofs;
		m_Parts[i]->e    += ofs;
		m_Parts[i]->mins += ofs;
		m_Parts[i]->maxs += ofs;
	}
	RecountMinMax();
}
void cBaseTrigger::RecountMinMax(void)
{
	m_Maxs = QVector2D(-999999,-999999);
	m_Mins = QVector2D( 999999, 999999);
	for(int i=m_Parts.size()-1;i>=0;i--)
	{
		for(int j=0;j<2;j++)
		{
			if (m_Parts[i]->mins[j] < m_Mins[j])
				m_Mins[j] = m_Parts[i]->mins[j];
			
			if (m_Parts[i]->maxs[j] > m_Maxs[j])
				m_Maxs[j] = m_Parts[i]->maxs[j];
		}
	}
	//	qDebug()<<m_Mins<<m_Maxs;
}
bool cBaseTrigger::Crosses(const QVector2D &os, const QVector2D &oe)
{
	for (int i=m_Parts.size()-1;i>=0;i--)
		if (m_Parts[i]->Crosses(os,oe)) return true;
	
	return false;
}
void cBaseTrigger::Merge(cBaseTrigger *o)
{
	m_Parts += o->m_Parts;
	o->m_Parts.clear();
	REMOVE_OBJECT(o);
	
	RecountMinMax();
}
bool cBaseTrigger::RenderMe()
{
	//glColor4f(1,1,1,1);	
	
	//MGR->OutlineRect(m_Mins,m_Maxs);
	
	if (!cBase::RenderMe()) return false;
	
	
	int sz = m_Parts.size();
	
	
//	g_pOutlineFrameBuf->bind();
	
//	glClearColor(0,0,0,0);
//	glClear(GL_COLOR_BUFFER_BIT);
	
//	//if (!m_pEntity)
//		glColor4f(1.f,0.75f,0.f,1.f);
//	//else
//	//	glColor4f(1.f,0.f,0.75f,1.f);
	
//	glBindTexture(GL_TEXTURE_2D,0);
	
//	glMatrixMode(GL_MODELVIEW);
//	for(int x = -1;x<=1;x++)
//	for(int y = -1;y<=1;y++)
//	{
//		glLoadIdentity();
//		glTranslatef((MGR->m_View.x()+x),(MGR->m_View.y()+y),0.f);
	
//		for(int i=0;i<sz;i++)
//			m_Parts[i]->DrawMe();
//	}
//	glLoadIdentity();
//	glTranslatef(MGR->m_View.x(),MGR->m_View.y(),0.f);
	
//	glColor4f(1,1,1,0);
	
//	glBlendFunc(GL_ONE,GL_ZERO);
//	for(int i=0;i<sz;i++)
//		m_Parts[i]->DrawMe();
//	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	
	g_pFrameBuf->bind();
	
	
	glClearColor(0,0,0,0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	//if (!m_pEntity)
		glColor4f(0.972549f,0.603921f,0.f,1.f);
	//else
	//	glColor4f(1.f,0.f,0.75f,1.f);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(MGR->m_View.x(),MGR->m_View.y(),0.f);
	
	glBindTexture(GL_TEXTURE_2D,g_TrigTex);
	
	for(int i=0;i<sz;i++)
		m_Parts[i]->DrawMe();
	
	glLoadIdentity();
	
	g_pFrameBuf->release();
	
	if (m_Selected) glColor4f(1.f,0.f,0.f,0.75f);
	else glColor4f(1.f,1.f,1.f,0.75f);
	
	float u = MGR->m_ViewSize.x()/1280.f,v = MGR->m_ViewSize.y()/1024.f;
	float uvs[]   = {0,v, 0,0, u,0, u,v};
	
	float vx = MGR->m_ViewSize.x()*MGR->m_Zoom,vy = MGR->m_ViewSize.y()*MGR->m_Zoom;
	float verts[] = {0,0, 0,vy, vx,vy, vx,0};
	
	glEnableClientState(GL_VERTEX_ARRAY); 
	glVertexPointer(2,GL_FLOAT,0,verts);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
	glTexCoordPointer(2,GL_FLOAT,0,uvs);
	
//	glBindTexture(GL_TEXTURE_2D,g_pOutlineFrameBuf->texture());
//	glDrawArrays(GL_QUADS,0,4);
	
	if (m_Selected) glColor4f(1.f,0.f,0.f,0.5f);
	else glColor4f(1.f,1.f,1.f,0.5f);
	
	glBindTexture(GL_TEXTURE_2D,g_pFrameBuf->texture());
	glDrawArrays(GL_QUADS,0,4);
	
//	glBegin(GL_QUADS);
//	glTexCoord2f(0,480.f/MGR->m_ViewSize.y());
//	glVertex2f(0,0);
//	glTexCoord2f(640.f/MGR->m_ViewSize.x(),480.f/MGR->m_ViewSize.y());
//	glVertex2f(MGR->m_ViewSize.x()*MGR->m_Zoom,0);
//	glTexCoord2f(640.f/MGR->m_ViewSize.x(),0);
//	glVertex2f(MGR->m_ViewSize.x()*MGR->m_Zoom,MGR->m_ViewSize.y()*MGR->m_Zoom);
//	glTexCoord2f(0,0);
//	glVertex2f(0,MGR->m_ViewSize.y()*MGR->m_Zoom);
//	glEnd();
	
	
	
//	glBegin(GL_QUADS);
//	glTexCoord2f(0,480.f/MGR->m_ViewSize.y());
//	glVertex2f(0,0);
//	glTexCoord2f(640.f/MGR->m_ViewSize.x(),480.f/MGR->m_ViewSize.y());
//	glVertex2f(MGR->m_ViewSize.x()*MGR->m_Zoom,0);
//	glTexCoord2f(640.f/MGR->m_ViewSize.x(),0);
//	glVertex2f(MGR->m_ViewSize.x()*MGR->m_Zoom,MGR->m_ViewSize.y()*MGR->m_Zoom);
//	glTexCoord2f(0,0);
//	glVertex2f(0,MGR->m_ViewSize.y()*MGR->m_Zoom);
//	glEnd();
	
	return true;
}
bool cBaseTrigger::ShapeHasPoint(const QVector2D &p)
{
	for(int i=m_Parts.size()-1;i>=0;i--)
		if (m_Parts[i]->HasPoint(p)) return true;
		
	return false;
}
trig_part_rect_s::trig_part_rect_s(const QVector2D &v1, const QVector2D &v2)
{
	for(int i=0;i<2;i++)
	{
		s[i] = min(v1[i],v2[i]);//+1.f;
		e[i] = max(v1[i],v2[i]);//-1.f;
	}
	RecalcBounds();
}

void trig_part_rect_s::RecalcBounds()
{
	for(int i=0;i<2;i++)
	{
		mins[i] = min(s[i],e[i]);
		maxs[i] = max(s[i],e[i]);
	}
}
void trig_part_rect_s::DrawMe()
{
	glBegin(GL_QUADS);
		glTexCoord2f(s.x()/64.f,-s.y()/64.f);
		glVertex2f(s.x(),s.y());
		glTexCoord2f(e.x()/64.f,-s.y()/64.f);
		glVertex2f(e.x(),s.y());
		glTexCoord2f(e.x()/64.f,-e.y()/64.f);
		glVertex2f(e.x(),e.y());
		glTexCoord2f(s.x()/64.f,-e.y()/64.f);
		glVertex2f(s.x(),e.y());

	glEnd();
}
bool trig_part_rect_s::HasPoint(const QVector2D &p)
{
	if (p.x() < s.x()) return false;
	if (p.x() > e.x()) return false;
	
	if (p.y() < s.y()) return false;
	if (p.y() > e.y()) return false;
	
	return true;
}
bool trig_part_rect_s::Crosses(const QVector2D &os, const QVector2D &oe)
{
	trig_part_rect_s o(os,oe);
	
	if (HasPoint(o.s)) return true;
	if (HasPoint(o.e)) return true;
	if (HasPoint(QVector2D(o.s.x(),o.e.y()))) return true;
	if (HasPoint(QVector2D(o.e.x(),o.s.y()))) return true;
	
	if (o.HasPoint(s)) return true;
	if (o.HasPoint(e)) return true;
	if (o.HasPoint(QVector2D(s.x(),e.y()))) return true;
	if (o.HasPoint(QVector2D(e.x(),s.y()))) return true;
	
	if ((s.y() < o.s.y() && e.y() > o.e.y()) && (s.x() >= o.s.x() && e.x() <= o.e.x()))
			return true;
	
	if ((o.s.y() < s.y() && o.e.y() > e.y()) && (o.s.x() >= s.x() && o.e.x() <= e.x()))
			return true;
	
	return false;
}
