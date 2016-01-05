#include "canimframebar.h"

CAnimFrameBar::CAnimFrameBar(QWidget *parent) : QFrame(parent)
{
	
}

void CAnimFrameBar::paintEvent(QPaintEvent *e)
{
	QFrame::paintEvent(e);
}
