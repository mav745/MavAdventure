#ifndef CSTRIPFRAMEWIDGET_H
#define CSTRIPFRAMEWIDGET_H

#include <QListWidget>

class cStripFrameWidget : public QListWidget
{
	Q_OBJECT
	
public:
	cStripFrameWidget(QWidget *parent = 0);
protected:
	void paintEvent(QPaintEvent *e);
};

#endif // CSTRIPFRAMEWIDGET_H
