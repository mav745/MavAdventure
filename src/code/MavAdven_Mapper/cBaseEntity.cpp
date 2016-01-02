#include "cBaseEntity.h"
#include "cGLManager.h"
#include <cEntityTool.h>
#include <QDir>
#include <QDebug>

extern bool g_ScrollEf;

cBaseEntity::cBaseEntity(QObject *parent) : cBase(parent)
{
	m_Texture = false;
	m_ScrollFac = m_ImageScale = QVector2D(1.f,1.f);
	//m_Selected = true;
}

void cBaseEntity::SetEntityType(QString type)
{
	if (!g_pEntityTool->m_EntsMap.contains(type)) return;
	if (m_pEntity) delete m_pEntity;
	m_pEntity = new entity_t( g_pEntityTool->m_EntsMap[type]);
	m_pEntity->type_name    = g_pEntityTool->m_EntsMap[type]->type_name;
	m_Mins  = *(QVector2D*)m_pEntity->fields["@mins" ]->data;
	m_Maxs  = *(QVector2D*)m_pEntity->fields["@maxs" ]->data;
	m_Color = *(QVector3D*)m_pEntity->fields["@color"]->data;
	m_Color /= 255.f;
	
	if (m_pEntity->fields.contains("@sprite"))
	{
		QString sprfile = *(QString*)m_pEntity->fields["@sprite"]->data;
		if (!sprfile.isEmpty())
		{
			QImage spr(QDir::currentPath()+"/"+sprfile);
			m_Texture = MGR->bindTexture(spr,GL_TEXTURE_2D,GL_RGBA,QGLContext::NoBindOption);
		}
	}
}

bool cBaseEntity::HasPoint(const QVector2D &v)
{
	for(int i=0;i<2;i++)
	{
		if (v[i] < m_Orig[i]+m_Mins[i]) return false;
		if (v[i] > m_Orig[i]+m_Maxs[i]) return false;
	}
	return true;
}

