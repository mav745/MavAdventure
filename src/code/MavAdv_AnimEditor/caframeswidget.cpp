#include "caframeswidget.h"
#include <QDropEvent>
#include <QMimeData>
#include <QDebug>

cAFramesWidget::cAFramesWidget(QWidget *parent) : QTableWidget(parent)
{
	
}


void cAFramesWidget::dropEvent(QDropEvent *e)
{
	
	if (e->source()->objectName() != "listStripFrames")
	{
		e->ignore();
		return;
	}
	
	QByteArray encoded = e->mimeData()->data("application/x-qabstractitemmodeldatalist");
	QDataStream stream(&encoded, QIODevice::ReadOnly);
	
	while (!stream.atEnd())
	{
	    int row, col;
	    QMap<int,  QVariant> roleDataMap;
	    stream >> row >> col >> roleDataMap;
	
		//qDebug()<<row<<col<<roleDataMap;
		qDebug()<<roleDataMap;
	    /* do something with the data */
	}
	int col, row;
	col = e->pos().x();
	row = e->pos().y();
	
	col /= columnWidth(0);
	row /= rowHeight(0);
	
	//qDebug()<<col<<row;
	e->accept();
}

void cAFramesWidget::dragEnterEvent(QDragEnterEvent *e)
{
	e->accept();
}
