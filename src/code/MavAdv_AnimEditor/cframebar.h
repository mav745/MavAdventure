#ifndef CANIMFRAMEBAR_H
#define CANIMFRAMEBAR_H

#include <QFrame>

class cFrameBar : public QFrame
{
public:
	explicit cFrameBar(QWidget *parent = 0);
protected:
	void paintEvent(QPaintEvent *e);
};

#endif // CANIMFRAMEBAR_H
