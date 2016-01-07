#ifndef CAFRAMESWIDGET_H
#define CAFRAMESWIDGET_H

#include <QTableWidget>
#include <QWidget>

class cAFramesWidget : public QTableWidget
{
	Q_OBJECT
public:
	explicit cAFramesWidget (QWidget *parent = 0);
	
protected:
	void dropEvent(QDropEvent *e);
	void dragEnterEvent(QDragEnterEvent *e);
};

#endif // CAFRAMESWIDGET_H
