#ifndef CANIMFRAMEBAR_H
#define CANIMFRAMEBAR_H

#include <QFrame>

class CAnimFrameBar : public QFrame
{
public:
	explicit CAnimFrameBar(QWidget *parent = 0);
protected:
	void paintEvent(QPaintEvent *e);
};

#endif // CANIMFRAMEBAR_H
