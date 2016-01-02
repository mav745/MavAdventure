#include "cBacksTool.h"

#include <cGLManager.h>

#include <QFileDialog>
#include <QInputDialog>
#include <mainwindow.h>
#include <ui_mainwindow.h>

#include <cBackground.h>

#include <QDebug>
#include <keys.h>

cBacksTool *g_pBacksTool;
extern QVector2D gMouseMove;

cBacksTool::cBacksTool(QObject *parent) :
	cBaseTool(parent)
{
	g_pBacksTool = this;
	
	connect(gpMainWND->ui->chk_HRep,SIGNAL(toggled(bool)),this,SLOT(setHRep(bool)));
	connect(gpMainWND->ui->chk_VRep,SIGNAL(toggled(bool)),this,SLOT(setVRep(bool)));
	
	connect(gpMainWND->ui->but_backVis,SIGNAL(clicked()),this,SLOT(visibility()));
	
	connect(gpMainWND->ui->but_backScrFac,SIGNAL(clicked()),
			  this,SLOT(scrFac()));
	connect(gpMainWND->ui->but_backScrSpd,SIGNAL(clicked()),
			  this,SLOT(scrSpd()));
	connect(gpMainWND->ui->but_backOrig,SIGNAL(clicked()),
			  this,SLOT(orig()));
			
	connect(gpMainWND->ui->but_changeBackDepth,SIGNAL(clicked()),
			  this,SLOT(changeBackDepth()));
	connect(gpMainWND->ui->but_changeBack,SIGNAL(clicked()),
			  this,SLOT(changeBack()));
	connect(gpMainWND->ui->but_AddBack,SIGNAL(clicked()),this,SLOT(addBack()));
	connect(gpMainWND->ui->but_RemoveBack,SIGNAL(clicked()),this,SLOT(removeBack()));
	connect(gpMainWND->ui->list_Backs,SIGNAL(currentTextChanged(QString)),
			  this,SLOT(backSelected(QString)));
	m_CurBackIdx = -1;
	//gpMainWND->ui->list_Backs->se
	//gpMainWND->ui->list_Backs->addItem("Long/ background/ name/ that/ doesn't/ really/ fit");
}
void cBacksTool::DrawMe()
{
	if (m_CurBackIdx != -1)
	{
		gpMainWND->ui->text_BackDepth->setText(
					QString().number(m_Backs[m_CurBackIdx]->m_Depth));
		gpMainWND->ui->text_BackScrFacX->setText(
					QString().number(m_Backs[m_CurBackIdx]->m_ScrFac.x()));
		gpMainWND->ui->text_BackScrFacY->setText(
					QString().number(m_Backs[m_CurBackIdx]->m_ScrFac.y()));
		gpMainWND->ui->text_BackScrSpdX->setText(
					QString().number(m_Backs[m_CurBackIdx]->m_ScrSpd.x()));
		gpMainWND->ui->text_BackScrSpdY->setText(
					QString().number(m_Backs[m_CurBackIdx]->m_ScrSpd.y()));
		gpMainWND->ui->text_BackOrigX->setText(
					QString().number(m_Backs[m_CurBackIdx]->m_Origin.x()));
		gpMainWND->ui->text_BackOrigY->setText(
					QString().number(m_Backs[m_CurBackIdx]->m_Origin.y()));
		gpMainWND->ui->chk_HRep->setChecked(m_Backs[m_CurBackIdx]->m_HRep);
		gpMainWND->ui->chk_VRep->setChecked(m_Backs[m_CurBackIdx]->m_VRep);
	}
	else
	{
		gpMainWND->ui->text_BackDepth->clear();
		gpMainWND->ui->text_BackScrFacX->clear();
		gpMainWND->ui->text_BackScrFacY->clear();
		gpMainWND->ui->text_BackScrSpdX->clear();
		gpMainWND->ui->text_BackScrSpdY->clear();
		gpMainWND->ui->text_BackOrigX->clear();
		gpMainWND->ui->text_BackOrigY->clear();
	}
}
void cBacksTool::ClearALL()
{
	while(m_Backs.size())
	{
		cBackground *pBack = m_Backs[0];
		m_Backs.removeAt(0);
		
		REMOVE_OBJECT(pBack);
	}
	
	UpdateBacksList();
}

