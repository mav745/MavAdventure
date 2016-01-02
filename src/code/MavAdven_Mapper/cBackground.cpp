#include "cBackground.h"
#include <QDir>
#include <cGLManager.h>
#include <QDebug>

#include <QDataStream>

extern bool g_ScrollEf;

cBackground::cBackground(QObject *parent) :
	cBase(parent)
{
	m_Filename.clear();
	m_Origin  = QVector2D(0,0);
	m_ScrFac  = QVector2D(1,1);
	m_ScrSpd  = QVector2D(0,0);
	m_HRep    = true;
	m_VRep    = true;
	m_Visible = true;
	m_Texture = 0;
	m_BackImg = NULL;
	m_Width   = 0;
	m_Height  = 0;
}

void cBackground::AssignBackground(QString fname)
{
	if (m_Texture)
	{
		MGR->deleteTexture(m_Texture);
		delete m_BackImg;
	}
	//fname = fname.replace("/","\\");
	//qDebug()<<"fname"<<fname;
	m_BackImg = new QImage(fname);
	//qDebug()<<"image"<<m_TSetImg->isNull();
	m_Texture = MGR->context()->bindTexture(
					*m_BackImg,GL_TEXTURE_2D,GL_RGBA,QGLContext::InvertedYBindOption);
	m_Width  = m_BackImg->width();
	m_Height = m_BackImg->height();
	m_Filename = fname.remove(QDir::currentPath());
}

bool cBackground::RenderMe()
{
	if (!m_Visible) return false;
	
	QVector2D orig = m_Origin;
	if (g_ScrollEf)
		orig += MGR->m_View*m_ScrFac;
	else
		orig += MGR->m_View;
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,m_Texture);
	
	glColor3f(1.f,1.f,1.f);
	//0  1
	//2  3
	//4  5
	//6  7
	float w,h;
	w = MGR->m_ViewSize.x()*MGR->m_Zoom;
	h = MGR->m_ViewSize.y()*MGR->m_Zoom;
	float verts[8];
	float   uvs[8];
	if (m_HRep) 
	{
		verts[0] = verts[6] = 0.f;
		verts[2] = verts[4] =   w;
		uvs[0] = uvs[6] = (-floor(orig.x())  )/m_Width;
	   uvs[2] = uvs[4] = (-floor(orig.x())+w)/m_Width;
	}
	else
	{
		verts[0] = verts[6] = floor(orig.x())      ;
		verts[2] = verts[4] = floor(orig.x())+m_Width;
		uvs[0] = uvs[6] = 0.f;
		uvs[2] = uvs[4] = 1.f;
	}
	
	if (m_VRep) 
	{
		verts[1] = verts[3] = 0.f;
		verts[5] = verts[7] =   h;
		uvs[1] = uvs[3] = (floor(orig.y())+h)/m_Height;
	   uvs[5] = uvs[7] = (floor(orig.y())  )/m_Height;
	}
	else
	{
		verts[1] = verts[3] = floor(orig.y())       ;
		verts[5] = verts[7] = floor(orig.y())+m_Height;
		uvs[1] = uvs[3] = 1.f;
		uvs[5] = uvs[7] = 0.f;
	}
	
	glBegin(GL_QUADS);
	for(int i=0;i<8;i+=2)
	{
		glTexCoord2f(uvs[i]  ,uvs[i+1]  );
		glVertex2f  (verts[i],verts[i+1]);
	}
	glEnd();
	
	return true;
}

void cBackground::SaveMe(QDataStream *DS)
{
	//DS<<m_Filename<<m_Depth<<m_Origin<<m_ScrFac<<m_ScrSpd<<m_HRep<<m_VRep;
}

void cBackground::LoadMe(QDataStream *DS)
{
	//DS>>/*m_Filename>>m_Depth>>*/m_Origin>>m_ScrFac>>m_ScrSpd>>m_HRep>>m_VRep;
}
