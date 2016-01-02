#include <ui_mainwindow.h>
#include <mainwindow.h>

#include "cBaseToolButton.h"
#include <cGLManager.h>
#include <toolbuttons.h>

#include <QVector>
#include <QFrame>

#include <QTimer>

extern QTimer gStarter;

extern QVector<cBaseToolButton *>g_ToolButs;

cBaseToolButton::cBaseToolButton(QWidget *parent) :
	QPushButton(parent)
{
	m_pTool = NULL;
	m_PageIdx = 0;
	//m_ToolPanel = NULL;
	connect(this,SIGNAL(clicked(bool)),this,SLOT(IAmClicked(bool)));
	connect(&gStarter,SIGNAL(timeout()),this,SLOT(AssignMe()));
	g_ToolButs.push_back(this);
}
void cBaseToolButton::AssignMe()
{
}

void cBaseToolButton::HidePanel()
{
	gpMainWND->ui->stackedWidget->setCurrentIndex(0);
	//if (m_ToolPanel) m_ToolPanel->hide();
}
void cBaseToolButton::ShowPanel()
{
	gpMainWND->ui->stackedWidget->setCurrentIndex(m_PageIdx);
	//if (m_ToolPanel) m_ToolPanel->show();
}
void cBaseToolButton::UncheckOthers()
{
	for(int i=g_ToolButs.size()-1;i>=0;i--)
	{
		if (g_ToolButs[i] == this) continue;
		g_ToolButs[i]->setChecked(false);
		g_ToolButs[i]->HidePanel();
	}
}
void cBaseToolButton::IAmClicked(bool check)
{
	if (check)
	{
		UncheckOthers();
		g_CurrentTool = m_pTool;
		ShowPanel();
	}
	else
	{
		g_CurrentTool = NULL;
		HidePanel();
	}
	MGR->setFocus();
}