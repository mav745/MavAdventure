#include "cbasenode.h"
#include <QDir>
#include <QPainter>

QImage * cBaseNode::s_pIconsImg = NULL;

cBaseNode::cBaseNode(QObject *parent) : QObject(parent)
{
	m_State = false;
	m_Type = ENT_GROUP;
	if (!s_pIconsImg)
		s_pIconsImg = new QImage(QDir::currentPath()+"/res/editor/animeditor_layers.png");
}

void cBaseNode::DrawIcon(QPainter *p, QPoint &c)
{
	if (m_Visible)
		p->drawImage(c,*s_pIconsImg,QRect(0,32,16,16));
	
	int top = 48;
	if      (m_Type == ENT_GROUP) top = 0;
	else if (m_Type == ENT_STRIP) top = 16;
	else if (m_Type == ENT_SOUND) top = 24;
	
	p->drawImage(c,*s_pIconsImg,QRect(16*m_State,top,16,16));
}