void cBacksTool::ProcessInput()
{
	bool shift_down   = !!(MGR->KeyState(Qt::Key_Shift  ) & KEY_ISDOWN);
	bool control_down = !!(MGR->KeyState(Qt::Key_Control) & KEY_ISDOWN);
	bool alt_down     = !!(MGR->KeyState(Qt::Key_Alt    ) & KEY_ISDOWN);
	bool space_down   = !!(MGR->KeyState(Qt::Key_Space  ) & KEY_ISDOWN);
	QCursor cur;
	cur.setShape(Qt::ArrowCursor);
	
	if ((shift_down && !control_down && !alt_down && !space_down)
	&& (MGR->KeyState(Qt::LeftButton) & KEY_ISDOWN))
	{
		cur.setShape(Qt::SizeAllCursor);
		m_MouseAccum += gMouseMove;
		for(int i=0;i<2;i++)
		{
			if (fabs(m_MouseAccum[i]) > MGR->GridCellSize())
			{
				if (m_CurBackIdx != -1)
				{
					int cells = m_MouseAccum[i]/MGR->GridCellSize();
					cells *= MGR->GridCellSize();
					m_MouseAccum[i] -= cells;
					m_Backs[m_CurBackIdx]->m_Origin[i] += cells;
				}
			}
		}
//		if (m_CurBackIdx != -1)
//		{
//			if (m_Backs[m_CurBackIdx]->m_Origin[0] >= 
//				 m_Backs[m_CurBackIdx]->m_Width)
//					m_Backs[m_CurBackIdx]->m_Origin[0] -= 2*m_Backs[m_CurBackIdx]->m_Width;
//			if (m_Backs[m_CurBackIdx]->m_Origin[0] <=
//				 -m_Backs[m_CurBackIdx]->m_Width)
//					m_Backs[m_CurBackIdx]->m_Origin[0] += 2*m_Backs[m_CurBackIdx]->m_Width;
			
//			if (m_Backs[m_CurBackIdx]->m_Origin[1] >= 
//				 m_Backs[m_CurBackIdx]->m_Height)
//					m_Backs[m_CurBackIdx]->m_Origin[1] -= 2*m_Backs[m_CurBackIdx]->m_Height;
//			if (m_Backs[m_CurBackIdx]->m_Origin[1] <=
//				 -m_Backs[m_CurBackIdx]->m_Height)
//					m_Backs[m_CurBackIdx]->m_Origin[1] += 2*m_Backs[m_CurBackIdx]->m_Height;
//		}
	}
	else
	{
		m_MouseAccum = QVector2D(0,0);
	}
	MGR->setCursor(cur);
}

void cBacksTool::UpdateBacksList()
{
	gpMainWND->ui->list_Backs->clear();
	int sz = m_Backs.size();
	for(int i=0;i<sz;i++)
	{
		gpMainWND->ui->list_Backs->addItem(m_Backs[i]->m_Filename);
	}
	if (sz > 0)
		m_CurBackIdx = 0;
	else
		m_CurBackIdx = -1;
}
cBackground * cBacksTool::AddBack(QString fname, int depth)
{
	cBackground *pBack = (cBackground *)GET_OBJECT(background);
	
	pBack->AssignBackground(fname);
	
	SET_OBJECT_DEPTH(pBack,depth);
	
	m_Backs.push_back(pBack);
	
	qSort(m_Backs.begin(),m_Backs.end(),depthCompare);
	
	UpdateBacksList();
	
	return pBack;
}

void cBacksTool::addBack()
{
	bool ok;
	int depth = QInputDialog::getInt(
				gpMainWND,
				"New Background",
				"New Background",
				100000,
				-2147483647,
				2147483647,
				1, &ok);
	if (ok)
	{
		QString fname = QFileDialog::getOpenFileName(gpMainWND,"Open Background",QDir::currentPath(),
											  tr("PNG (*.png)"));
		if (!fname.isEmpty())
		{
			AddBack(fname,depth);
		}
	}
	MGR->setFocus();
}

void cBacksTool::removeBack()
{
	if (m_CurBackIdx == -1)
		return;
	
	cBackground *pBack = m_Backs[m_CurBackIdx];
	m_Backs.removeAt(m_CurBackIdx);
	
	REMOVE_OBJECT(pBack);
	
	UpdateBacksList();
}

