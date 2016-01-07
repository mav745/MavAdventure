#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cframegrid.h"
#include <QFileDialog>
#include <cstriplayer.h>
#include <qevent.h>

#include <QTimer>

#include <QDebug>

MainWindow * MainWindow::s_WND = NULL;
bool MainWindow::s_bShift = false;
bool MainWindow::s_bCtrl = false;
bool MainWindow::s_bAlt = false;
bool MainWindow::s_bSpace = false;

QMap <int, int> g_KeysMap, g_KeyMapPrev;
QPoint g_MousePos;
QPoint g_MouseOfs, g_MousePrev;
int g_MouseWheel;

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
	
	ui->stripFrameList->setGridSize(QSize(171,64));
	ui->stripFrameList->setIconSize(QSize(171,64));
	
	QPixmap Strip(QDir::currentPath()+"/res/sprites/s_mav_run.png");
	
	for(int i=0; i<16; i++)
	{
		QListWidgetItem *item1 = new QListWidgetItem;
		QPixmap *pImg = new QPixmap( Strip.copy(i*64, 0, 64, 64) );
		item1->setIcon(QIcon(*pImg));
		item1->setText(QString::number(i));
		item1->setData(3,(int)pImg);
		ui->stripFrameList->addItem(item1);
	}
	
	
//	QListWidgetItem *item1 = new QListWidgetItem;
//	QIcon *pIco = new QIcon(QDir::currentPath()+"/res/editor/anim_edit/folder_icon.png");
//	item1->setData(0,(int)pIco);
//	qDebug()<<"pIco"<<pIco;

//	ui->stripFrameList->addItem(item1);

//	QListWidgetItem *item2 = new QListWidgetItem;
//	QIcon *pIco2 = new QIcon(QDir::currentPath()+"/res/editor/anim_edit/folder_icon.png");
//	item2->setData(0,(int)pIco2);

//	ui->stripFrameList->addItem(item2);

//	QTreeWidgetItem *item1 = new QTreeWidgetItem;
//	item1->setText(0,"group1");
//	item1->setIcon(0,QIcon(QDir::currentPath()+"/res/editor/anim_edit/folder_icon.png"));
//	item1->setIcon(1,QIcon(QDir::currentPath()+"/res/editor/anim_edit/eye_icon.png"));
//	ui->stripList->addTopLevelItem(item1);

//	QTreeWidgetItem *item2 = new QTreeWidgetItem;
//	item2->setText(0,"new_item");
//	item2->setIcon(0,QIcon(QDir::currentPath()+"/res/editor/anim_edit/strip_icon.png"));
//	item2->setIcon(1,QIcon(QDir::currentPath()+"/res/editor/anim_edit/eye_icon.png"));
//	item1->addChild(item2);

//	QTreeWidgetItem *item3 = new QTreeWidgetItem;
//	item3->setText(0,"group2");
//	item3->setIcon(0,QIcon(QDir::currentPath()+"/res/editor/anim_edit/folder_icon.png"));
//	item3->setIcon(1,QIcon(QDir::currentPath()+"/res/editor/anim_edit/eye_icon.png"));
//	ui->stripList->addTopLevelItem(item3);
}

void MainWindow::ProcessInput(void)
{
	ui->fieldAnimPreview->ProcessInput();
//	ui->fieldStripFrames->ProcessInput();
	//ui->fieldStrips->ProcessInput();
	
	//TODO
}


//==============================================================
void MainWindow::paintEvent(QPaintEvent *e)
{
	QMap <int, int>::iterator it;
	for(it = g_KeysMap.begin(); it != g_KeysMap.end(); it++)
	{
		it.value() &= ~(KS_PRESSED | KS_RELEASED);
		
		if (!g_KeyMapPrev.contains(it.key())) continue;
		int value_prev = g_KeyMapPrev[it.key()];
		
		if ( (it.value() & KS_DOWN) && !(value_prev & KS_DOWN))
			it.value() |= KS_PRESSED;
		
		if (!(it.value() & KS_DOWN) &&  (value_prev & KS_DOWN))
		{
			it.value() |= KS_RELEASED;
		}
	}
	
	s_bShift = KeyState(Qt::Key_Shift) & KS_DOWN;
	s_bCtrl = KeyState(Qt::Key_Control) & KS_DOWN;
	s_bAlt = (KeyState(Qt::Key_Alt) & KS_DOWN) | (KeyState(Qt::Key_AltGr) & KS_DOWN);
	s_bSpace = KeyState(Qt::Key_Space) & KS_DOWN;
	
	ProcessInput();
	
	g_MouseWheel = 0;
	g_MouseOfs = QPoint(0,0);
	g_KeyMapPrev = g_KeysMap;
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
	g_MouseWheel = e->angleDelta().y();
}

void MainWindow::mouseMoveEvent(QMouseEvent *e)
{
	if (g_MousePrev != QPoint(0,0))
		g_MouseOfs += e->pos() - g_MousePrev;
	
	g_MousePos = e->pos();
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

void MainWindow::on_b_StripAdd_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this,
													tr("Open Image"), QDir::currentPath()+"/", tr("Image Files (*.png)"));
	if (fileName.isNull()) return;
	
	cStripLayer *pNewStrip = cStripLayer::GetNewStrip(fileName);
	if (pNewStrip)
	{
		pNewStrip->setText(0,pNewStrip->m_Name);
		pNewStrip->setIcon(0,QIcon(QDir::currentPath()+"/res/editor/anim_edit/strip_icon.png"));
		pNewStrip->setIcon(1,QIcon(QDir::currentPath()+"/res/editor/anim_edit/eye_icon.png"));
		ui->stripList->addTopLevelItem(pNewStrip);
	}
}

void MainWindow::on_b_StripGroup_clicked()
{
	QList<QTreeWidgetItem*> selected = ui->stripList->selectedItems();
	
	//selected[i]->parent();
	
	//ui->stripList->indexOfTopLevelItem()
}
