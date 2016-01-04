#include <util.h>
#include "cstripframesframe.h"
#include <mainwindow.h>
#include <ui_mainwindow.h>

#include <QDir>
#include <QPainter>
#include <QResizeEvent>

#include <QDebug>

extern QPoint g_MousePos;
extern QPoint g_MouseOfs, g_MousePrev;
extern int g_MouseWheel;

CStripFramesFrame::CStripFramesFrame(QWidget *parent) : QFrame(parent)
{
    m_ScrollY = 0;
    m_ScrollHeight = 64*16+16*16+8;
    m_FrameList = new imgList_t;

    QImage Strip(QDir::currentPath()+"/res/sprites/s_mav_run.png");
    for(int i=0; i<16; i++)
    {
        m_FrameList->push_back( new QImage( Strip.copy(i*64, 0, 64, 64) ) );
    }
}

CStripFramesFrame::~CStripFramesFrame()
{
    for(int i=0; i<16; i++)
    {
        delete m_FrameList->at(i);
    }
    delete m_FrameList;
}

void CStripFramesFrame::paintEvent(QPaintEvent *e)
{
    //
    MainWindow::GetUi()->vscrlStripFrames->setMinimum(0);
    MainWindow::GetUi()->vscrlStripFrames->setMaximum(m_ScrollHeight - height());
    //

    m_ScrollY = MainWindow::GetUi()->vscrlStripFrames->sliderPosition();

    int sx = 64;
    int sy = 64;
    int cx = width()/2;
    int cy = 8;
    int x = cx - sx/2;

    QPainter p(this);
    p.translate(0, -m_ScrollY);
    for(int i = 0; i<m_FrameList->size(); i++)
    {
        p.drawImage(x, cy+i*(sy+16), *m_FrameList->at(i));
        p.drawRect(x-1, cy+i*(sy+16)-1, sx+2, sy+2);
        p.drawText(cx-16, cy+i*(sy+16)+64, 32, 14, Qt::AlignTop|Qt::AlignCenter, QString::number(i));
        //XTJGHVJGLJKG

    }

    QFrame::paintEvent(e);
}
