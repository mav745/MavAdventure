#include <util.h>
#include "cstripframesframe.h"
#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <cbasenode.h>

#include <QDir>
#include <QPainter>
#include <QResizeEvent>

#include <QDebug>

extern QPoint g_MousePos;
extern QPoint g_MouseOfs, g_MousePrev;
extern int g_MouseWheel;

CStripFramesFrame::CStripFramesFrame(QWidget *parent) : QFrame(parent)
{
	//tempovo
	m_FrameH = 64;
	m_FrameW = 64;
	m_NumFrames = 16;
	
	
    m_ScrollY = 0;
    m_ScrollHeight = m_FrameW*m_NumFrames+FRAME_SPACING*m_NumFrames+TOP_SPACING;
    m_FrameList = new imgList_t;

    QImage Strip(QDir::currentPath()+"/res/sprites/s_mav_run.png");


    for(int i=0; i<m_NumFrames; i++)
    {
        m_FrameList->push_back( new QImage( Strip.copy(i*m_FrameW, 0, m_FrameW, m_FrameH) ) );
	}
}

void CStripFramesFrame::ProcessInput()
{
	if (!hasFocus()) return;
	QPoint mouse_local = g_MousePos - pos();
	
	float spos = MainWindow::GetUi()->vscrlStripFrames->sliderPosition();
	float smax = MainWindow::GetUi()->vscrlStripFrames->maximum();
	if (g_MouseWheel > 0) spos -= smax/8.f;
	if (g_MouseWheel < 0) spos += smax/8.f;
	MainWindow::GetUi()->vscrlStripFrames->setSliderPosition(spos);
	
	if (MainWindow::KeyState(Qt::LeftButton) & KS_RELEASED) //PRESS should drag'n'drop
	{
        int sel_pos = (mouse_local.y()+spos-TOP_SPACING)/(m_FrameH+FRAME_SPACING);
		if (MainWindow::s_bCtrl)
		{
			//invert item selection
			int index = m_SelFrames.indexOf(sel_pos);
			if (index != -1) m_SelFrames.remove(index);
			else m_SelFrames << sel_pos;
		}
		else
		{
			if (MainWindow::s_bShift)
			{
				if (m_SelFrames.size() >= 1) 
				{
					//select range
					int first = m_SelFrames[0];
					int first_real = first;
					m_SelFrames.clear();
					if (first > sel_pos)
					{
						int m = sel_pos;
						sel_pos = first;
						first = m;
					}
					for(int i=first;i<=sel_pos;i++)
						m_SelFrames << i;
					m_SelFrames.remove(m_SelFrames.indexOf(first_real));
					m_SelFrames.push_front(first_real);
				}
				else goto simpleSelect;
			}
			else
			{
simpleSelect:
				m_SelFrames.clear();
				m_SelFrames << sel_pos;
			}
		}
	}
}

CStripFramesFrame::~CStripFramesFrame()
{
	//TODO: remove later
    for(int i=0; i<m_NumFrames; i++)
    {
        delete m_FrameList->at(i);
    }
	delete m_FrameList;
}

void CStripFramesFrame::Reload(cBaseNode *pLayer)
{
	
}

void CStripFramesFrame::paintEvent(QPaintEvent *e)
{
    //
    MainWindow::GetUi()->vscrlStripFrames->setMinimum(0);
    MainWindow::GetUi()->vscrlStripFrames->setMaximum((m_ScrollHeight - height()));
    //

    m_ScrollY = MainWindow::GetUi()->vscrlStripFrames->sliderPosition();

	
    int CenterX = width()/2; // center of field
    int Left = CenterX - m_FrameW/2; // left spacing to draw frame

    QPainter p(this);
    p.translate(0, -m_ScrollY);
    for(int i = 0; i<m_NumFrames; i++)
    {
        p.drawImage(Left, TOP_SPACING+i*(m_FrameH+16), *m_FrameList->at(i));
		if (m_SelFrames.indexOf(i) != -1)
			p.setPen(QColor(255,0,0));
		else
			p.setPen(QColor(0,0,0));
        p.drawRect(Left-1, TOP_SPACING+i*(m_FrameH+FRAME_SPACING)-1, m_FrameW+2, m_FrameH+2);
        p.drawText(CenterX-16, TOP_SPACING+i*(m_FrameH+FRAME_SPACING)+m_FrameH, 32, 14, Qt::AlignTop|Qt::AlignCenter, QString::number(i));
    }

    QFrame::paintEvent(e);
}
