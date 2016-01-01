#include "cMainManager.h"
#include "util.h"
#include "cSprite.h"
#include <cGameRoom.h>

#include "cRing.h"

#include <QDebug>

cRing::cRing(QObject *parent) :
	cBaseAnimating(parent)
{
	//m_pTexture = cSprite::getSprite("res/sprites/s_ring2.png",8);
	//m_pTexture->m_Orig = QPointF(12,12);
	
	m_Depth = 0;
	
	AddAnim("res/sprites/s_ring.png","idle",8);
	//SetAnim("idle");
	
	m_Orig[0] = UTIL_RandomInt(0,640);
	m_Orig[1] = 400-m_Id*64;
	m_ImageSpeed = 0.2f;
	m_ImageIndex = m_Id*2;//UTIL_RandomInt(0,10);
	//float sc = 0.01f*(float)UTIL_RandomInt(75,100);
	//m_ImageScale = QVector2D(sc,sc);
	//m_ImageAngle = UTIL_RandomInt(-20,25);
	//qDebug()<<"ring spawned"<<m_Orig;
}

void cRing::RenderMe() 
{
	//glBlendFunc(GL_ONE,GL_ONE);
	cBaseAnimating::RenderMe();
	//glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
}