bool cBaseEntity::RenderMe()
{
	bool drawANG = false;
	float frameStart = 0;
	if (m_pEntity->fields.contains("@sprite") &&
		 m_pEntity->fields.contains("sprite_file"))
	{
		QString sprfile_cur = *(QString*)m_pEntity->fields["@sprite"	 ]->data;
		QString sprfile_new = *(QString*)m_pEntity->fields["sprite_file"]->data;
//		qDebug()<<sprfile_cur;
//		qDebug()<<sprfile_new;
//		qDebug()<<"-------------------------";
		if (sprfile_new.right(4) == ".ang")
		{
			drawANG = true;
			if (m_pEntity->fields.contains("start_frame"))
				frameStart = m_pEntity->fields["start_frame"]->asString().toFloat();
			if (m_pEntity->fields.contains("image_speed"))
				m_AnimSpeed = m_pEntity->fields["image_speed"]->asString().toFloat();
		}
		if (sprfile_cur != sprfile_new)
		{
			
			
			if (m_Texture)
				MGR->deleteTexture(m_Texture);

			sprfile_cur = sprfile_new;
			m_pEntity->fields["@sprite"]->fromString(sprfile_new);
			
			QImage *spr = NULL;
			if (drawANG)//(sprfile_cur.right(4) == ".ang")
			{
				QFile ANG(QDir::currentPath()+"/"+sprfile_cur);
				if (ANG.open(QFile::ReadOnly))
				{
					int ang_numfr,ang_w,ang_h;
					QDataStream ANG_DS(&ANG);
					ANG_DS >> ang_numfr >> ang_w >> ang_h;
					uchar *ang_data = new uchar[ang_w*ang_h*4];
					ANG_DS.readRawData((char*)ang_data,ang_w*ang_h*4);
					int r;
					for(int i=0;i<ang_w*ang_h*4;i+=4)
					{
						r = ang_data[i+2];
						ang_data[i+2] = ang_data[i];
						ang_data[i] = r;
					}
					spr = new QImage(ang_data,ang_w,ang_h,QImage::Format_RGBA8888);
					m_SprW = ang_w;
					m_SprH = ang_h;
					m_FrameW = ang_w/ang_numfr;
					m_FrameH = ang_h;
					m_NumFrames = ang_numfr;
//					qDebug()<<"m_NumFrames"<<m_NumFrames;
//					qDebug()<<"m_FrameW"<<m_FrameW;
//					qDebug()<<"m_SprW"<<m_SprW;
					ANG.close();
				}
			}
			else
			{
				spr = new QImage(QDir::currentPath()+"/"+sprfile_cur);
			}
			//QImage spr(QDir::currentPath()+"/"+sprfile_cur);
			if (spr && !spr->isNull())
			{
				m_Texture = MGR->bindTexture(*spr,GL_TEXTURE_2D,GL_RGBA,QGLContext::NoBindOption);
				if (drawANG)
				{
					m_Mins = -QVector2D(m_FrameW/2.f,m_FrameH/2.f);
					m_Maxs = -m_Mins;
				}
				else
				{
					m_Mins = -QVector2D(spr->width()/2.f,spr->height()/2.f);
					m_Maxs = -m_Mins;
				}
				
			}
			else
				m_Texture = 0;
			if (spr) delete spr;
		}
	}
	if (m_pEntity->fields.contains("x_scale"))
		m_ImageScale[0] = *(float*)m_pEntity->fields["x_scale"]->data;
	if (m_pEntity->fields.contains("y_scale"))
		m_ImageScale[1] = *(float*)m_pEntity->fields["y_scale"]->data;
	
	if (m_pEntity->fields.contains("x_scroll"))
		m_ScrollFac[0] = *(float*)m_pEntity->fields["x_scroll"]->data;
	if (m_pEntity->fields.contains("y_scroll"))
		m_ScrollFac[1] = *(float*)m_pEntity->fields["y_scroll"]->data;
	
	//random comment here
	
	if (m_Texture)
	{
		glBindTexture(GL_TEXTURE_2D,m_Texture);
		//if (m_Selected)
		//	glColor4f(1.f,1.f,1.f,0.5f);
		//else
		if (m_pEntity->fields.contains("image_alpha"))
			glColor4f(1.f,1.f,1.f,m_pEntity->fields["image_alpha"]->asString().toFloat());
		else
			glColor3f(1.f,1.f,1.f);
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D,0);
		if (m_Selected)
			glColor4f(1.f,0.f,0.f,0.35f);
		else
			glColor4f(m_Color.x(),m_Color.y(),m_Color.z(),0.35f);
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (g_ScrollEf)
		glTranslatef(MGR->m_View.x()*m_ScrollFac.x(),MGR->m_View.y()*m_ScrollFac.y(),0.f);
	else
		glTranslatef(MGR->m_View.x(),MGR->m_View.y(),0.f);
	glTranslatef(m_Orig.x(),m_Orig.y(),0.f);
	
	glBegin(GL_QUADS);
		if (drawANG)
		{
			float frameStep = 1.f/m_NumFrames;//m_FrameW/m_SprW;
			frameStart = frameStart + MGR->m_Time*m_AnimSpeed;
			frameStart = fmod(frameStart,m_NumFrames);
			//qDebug()<<"frameStart"<<frameStart;
			frameStart = (int)frameStart;
			frameStart *= frameStep;

			glTexCoord2f(frameStart,0);
			glVertex2f(m_Mins.x()*m_ImageScale.x(),m_Mins.y()*m_ImageScale.y());
			
			glTexCoord2f(frameStart,1);
			glVertex2f(m_Mins.x()*m_ImageScale.x(),m_Maxs.y()*m_ImageScale.y());
			
			glTexCoord2f(frameStart+frameStep,1);
			glVertex2f(m_Maxs.x()*m_ImageScale.x(),m_Maxs.y()*m_ImageScale.y());
			
			glTexCoord2f(frameStart+frameStep,0);
			glVertex2f(m_Maxs.x()*m_ImageScale.x(),m_Mins.y()*m_ImageScale.y());
		}
		else
		{
			glTexCoord2f(0,0);
			glVertex2f(m_Mins.x()*m_ImageScale.x(),m_Mins.y()*m_ImageScale.y());
			
			glTexCoord2f(0,1);
			glVertex2f(m_Mins.x()*m_ImageScale.x(),m_Maxs.y()*m_ImageScale.y());
			
			glTexCoord2f(1,1);
			glVertex2f(m_Maxs.x()*m_ImageScale.x(),m_Maxs.y()*m_ImageScale.y());
			
			glTexCoord2f(1,0);
			glVertex2f(m_Maxs.x()*m_ImageScale.x(),m_Mins.y()*m_ImageScale.y());
		}
	glEnd();
	
	if (!m_Texture || m_Selected)
	{
		glBindTexture(GL_TEXTURE_2D,0);
		if (m_Selected)
		{
			glLineStipple(1,0x3333);
			glEnable(GL_LINE_STIPPLE);
			glColor3f(1.f,0.f,0.f);
		}
		else
			glColor3f(m_Color.x(),m_Color.y(),m_Color.z());
		glBegin(GL_LINE_LOOP);
		if (drawANG)
		{
			glVertex2f(-m_FrameW/2*m_ImageScale.x(),m_Mins.y()*m_ImageScale.y());
			glVertex2f(-m_FrameW/2*m_ImageScale.x(),m_Maxs.y()*m_ImageScale.y());
			glVertex2f( m_FrameW/2*m_ImageScale.x(),m_Maxs.y()*m_ImageScale.y());
			glVertex2f( m_FrameW/2*m_ImageScale.x(),m_Mins.y()*m_ImageScale.y());
		}
		else
		{
			glVertex2f(m_Mins.x()*m_ImageScale.x(),m_Mins.y()*m_ImageScale.y());
			glVertex2f(m_Mins.x()*m_ImageScale.x(),m_Maxs.y()*m_ImageScale.y());
			glVertex2f(m_Maxs.x()*m_ImageScale.x(),m_Maxs.y()*m_ImageScale.y());
			glVertex2f(m_Maxs.x()*m_ImageScale.x(),m_Mins.y()*m_ImageScale.y());
		}
		glEnd();
		glDisable(GL_LINE_STIPPLE);
	}
	if (m_Selected)
	{
		glColor3f(1.f,0.f,0.f);
		glBegin(GL_LINES);
			glVertex2f(-4.f,-4.f);
			glVertex2f( 4.f, 4.f);
			glVertex2f(-4.f, 4.f);
			glVertex2f( 4.f,-4.f);
		glEnd();
	}
	glLoadIdentity();
	if (m_pEntity->fields.contains("targetname"))
	{
		//if (m_Selected)
		//	glColor3f(1.f,0.f,0.f);
		//else
		glColor3f(1.f,1.f,1.f);
		
		if (g_ScrollEf)
			MGR->drawText(
					m_Orig.x()*m_ScrollFac.x(),
					m_Orig.y()*m_ScrollFac.x()+m_Maxs.y()+10.f,
					m_pEntity->fields["targetname"]->asString(),ETA_CENTER);
		else
			MGR->drawText(
					m_Orig.x(),
					m_Orig.y()+m_Maxs.y()+10.f,
					m_pEntity->fields["targetname"]->asString(),ETA_CENTER);
		//glColor4f(1.f,1.f,1.f,1.f);
	}
	if (!m_Texture && m_pEntity)
	{
		//if (m_Selected)
		//	glColor3f(1.f,0.f,0.f);
		//else
		
		glColor3f(1.f,1.f,1.f);//m_Color.x(),m_Color.y(),m_Color.z(),0.5f);
			
		if (g_ScrollEf)
			MGR->drawText(
					m_Orig.x()*m_ScrollFac.x(),
					m_Orig.y()*m_ScrollFac.x()+m_Mins.y()-10.f,m_pEntity->type_name,ETA_CENTER);
		else
			MGR->drawText(
					m_Orig.x(),
					m_Orig.y()+m_Mins.y()-10.f,m_pEntity->type_name,ETA_CENTER);
		//glColor4f(1.f,1.f,1.f,1.f);
	}
	return 1;
}

