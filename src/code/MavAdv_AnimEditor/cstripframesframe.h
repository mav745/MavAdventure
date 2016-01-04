#ifndef CSTRIPFRAMESFRAME_H
#define CSTRIPFRAMESFRAME_H

#include <QFrame>

typedef QVector<QImage*> imgList_t;

class CStripFramesFrame : public QFrame
{
    Q_OBJECT

    public:
        imgList_t *m_FrameList;
        int m_ScrollY;
        int m_ScrollHeight;
        explicit CStripFramesFrame(QWidget *parent = 0);

        ~CStripFramesFrame();
    protected:
            void paintEvent(QPaintEvent *e);
};

#endif // CSTRIPFRAMESFRAME_H
