#ifndef CSTRIPFRAMESFRAME_H
#define CSTRIPFRAMESFRAME_H

#include <QFrame>
#include <util.h>
#include <QVector>

#define TOP_SPACING 8
#define FRAME_SPACING 16

class cBaseNode;

class CStripFramesFrame : public QFrame
{
    Q_OBJECT

    public:
        imgList_t *m_FrameList;
		
        int m_ScrollY;
        int m_ScrollHeight;
		int m_FrameH;
		int m_FrameW;
		int m_NumFrames;

		QVector<int> m_SelFrames;
		
        explicit CStripFramesFrame(QWidget *parent = 0);

		void ProcessInput(void);
		
        ~CStripFramesFrame();
    public slots:
        void Reload(cBaseNode *pLayer);
    protected:
		void paintEvent(QPaintEvent *e);
};

#endif // CSTRIPFRAMESFRAME_H
