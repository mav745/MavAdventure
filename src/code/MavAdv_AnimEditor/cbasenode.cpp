#include "cbasenode.h"
#include <QDir>
#include <QPainter>

QImage * cBaseNode::s_pIconsImg = NULL;

cBaseNode::cBaseNode(int type) : QTreeWidgetItem(type)
{
	m_Visible = true;
	m_State = false;
	m_Type = ENT_GROUP;
	if (!s_pIconsImg)
		s_pIconsImg = new QImage(QDir::currentPath()+"/res/editor/animeditor_layers.png");
}

void cBaseNode::DrawIcon(QPainter *p, QPoint &c)
{
	if (m_Visible)
		p->drawImage(QPoint(4,c.y()),*s_pIconsImg,QRect(0,48,16,16));
	
	int top = 64;
	if      (m_Type == ENT_GROUP) top = 0;
	else if (m_Type == ENT_STRIP) top = 16;
	else if (m_Type == ENT_SOUND) top = 32;
	
	p->drawImage(c+QPoint(16,0),*s_pIconsImg,QRect(16*m_State,top,16,16));
	p->drawText(c.x()+34, c.y(), 256, 16, Qt::AlignTop|Qt::AlignLeft, m_Name);
	
	c.ry() += 16;
	
	if (m_Type == ENT_GROUP && !m_State)
	{
		c.rx() += 8;
		for(int i=0;i<m_FileChildren.size();i++)
			m_FileChildren[i]->DrawIcon(p,c);
		c.rx() -= 8;
	}
}

bool cBaseNode::Select(const QPoint &c)
{
	return false;
}

