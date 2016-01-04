#ifndef CPREVIEWAREA_H
#define CPREVIEWAREA_H

#include <QFrame>

class CPreviewFrame : public QFrame
{
	Q_OBJECT
	
public:
	QPoint m_Scroll;
	explicit CPreviewFrame(QWidget *parent = 0);
	
protected:
	void paintEvent(QPaintEvent *e);
	void resizeEvent(QResizeEvent *e);
};

#endif // CPREVIEWAREA_H
