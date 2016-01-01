#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cMainManager.h"
#include <QKeyEvent>
#include <util.h>
#include <qglobal.h>
#include <QDebug>

#include <QMap>
QMap<int, quint8> gKeysMap;
QMap<int, quint8> gKeysMap_Old;
QVector2D gMouse; 

cMainManager *gpMainMgr = 0;
MainWindow   *gpMainWND = 0;

int butA		 = Qt::Key_Z;
int butB		 = Qt::Key_X;
int butC		 = Qt::Key_C;

int butStart = Qt::Key_Return;

int butUp    = Qt::Key_Up;
int butDown  = Qt::Key_Down;
int butLeft  = Qt::Key_Left;
int butRight = Qt::Key_Right;

bool g_InputActive = true;

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	gpMainMgr = new cMainManager(this);
	gpMainWND = this;
	//centralWidget()->setAttribute(Qt::WA_TransparentForMouseEvents);
	//centralWidget()->setMouseTracking(true);
	//setMouseTracking(true);
}

void MainWindow::resizeEvent(QResizeEvent *re)
{
	MGR->resize(re->size().width(),re->size().height());
}

void MainWindow::keyReleaseEvent(QKeyEvent *e)
{
	if (!g_InputActive) return;
	
	gKeysMap[e->key()] |= KEY_ISUP;
	gKeysMap[e->key()] &= ~KEY_ISDOWN;
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
	if (e->key() == Qt::Key_Escape)
	{
		this->close();
	}
	
	if (!g_InputActive) return;
	
	//qDebug()<<"scancode"<<e->nativeScanCode();
	//qDebug()<<"VirtualKey"<<e->nativeVirtualKey();
	
	gKeysMap[e->key()] |= KEY_ISDOWN;
	gKeysMap[e->key()] &= ~KEY_ISUP;
}
MainWindow::~MainWindow()
{
	if (gpMainMgr) delete gpMainMgr;
	delete ui;
}
