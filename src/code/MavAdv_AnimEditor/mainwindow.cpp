#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <qevent.h>

#include <QTimer>

MainWindow * MainWindow::s_WND = NULL;

QMap <int, int> g_KeysMap, g_KeyMapPrev;
QPoint g_MouseOfs, g_MousePrev;

QTimer *g_Timer;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	s_WND = this;
	g_Timer = new QTimer;
	connect(g_Timer,SIGNAL(timeout()),this,SLOT(update()));
	g_Timer->setSingleShot(false);
	g_Timer->start(1000/60);

}

void MainWindow::ProcessInput(void)
{
    if (ui->fieldAnimPreview->hasFocus())
    {
        if (MainWindow::KeyState(Qt::Key_Space) & KS_DOWN)
        {
            if (MainWindow::KeyState(Qt::LeftButton) & KS_DOWN)
            {
                ui->fieldAnimPreview->m_Scroll -= g_MouseOfs;
            }
        }
    }
	
	//TODO
	
}


//==============================================================
void MainWindow::paintEvent(QPaintEvent *e)
{
	QMap <int, int>::iterator it;
	for(it = g_KeysMap.begin();it != g_KeysMap.end();it++)
	{
		it.value() &= ~(KS_PRESSED && KS_RELEASED);
		
		if (!g_KeyMapPrev.contains(it.key())) continue;
		int value_prev = g_KeyMapPrev[it.key()];
		
		if ( (it.value() & KS_DOWN) && !(value_prev & KS_DOWN))
			it.value() |= KS_PRESSED;
		
		if (!(it.value() & KS_DOWN) &&  (value_prev & KS_DOWN))
			it.value() |= KS_RELEASED;
	}
	
	ProcessInput();
	
	g_MouseOfs = QPoint(0,0);
	g_KeyMapPrev = g_KeysMap;
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
	//TODO
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
	if (g_MousePrev != QPoint(0,0))
		g_MouseOfs += e->pos() - g_MousePrev;
	
	g_MousePrev = e->pos();
}

void MainWindow::mousePressEvent(QMouseEvent *e)
{
	g_MouseOfs = QPoint(0,0);
	g_MousePrev = e->pos();
	g_KeysMap[e->button()] |=  KS_DOWN;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *e)
{
	g_KeysMap[e->button()]  &= ~KS_DOWN;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
	g_KeysMap[e->key()] |=  KS_DOWN;
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
	g_KeysMap[e->key()] &= ~KS_DOWN;
}

MainWindow::~MainWindow()
{
	delete ui;
}

Ui::MainWindow *MainWindow::GetUi()
{
	return s_WND->ui;
}

int MainWindow::KeyState(int key)
{
	if (!g_KeysMap.contains(key)) return KS_UP;
	return g_KeysMap[key];
}
