#include "cnodearea.h"
#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <cbasenode.h>
#include <QPainter>

cNodeArea::cNodeArea(QWidget *parent) : QFrame(parent)
{
	
}

void cNodeArea::ProcessInput()
{
	if (!hasFocus()) return;
	
	
}

void cNodeArea::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	QPoint c(4,4);
	for(int i=0;i<m_FileChildren.size();i++)
	{
		m_FileChildren[i]->DrawIcon(&p,c);
	}
	QFrame::paintEvent(e);
}
