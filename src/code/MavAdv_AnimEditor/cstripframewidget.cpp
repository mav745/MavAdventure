#include "cstripframewidget.h"

#include <QMimeData>

#include <QDebug>
#include <QPainter>

cStripFrameWidget::cStripFrameWidget(QWidget *parent) : QListWidget(parent)
{
	setGridSize(QSize(171,64));
	setIconSize(QSize(171,64));
}


void cStripFrameWidget::paintEvent(QPaintEvent *e)
{
	QListWidget::paintEvent(e);
	/*int grid_h = this->gridSize().height();
	QList<QListWidgetItem*> lItems = this->findItems("*",Qt::MatchWildcard);
	//qDebug()<<lItems;
	
	QPainter p(viewport());
	
	QIcon *pIco;
	for(int i=0,h=0; i<lItems.size(); i++,h+=grid_h)
	{
		pIco = (QIcon*)lItems[i]->data(0).toInt();
		p.drawPixmap(0,h,pIco->pixmap(16,16));
	}*/
}
