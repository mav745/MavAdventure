#include <cGLManager.h>
#include <toolbuttons.h>
#include <QVector>

#include <QDebug>

#include <mainwindow.h>
#include <ui_mainwindow.h>



QVector<cBaseToolButton *>g_ToolButs;

cLineToolButton::cLineToolButton        (QWidget *parent) : cBaseToolButton(parent){}
cTileToolButton::cTileToolButton        (QWidget *parent) : cBaseToolButton(parent){}
cBacksToolButton::cBacksToolButton      (QWidget *parent) : cBaseToolButton(parent){}
cTriggsToolButton::cTriggsToolButton    (QWidget *parent) : cBaseToolButton(parent){}
cEntitiesToolButton::cEntitiesToolButton(QWidget *parent) : cBaseToolButton(parent){}
cRoomToolButton::cRoomToolButton        (QWidget *parent) : cBaseToolButton(parent){}

void cLineToolButton::AssignMe()
{
	m_pTool = new cLineTool;
	m_pTool->m_Name = "lines";
	m_PageIdx = gpMainWND->ui->stackedWidget->indexOf(gpMainWND->ui->lineToolPage);
	//m_ToolPanel = gpMainWND->ui->lineToolFrame;
	//m_ToolPanel->setGeometry(gpMainWND->GetToolFrameRefRect());
	HidePanel();
}
void cTileToolButton::AssignMe()
{
	m_pTool = new cTileTool;
	m_pTool->m_Name = "tiles";
	m_PageIdx = gpMainWND->ui->stackedWidget->indexOf(gpMainWND->ui->tileToolPage);
//	m_ToolPanel = gpMainWND->ui->tileToolFrame;
//	m_ToolPanel->setGeometry(gpMainWND->GetToolFrameRefRect());
	HidePanel();
}
void cBacksToolButton::AssignMe()
{
	m_pTool = new cBacksTool;
	m_pTool->m_Name = "backs";
	m_PageIdx = gpMainWND->ui->stackedWidget->indexOf(gpMainWND->ui->backsToolPage);
//	m_ToolPanel = gpMainWND->ui->backsToolFrame;
//	m_ToolPanel->setGeometry(gpMainWND->GetToolFrameRefRect());
	HidePanel();
}
void cTriggsToolButton::AssignMe()
{
	m_pTool = new cTriggsTool;
	m_pTool->m_Name = "triggs";
	m_PageIdx = gpMainWND->ui->stackedWidget->indexOf(gpMainWND->ui->triggsToolPage);
//	m_ToolPanel = gpMainWND->ui->triggsToolFrame;
//	m_ToolPanel->setGeometry(gpMainWND->GetToolFrameRefRect());
	HidePanel();
}
void cEntitiesToolButton::AssignMe()
{
	m_pTool = new cEntityTool;
	m_pTool->m_Name = "ents";
	m_PageIdx = gpMainWND->ui->stackedWidget->indexOf(gpMainWND->ui->entityToolPage);
//	m_ToolPanel = gpMainWND->ui->entityToolFrame;
//	m_ToolPanel->setGeometry(gpMainWND->GetToolFrameRefRect());
	HidePanel();
}
void cRoomToolButton::AssignMe()
{
	m_pTool = new cBaseTool;
	m_pTool->m_Name = "room";
	m_PageIdx = gpMainWND->ui->stackedWidget->indexOf(gpMainWND->ui->roomSetPage);
//	m_ToolPanel = gpMainWND->ui->roomSetFrame;
//	m_ToolPanel->setGeometry(gpMainWND->GetToolFrameRefRect());
	HidePanel();
}
