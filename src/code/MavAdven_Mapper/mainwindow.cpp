#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <cGLManager.h>
#include <cSegment.h>

#include <QVector2D>
#include <cBase.h>

#include <QDebug>
#include <cAxialExpand.h>

#include <QTimer>

#include <cBacksTool.h>
#include <cLineTool.h>
#include <cTileTool.h>
#include <cBackground.h>
#include <cTileLayer.h>

#include <cdlgentprops.h>


MainWindow *gpMainWND;
cGLManager *gpMainMgr;
QVector2D gMouse;
QTimer gStarter;

bool g_ScrollEf = true;

cDlgEntProps *g_pProps;

extern QString g_lastSaved;

qint64 qPointToLongLong(const QPoint &p)
{
	qint64 v = p.x();
	v <<= 32;
	v += p.y();
	return v;
}

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	g_pProps = new cDlgEntProps(this);
	
	ui->setupUi(this);
	gpMainWND = this;
	gpMainMgr = new cGLManager(this);

	//setMinimumHeight(618);
	//setMaximumHeight(618);
	gStarter.setSingleShot(true);
	gStarter.start(10);
	setWindowTitle("new unsaved level...");
	//setGeometry(x(),y(),width(),600);
	//ui->centralWidget->setGeometry(x(),y(),width(),600);
}

MainWindow::~MainWindow()
{
	if (gpMainMgr) delete gpMainMgr;
	delete ui;
}

void MainWindow::on_butLines_toggled(bool checked)
{
	
}

void MainWindow::on_butExpand_clicked() //compile
{

//	QVector<cBase *>objs;
//	OBJECTS_BY_CLASSNAME(par_expand,objs);
//	for(int i=objs.size()-1;i>=0;i--) REMOVE_OBJECT(objs[i]);
//	OBJECTS_BY_CLASSNAME(axial_expand,objs);
//	for(int i=objs.size()-1;i>=0;i--) REMOVE_OBJECT(objs[i]);
	
//	QMap<qint64,cSegment *>tSegs;
//	cSegment *seg;
//	for(int i=g_pLineTool->m_Lines.size()-1;i>=0;i--)
//	{
//		seg = (cSegment *)g_pLineTool->m_Lines[i];
//		seg->m_Neighs.clear();
//		seg->ExpandMe(QVector2D(16,32));
//		if (seg->m_JumpDown)
//		{
//			for(int i=seg->m_Axials.size()-1;i>=0;i--)
//			{
//				if (seg->m_Axials[i]->m_N.x() != 0.f)
//					seg->m_Axials[i]->m_ToRemove = true;
//			}
//			for(int i=seg->m_Exps.size()-1;i>=0;i--)
//			{
//				if (seg->m_Exps[i]->m_N.y() >= 0.f)
//					seg->m_Exps[i]->m_ToRemove = true;
//			}
//		}
//		tSegs.insertMulti(qPointToLongLong(seg->m_S),seg);
//		tSegs.insertMulti(qPointToLongLong(seg->m_E),seg);
//	}
//	QVector<cLine *>vecDone;
//	QList<cSegment *>vecNeighs;
//	for(int i=g_pLineTool->m_Lines.size()-1;i>=0;i--)
//	{
//		vecDone.clear();
//		seg = (cSegment *)g_pLineTool->m_Lines[i];
//		vecNeighs  = tSegs.values(qPointToLongLong(seg->m_S));
//		vecNeighs += tSegs.values(qPointToLongLong(seg->m_E));
//		for(int j=vecNeighs.size()-1;j>=0;j--)
//		{
//			if (vecNeighs[j] == seg) continue;
//			if (vecDone.contains(vecNeighs[j])) continue;
			
//			if ((vecNeighs[j]->m_S == seg->m_E) ||
//				 (vecNeighs[j]->m_E == seg->m_S) )
//			{
//				vecDone.push_back(      vecNeighs[j]);
//				seg->m_Neighs.push_back(vecNeighs[j]);
//			}
//		}
//	}
//	cSegment *oseg;
//	cBaseExpanded *ex,*oex;
//	for(int i=g_pLineTool->m_Lines.size()-1;i>=0;i--)
//	{
//		seg = (cSegment *)g_pLineTool->m_Lines[i];
//		if (!seg->m_Neighs.size())
//		{
//			for(int n=seg->m_Exps.size()-1;n>=0;n--)
//			{
//				ex = seg->m_Exps[n];
//				if (dot(ex->m_N,seg->m_N) < 0.f) ex->m_ToRemove = true;
//			}
//		}
//		else for(int j=seg->m_Neighs.size()-1;j>=0;j--)
//		{
//			for(int k=seg->m_Axials.size()-1;k>=0;k--)
//			{
//				if (seg->m_Neighs[j]->ExpCaseHasPoint(seg->m_Axials[k]->m_S))
//				{
//					seg->m_Axials[k]->m_ToRemove = true;
//					continue;
//				}
//				if (seg->m_Neighs[j]->ExpCaseHasPoint(seg->m_Axials[k]->m_E))
//				{
//					seg->m_Axials[k]->m_ToRemove = true;
//					continue;
//				}
//				if (seg->m_Neighs[j]->ExpCaseHasPoint(
//						 (seg->m_Axials[k]->m_S + seg->m_Axials[k]->m_E)*0.5f
//						 ))
//				{
//					seg->m_Axials[k]->m_ToRemove = true;
//					continue;
//				}
//			}
//			oseg = seg->m_Neighs[j];
//			for(int n=seg->m_Exps.size()-1;n>=0;n--)
//			{
//				ex = seg->m_Exps[n];
//				if (dot(ex->m_N,seg->m_N) < 0.f) ex->m_ToRemove = true;
//				for(int m=oseg->m_Exps.size()-1;m>=0;m--)
//				{
//					oex = oseg->m_Exps[m];
//					if (oex->m_ToRemove) continue;
//					if (dot(oex->m_T,ex->m_T) > 0.f && oex->ContaintsLine(ex))
//						ex->m_ToRemove = true;
//				}
//			}
//		}
//		for(int j=seg->m_Exps.size()-1;j>=0;j--)
//		{
//			if (seg->m_Exps[j]->m_ToRemove)
//				REMOVE_OBJECT(seg->m_Exps[j]);
//		}
//	}
	
	MGR->setFocus();
}

QRect MainWindow::GetToolFrameRefRect(void)
{
	return QRect(
				ui->toolFrameRef->x(),
				ui->toolFrameRef->y(),//+ui->menuBar->height(),
				ui->toolFrameRef->width(),
				ui->toolFrameRef->height()
				);
}

void MainWindow::on_but_ScrollEf_clicked()
{
	g_ScrollEf = !g_ScrollEf;
}

void MainWindow::on_pushButton_4_clicked()
{
	if (g_lastSaved.isEmpty()) MGR->SaveAss();
	if (!g_lastSaved.isEmpty())
	{
		QStringList parts = g_lastSaved.split("/",QString::SkipEmptyParts);
		QString cmd = "start "+QDir::currentPath()+"/"+"MavAdventure.exe -map "+parts[parts.size()-1];
		system(cmd.toLatin1());
	}
}
