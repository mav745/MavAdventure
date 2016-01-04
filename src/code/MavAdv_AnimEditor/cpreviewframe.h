#ifndef CPREVIEWAREA_H
#define CPREVIEWAREA_H

#include <QFrame>

class CPreviewFrame : public QFrame
{
	Q_OBJECT
	
	bool m_Init;

    public:
        QPoint m_Scroll;
        QImage m_Bkg;
        int m_Scale;
        explicit CPreviewFrame(QWidget *parent = 0);
        void ProcessInput(void);

    protected:
        void paintEvent(QPaintEvent *e);
        void resizeEvent(QResizeEvent *e);
};

#endif // CPREVIEWAREA_H
