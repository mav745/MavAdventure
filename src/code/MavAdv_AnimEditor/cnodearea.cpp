#include "cnodearea.h"
#include <mainwindow.h>
#include <ui_mainwindow.h>

cNodeArea::cNodeArea(QWidget *parent) : QFrame(parent)
{
	
}

void cNodeArea::ProcessInput()
{
	if (!hasFocus()) return;
	
	
}

void cNodeArea::paintEvent(QPaintEvent *e)
{
	
	QFrame::paintEvent(e);
}