void cBacksTool::changeBack()
{
	if (m_CurBackIdx == -1) return;
	
	QString fname = QFileDialog::getOpenFileName(gpMainWND,"Open Background",QDir::currentPath(),
										  tr("PNG (*.png)"));
	if (!fname.isEmpty())
	{
		cBackground *pBack = m_Backs[m_CurBackIdx];
		
		pBack->AssignBackground(fname);
		
		UpdateBacksList();
	}
}

void cBacksTool::changeBackDepth()
{
	if (m_CurBackIdx == -1) return;
	
	bool ok;
	int depth = QInputDialog::getInt(
				gpMainWND,
				"Change Depth",
				"Change Depth",
				m_Backs[m_CurBackIdx]->m_Depth,
				-2147483647,
				2147483647,
				1, &ok);
	if (ok)
	{
		SET_OBJECT_DEPTH(m_Backs[m_CurBackIdx],depth);
		qSort(m_Backs.begin(),m_Backs.end(),depthCompare);
		UpdateBacksList();
	}
}

void cBacksTool::visibility()
{
	if (m_CurBackIdx == -1)
		return;
	
	m_Backs[m_CurBackIdx]->m_Visible = !m_Backs[m_CurBackIdx]->m_Visible;
}

void cBacksTool::scrFac()
{
	if (m_CurBackIdx == -1)
		return;
	
	bool ok;
	float scrx = QInputDialog::getDouble(
				gpMainWND,
				"Scroll Factor X",
				"Scroll Factor X",
				m_Backs[m_CurBackIdx]->m_ScrFac.x(),
				-2147483647,
				2147483647,
				2, &ok);
	if (ok)
	{
		float scry = QInputDialog::getDouble(
					gpMainWND,
					"Scroll Factor Y",
					"Scroll Factor Y",
					m_Backs[m_CurBackIdx]->m_ScrFac.y(),
					-2147483647,
					2147483647,
					2, &ok);
		if (ok)
		{
			m_Backs[m_CurBackIdx]->m_ScrFac = QVector2D(scrx,scry);
		}
	}
}

void cBacksTool::scrSpd()
{
	if (m_CurBackIdx == -1)
		return;
	
	bool ok;
	float scrx = QInputDialog::getDouble(
				gpMainWND,
				"Scroll Speed X",
				"Scroll Speed X",
				m_Backs[m_CurBackIdx]->m_ScrSpd.x(),
				-2147483647,
				2147483647,
				2, &ok);
	if (ok)
	{
		float scry = QInputDialog::getDouble(
					gpMainWND,
					"Scroll Speed Y",
					"Scroll Speed Y",
					m_Backs[m_CurBackIdx]->m_ScrSpd.y(),
					-2147483647,
					2147483647,
					2, &ok);
		if (ok)
		{
			m_Backs[m_CurBackIdx]->m_ScrSpd = QVector2D(scrx,scry);
		}
	}
}

void cBacksTool::orig()
{
	if (m_CurBackIdx == -1)
		return;
	
	bool ok;
	int origx = QInputDialog::getInt(
				gpMainWND,
				"Origin X",
				"Origin X",
				m_Backs[m_CurBackIdx]->m_Origin.x(),
				-2147483647,
				2147483647,
				1, &ok);
	if (ok)
	{
		int origy = QInputDialog::getInt(
					gpMainWND,
					"Origin Y",
					"Origin Y",
					m_Backs[m_CurBackIdx]->m_Origin.y(),
					-2147483647,
					2147483647,
					1, &ok);
		if (ok)
		{
			m_Backs[m_CurBackIdx]->m_Origin = QVector2D(origx,origy);
		}
	}
}

void cBacksTool::setHRep(bool state)
{
	if (m_CurBackIdx == -1)
		return;
	
	m_Backs[m_CurBackIdx]->m_HRep = state;
}

void cBacksTool::setVRep(bool state)
{
	if (m_CurBackIdx == -1)
		return;
	
	m_Backs[m_CurBackIdx]->m_VRep = state;
}

void cBacksTool::backSelected(QString name)
{
	for(int i=m_Backs.size()-1;i>=0;i--)
	{
		if (m_Backs[i]->m_Filename == name)
		{
			m_CurBackIdx = i;
			return;
		}
	}
}
