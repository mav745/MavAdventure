#ifndef CSTRIPSAREA_H
#define CSTRIPSAREA_H

#include <qframe.h>

#include <QVector>

class cBaseNode;

class cNodeArea : public QFrame
{
public:
	explicit cNodeArea(QWidget *parent = 0);
	
	QVector<cBaseNode*> m_FileChildren; //for file hierarchy
	
	void ProcessInput(void);

signals:
	void selected(cBaseNode *pLayer);
protected:
	void paintEvent(QPaintEvent *e);
};

#endif // CSTRIPSAREA_H
