#include "cpreviewframe.h"
#include <mainwindow.h>
#include <ui_mainwindow.h>

#include <QPainter>

#include <QResizeEvent>

CPreviewFrame::CPreviewFrame(QWidget *parent) : QFrame(parent)
{
	m_Scroll = QPoint(0,0);
}



void CPreviewFrame::resizeEvent(QResizeEvent *e)
{

}

void CPreviewFrame::paintEvent(QPaintEvent *e)
{
	
	QPainter p(this);
	p.translate(-m_Scroll.x(),-m_Scroll.y());
	p.drawLine(-32,-32,128,128);
	QFrame::paintEvent(e);
}
