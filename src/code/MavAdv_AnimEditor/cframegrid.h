#ifndef CAFRAMESWIDGET_H
#define CAFRAMESWIDGET_H

#include <QTableWidget>
#include <QWidget>

class cFrameGrid : public QTableWidget
{
	Q_OBJECT
public:
	explicit cFrameGrid (QWidget *parent = 0);
	
protected:
	void dropEvent(QDropEvent *e);
	void dragEnterEvent(QDragEnterEvent *e);
};

#endif // CAFRAMESWIDGET_H
