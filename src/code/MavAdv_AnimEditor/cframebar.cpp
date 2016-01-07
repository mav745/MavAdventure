#include "cframebar.h"

cFrameBar::cFrameBar(QWidget *parent) : QFrame(parent)
{
	
}

void cFrameBar::paintEvent(QPaintEvent *e)
{
	QFrame::paintEvent(e);
}
