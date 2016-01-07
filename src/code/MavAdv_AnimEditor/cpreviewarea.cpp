#include "cpreviewframe.h"
#include <mainwindow.h>
#include <ui_mainwindow.h>

#include <QDir>
#include <QPainter>
#include <QResizeEvent>

#include <QDebug>

extern QPoint g_MousePos;
extern QPoint g_MouseOfs, g_MousePrev;
extern int g_MouseWheel;

CPreviewFrame::CPreviewFrame(QWidget *parent) : QFrame(parent)
{
	m_Init = false;
    m_Scroll = QPoint(0, 0);
    m_Bkg = QImage(QDir::currentPath()+"/res/editor/chess_grid.png");
    m_Scale = 1;
}


void CPreviewFrame::resizeEvent(QResizeEvent *e)
{
	if (!m_Init)
	{
		m_Scroll = -QPoint(width()>>1,height()>>1);
		m_Init = true;
	}
}

void CPreviewFrame::ProcessInput(void)
{
    if(hasFocus())
    {
        if (MainWindow::KeyState(Qt::Key_Space) & KS_DOWN)
        {
            if (MainWindow::KeyState(Qt::LeftButton) & KS_DOWN)
            {
                m_Scroll -= g_MouseOfs;
            }
        }
        else
        if(MainWindow::KeyState(Qt::MiddleButton) & KS_DOWN)
        {
            m_Scroll -= g_MouseOfs;
        }

        if(g_MouseWheel!=0)
        {
            int scl_prev = m_Scale;
            if(g_MouseWheel>0 && m_Scale<8) m_Scale+=1;
            if(g_MouseWheel<0 && m_Scale>1) m_Scale-=1;

            if(scl_prev != m_Scale)
            {
                QPoint mouse = g_MousePos - pos();
                m_Scroll = (mouse + m_Scroll) * m_Scale / scl_prev;
                m_Scroll -= mouse;
            }
        }

        if (MainWindow::KeyState(Qt::Key_Control) & KS_DOWN)
        {
            if (MainWindow::KeyState(Qt::RightButton) & KS_DOWN)
            {
                m_Scroll = -QPoint(width()>>1,height()>>1);
            }
        }
    }
}

void CPreviewFrame::paintEvent(QPaintEvent *e)
{
    QBrush Brush(m_Bkg);
    QPainter p(this);
    p.fillRect(rect(), Brush);
    p.translate(-m_Scroll.x(), -m_Scroll.y());

    p.setPen(QColor(255, 243, 67));
    p.drawLine(-32, 0, 32, 0);
    p.drawLine(0, -32, 0, 32);
    p.scale(m_Scale, m_Scale);


    p.setPen(QColor(0, 0, 0));
    // рисуем
    p.drawLine(-32,-32,128,128);

    QFrame::paintEvent(e);
}
