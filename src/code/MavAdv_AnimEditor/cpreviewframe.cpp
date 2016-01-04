#include "cpreviewframe.h"
#include <mainwindow.h>
#include <ui_mainwindow.h>

#include <QDir>
#include <QPainter>
#include <QResizeEvent>

#include <QDebug>

CPreviewFrame::CPreviewFrame(QWidget *parent) : QFrame(parent)
{
    m_Scroll = QPoint(0, 0);
    m_Bkg = QImage(QDir::currentPath()+"/res/editor/chess_grid.png");
}


void CPreviewFrame::resizeEvent(QResizeEvent *e)
{

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
    p.scale(2,2);


    p.setPen(QColor(0, 0, 0));
    // рисуем
    p.drawLine(-32,-32,128,128);

    QFrame::paintEvent(e);
}
