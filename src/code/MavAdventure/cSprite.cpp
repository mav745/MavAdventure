#include "cSprite.h"
#include "cMainManager.h"
#include <math.h>
#include "util.h"

#include <QDir>
#include <QImage>
#include <QMap>

#include <QDebug>

QMap<QString, cSprite*> g_SpritesMap;

cSprite::cSprite()
{
	m_Orig = QPointF(0.f,0.f);
	m_Frame = 0;
	m_FrameDir = EFD_HOR;
}

float cSprite::SetFrame(float frame, bool loop)
{
	m_Frame = frame;
	if (loop)
	{
		if (m_Frame >= m_numFrames)
			m_Frame -= m_numFrames;
		if (m_Frame < 0.f)
			m_Frame += m_numFrames;
	}
	else
	{
		if (m_Frame >= m_numFrames) m_Frame = m_numFrames-1;
		if (m_Frame < 0.f) m_Frame = 0;
	}
	return m_Frame;
}

void cSprite::Render()
{
	if (m_FrameDir == EFD_HOR)
	{
		float tV[] =
		{
			(     -(float)m_Orig.x()),(   -(float)m_Orig.y()),
			(     -(float)m_Orig.x()),(m_H-(float)m_Orig.y()),
			(m_FrW-(float)m_Orig.x()),(m_H-(float)m_Orig.y()),
			(m_FrW-(float)m_Orig.x()),(   -(float)m_Orig.y())
		};
		
		
		float lU, rU;
		lU = (float)(int(m_Frame))*m_FrW/(float)m_W;
		rU = lU+m_FrW/(float)m_W;
		float tUV[] = 
		{
			lU,1.f,
			lU,0.f,
			rU,0.f,
			rU,1.f
		};
		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		
		glEnable(GL_TEXTURE_2D);
		
		glBindTexture(GL_TEXTURE_2D,m_Tex);
		glBegin(GL_QUADS);
			glTexCoord2f(tUV[0],tUV[1]);
			glVertex2f(tV[0],tV[1]);
			glTexCoord2f(tUV[2],tUV[3]);
			glVertex2f(tV[2],tV[3]);
			glTexCoord2f(tUV[4],tUV[5]);
			glVertex2f(tV[4],tV[5]);
			glTexCoord2f(tUV[6],tUV[7]);
			glVertex2f(tV[6],tV[7]);
		glEnd();
		
		//glEnable(GL_TEXTURE_2D);
		//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		/*
		qDebug()<<lU<<rU;
		qDebug()
				<<tV[0]<<tV[1]<<","
				<<tV[2]<<tV[3]<<","
				<<tV[4]<<tV[5]<<","
				<<tV[6]<<tV[7];
		glEnableClientState(GL_VERTEX_ARRAY); glVertexPointer(2,GL_FLOAT,0,tV);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY); glTexCoordPointer(2,GL_FLOAT,0,tUV);*/
	}
	else if (m_FrameDir == EFD_VER)
	{
		float tV[] =
		{
			(   -(float)m_Orig.x()),(     -(float)m_Orig.y()),
			(   -(float)m_Orig.x()),(m_FrW-(float)m_Orig.y()),
			(m_W-(float)m_Orig.x()),(m_FrW-(float)m_Orig.y()),
			(m_W-(float)m_Orig.x()),(     -(float)m_Orig.y())
		};
		
		
		float uV, dV;
		uV = (float)(int(m_Frame))*m_FrW;
		dV = uV+m_FrW;
		uV /= (float)m_H;
		dV /=(float) m_H;
		float tUV[] = 
		{
			0.f,1.f-uV,//1.f,
			0.f,1.f-dV,//0.f,
			1.f,1.f-dV,//0.f,
			1.f,1.f-uV//,1.f
		};
		
		//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		
		glEnable(GL_TEXTURE_2D);
		glColor3f(1.f,1.f,1.f);
		glBindTexture(GL_TEXTURE_2D,m_Tex);
		glBegin(GL_QUADS);
			glTexCoord2f(tUV[0],tUV[1]);
			glVertex2f(tV[0],tV[1]);
			glTexCoord2f(tUV[2],tUV[3]);
			glVertex2f(tV[2],tV[3]);
			glTexCoord2f(tUV[4],tUV[5]);
			glVertex2f(tV[4],tV[5]);
			glTexCoord2f(tUV[6],tUV[7]);
			glVertex2f(tV[6],tV[7]);
		glEnd();
		
		//glEnable(GL_TEXTURE_2D);
		//glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		
//		glEnableClientState(GL_VERTEX_ARRAY); glVertexPointer(2,GL_FLOAT,0,tV);
//		glEnableClientState(GL_TEXTURE_COORD_ARRAY); glTexCoordPointer(2,GL_FLOAT,0,tUV);
	}
	
	/*int tInds[] = {0,1,2,3};
	glEnableClientState(GL_INDEX_ARRAY);glIndexPointer(GL_INT,0,tInds);
	
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.f,1.f,1.f);
	glBindTexture(GL_TEXTURE_2D,m_Tex);
	glDrawArrays(GL_QUADS,0,4);*/
}

cSprite * cSprite::getSprite(QString fname, int numFr, FRAME_DIR dir, int w, int h)
{
	if (g_SpritesMap.keys().contains(fname))
		return g_SpritesMap[fname];

	QImage *img;//(fname);
	if (fname.right(4) == ".ang")
	{
		QFile ANG(fname);
		if (!ANG.open(QFile::ReadOnly)) return NULL;
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
		img = new QImage(ang_data,ang_w,ang_h,QImage::Format_RGBA8888);
		numFr = ang_numfr;
	}
	else
		img = new QImage(fname);
	
	
	int tex = cMainManager::g_pMainMgr->context()->bindTexture(
					 *img,
					 GL_TEXTURE_2D,
					 GL_RGBA,
					 QGLContext::InvertedYBindOption);
	if (!tex)
	{
		//qDebug()<<"no tex";
		delete img;
		return NULL;
	}
	//qDebug()<<"tex"<<tex;
	cSprite *pSpr = new cSprite;
	pSpr->m_numFrames = numFr;
	pSpr->m_W   = img->width();
	pSpr->m_H   = img->height();
	pSpr->m_Tex = tex;
	if (dir == EFD_HOR)
	{
		pSpr->m_FrW = pSpr->m_W/numFr;
		pSpr->m_Orig = QPointF(pSpr->m_FrW/2,pSpr->m_H/2);
	}
	else
	{
		pSpr->m_FrW = pSpr->m_H/numFr;
		pSpr->m_Orig = QPointF(pSpr->m_W/2,pSpr->m_FrW/2);
	}
	pSpr->m_FrameDir = dir;
	g_SpritesMap[fname] = pSpr;
	
	delete img;
	return pSpr;
}

void cSprite::DrawFrame(const QVector2D &orig, int frame)
{
	m_Frame = frame % m_numFrames;
	//m_Orig.rx() = orig.x();
	//m_Orig.ry() = orig.y();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(floor(orig.x()),floor(orig.y()),0.f);
	Render();
	glLoadIdentity();
}
