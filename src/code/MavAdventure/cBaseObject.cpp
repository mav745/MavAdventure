#include "cBaseObject.h"
#include "cMainManager.h"
#include "cSprite.h"
#include "util.h"
#include <QDebug>
#include "cSprite.h"
#include <math.h>
#include <cGameRoom.h>

int gObjNum;
extern int gTestTileset;

cBaseObject::cBaseObject(QObject *parent) :
	cBaseRenderable(parent)
{
	m_TargetName.clear();
	m_Target.clear();
	m_ScrollFac = QVector2D(1.f,1.f);
//	m_Tail = QPointF(0.f,0.f);
	
// ----------------------------------------------
	
	m_ImageAlpha = 1.f;
	m_ImageIndex = 0.f;
	m_ImageSpeed = 0.f;
	m_ImageAngle = 0.f;
	m_ImageScale = QVector2D(1,1);
	//m_Depth = 0;
	m_Id = gObjNum;
	gObjNum++;
	
	//cSprite *pSpr;
	/*if (m_Id == 0)
	{
		m_ImageSpeed = 0.5f;
		m_Orig = QPoint(320,400);//UTIL_RandomInt(0,640),UTIL_RandomFloat(0,480));
		pSpr = cSprite::getSprite("res/sprites/s_mav_walk.png",10);
		pSpr->m_Orig = QPointF(32,32);
	}
	else
	{
		*/
//		m_Orig = QPoint(UTIL_RandomInt(0,640),400-m_Id*64);
//		pSpr = cSprite::getSprite();
//		pSpr->m_Orig = QPointF(16,16);
//		m_ImageAngle = UTIL_RandomInt(0,360);
	//}
	
	//m_pTexture = pSpr;
	//SetSpeedDirection(UTIL_RandomFloat(0.2f,5.f),UTIL_RandomFloat(0,360));
}
void cBaseObject::SetSpeedDirection(float s, float d)
{
	m_Velocity = UTIL_LengthDir(s,d); 
}
void cBaseObject::Step()
{
	//m_ImageAngle = UTIL_PointDirection(m_Orig,UTIL_mouseCoords());
	//SetSpeedDirection(3.f,m_ImageAngle);
	//qDebug()<<this<<"m_Velocity"<<m_Velocity;
	CalcCoords(m_Velocity);
	
	if (!m_pMyRoom->m_Block[EBS_LEFT] && !m_pMyRoom->m_Block[EBS_RIGHT])
	{
		int R,L,HMarg;
		HMarg = m_pMyRoom->m_Size[XX]-640;
		HMarg /= 2;
		R = 640+HMarg;
		L = -HMarg;
		if (m_Orig[XX] > R) m_Orig[XX] -= m_pMyRoom->m_Size[XX];
		if (m_Orig[XX] < L) m_Orig[XX] += m_pMyRoom->m_Size[XX];
	}
	if (!m_pMyRoom->m_Block[EBS_DOWN] && !m_pMyRoom->m_Block[EBS_UP])
	{
		int U,D,VMarg;
		VMarg = m_pMyRoom->m_Size[YY]-480;
		VMarg /= 2;
		D = 480+VMarg;
		U = -VMarg;
		if (m_Orig[YY] > D) m_Orig[YY] -= m_pMyRoom->m_Size[YY];
		if (m_Orig[YY] < U) m_Orig[YY] += m_pMyRoom->m_Size[YY];
	}
	
	
	//qDebug()<<m_Id<<m_ImageIndex;
	//m_Orig += m_Velocity*MGR->m_DeltaTime;
	
	/*if (m_Orig[X] >= 640.f && m_Velocity[X] > 0.f) m_Velocity[X] *= -1.f;
	if (m_Orig[X] <= 0.f && m_Velocity[X] < 0.f) m_Velocity[X] *= -1.f;
	
	if (m_Orig[Y] >= 480.f && m_Velocity[Y] > 0.f) m_Velocity[Y] *= -1.f;
	if (m_Orig[Y] <= 0.f && m_Velocity[Y] < 0.f) m_Velocity[Y] *= -1.f;*/
}

void cBaseObject::Use(cBaseObject *pOther)
{
	
}
void cBaseObject::Draw()
{
	//qDebug()<<this<<"m_Velocity"<<m_Velocity;
	//m_ImageAngle += 1.f;
	//if (m_ImageAngle > 360.f) m_ImageAngle -= 360.f;
	//if (m_ImageAngle <   0.f) m_ImageAngle -= 360.f;
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glTranslatef(floor(m_Orig.x()),floor(m_Orig.y()),0.f);
	glRotatef(m_ImageAngle,0,0,1);
	glScalef(m_ImageScale.x(),m_ImageScale.y(),1);
	glColor4f(1.f,1.f,1.f,m_ImageAlpha);
	
	if (m_pTexture)
	{
		m_pTexture->Render();
	}
	/*float tVerts[] = 
	{
		-16.f,-16.f,
		 16.f,-16.f,
		 16.f, 16.f,
		-16.f, 16.f
	};
	int tInds[] = {0,1,2,3};
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_INDEX_ARRAY);
	glVertexPointer(2,GL_FLOAT,0,tVerts);
	glIndexPointer(GL_INT,0,tInds);
	
	glBindTexture(GL_TEXTURE_2D,0);
	glColor3f(1.f,1.f,1.f);
	glDrawArrays(GL_LINE_LOOP,0,4);*/
	
	//MGR->drawText(m_Orig[X]-4,m_Orig[Y]+6,"+");
	
	glPopMatrix();

//	MGR->drawText(m_Orig[X]-32,m_Orig[Y]-12,
//					  QString::number((int)m_Orig[X])+','+
	//					  QString::number((int)m_Orig[Y]));
}
void cBaseObject::CalcCoords(const QVector2D &add)
{
	
	//for(int i=0;i<2;i++)
	m_Orig += add*MGR->m_DeltaTime;
	//DBG_PRINT("add"+QString);
	//qDebug()<<"add"<<add;
}
//void cBaseObject::ScrollMe(QPoint scr)
//{
//	QPointF fScr(scr.x(),scr.y());
//	fScr.rx() *= m_ScrollFac.x();
//	fScr.ry() *= m_ScrollFac.y();
	
//	CalcCoords(QVector2D(fScr.x(),fScr.y()));
//}

void cBaseObject::ParseKeyValue(const QString &key, const QString &value)
{
	if (key[0] == '@')
	{
		return;
	}
	else if (key == "targetname")
	{
		m_pMyRoom->ObjectSetTargetname(this,value);
	}
	else if (key == "target")
	{
		m_Target = value;
	}
	else if (key == "x_scroll")
	{
		m_ScrollFac[0] = value.toFloat();
	}
	else if (key == "y_scroll")
	{
		m_ScrollFac[1] = value.toFloat();
	}
	else
	{
		//DBG_PRINT_S("Unparsed: "+key+", "+value);
	}
}
