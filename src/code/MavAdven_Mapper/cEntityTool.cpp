#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <cdlgentprops.h>
#include "ui_cdlgentprops.h"
#include "cEntityTool.h"
#include <cGLManager.h>
#include <QFile>
#include <QStringList>
#include <QDebug>
#include <keys.h>
#include <QDir>
#include <QInputDialog>

cEntityTool *g_pEntityTool;
extern bool gIgnoreChange;
extern QVector2D gMouseMove;

cEntityTool::cEntityTool(QObject *parent) : cBaseEntityTool(parent)
{
	g_pEntityTool = this;
	
	m_Action = ETA_NONE;
	m_MouseAccum = QVector2D(0,0);
	
	connect(gpMainWND->ui->but_chEntDepth,SIGNAL(clicked()),this,SLOT(changeDepth()));
	connect(gpMainWND->ui->but_ShiftAllEnts,SIGNAL(clicked()),this,SLOT(ShiftAll()));
	
	QFile triggsFGD("fgds/entities.fgd");
	if (triggsFGD.open(QFile::ReadOnly))
	{
		QString fullText = triggsFGD.readAll();
		triggsFGD.close();
		
		fullText = fullText.remove("\n");
		fullText = fullText.remove("\r");
		fullText = fullText.remove("\t");
		fullText = fullText.remove(" ");
		
		//qDebug()<<fullText;
		QStringList parts = fullText.split("}",QString::SkipEmptyParts);
		
		for(int i=0;i<parts.size();i++)
		{
			//qDebug()<<"step 1 -"<<i;
			QStringList smallparts = parts[i].split("{",QString::SkipEmptyParts);
			QStringList entityheadparts = smallparts[0].split(":",QString::SkipEmptyParts);
			//qDebug()<<"step 2 -"<<i;
			entity_type_s *pEnt;
			
			//qDebug()<<"type_name"<<entityheadparts[0];
			
			if (entityheadparts.size()>1)
			{
				//qDebug()<<"base "<<entityheadparts[1];
				pEnt = new entity_type_s(m_EntsMap[entityheadparts[1]]);
			}
			else
				pEnt = new entity_type_s;
			
			//qDebug()<<"step 3 -"<<i;
			
			pEnt->type_name = entityheadparts[0];
			if (pEnt->type_name[0] == '@')
				pEnt->hidden = true;
			else
				 gpMainWND->ui->list_Entities->addItem(entityheadparts[0]);
						 
			m_EntsMap[entityheadparts[0]] = pEnt;
			
			if (smallparts.size()>1)
			{
				QStringList fieldsparts = smallparts[1].split(";",QString::SkipEmptyParts);
				for(int j=0;j<fieldsparts.size();j++)
				{
					//qDebug()<<"step 5 -"<<i<<"-"<<j;
					QStringList field_and_type = 
							fieldsparts[j].split(":",QString::SkipEmptyParts);
					
					value_s *pv;
					//qDebug()<<"step 6 -"<<i<<"-"<<j;
					QStringList type_and_def = field_and_type[1].split("(",QString::SkipEmptyParts);
					QString type, def = "";
					type = type_and_def[0];
					if (type_and_def.size() > 1)
					{
						def = type_and_def[1];
						def.remove(")");
						def.replace(","," ");
					}
					//qDebug()<<"step 7 -"<<i<<"-"<<j;
					//qDebug()<<"field"<<field_and_type[0]<<"type"<<type<<"def"<<def;
					
					if (!pEnt->fields.contains(field_and_type[0]))
					{
							  if (type == "float"  )  pv = new value_s(0.f);
						else if (type == "vector2")  pv = new value_s(0.f,0.f);
						else if (type == "color"  )  pv = new value_s(0.f,0.f,0.f);
						else if (type == "string" )  pv = new value_s(QString());
						else if (type == "file"   )  pv = new value_s(QByteArray());
						else if (type == "bool"   )  pv = new value_s(false);
						else if (type == "stringlist")
						{
							QList<QString> parts;
							QList<QString> items;
							parts = def.split(' ',QString::SkipEmptyParts);
							for(int i=0;i<parts.size();i++)
								items.push_back(parts[i]);
							pv = new value_s(items);
						}
						else if (type == "floatlist")
						{
							QList<QString> parts;
							QList<float> items;
							parts = def.split(' ',QString::SkipEmptyParts);
							for(int i=0;i<parts.size();i++)
							items.push_back(parts[i].toFloat());
							pv = new value_s(items);
						}
						else if (type == "intlist")
						{
							QList<QString> parts;
							QList<int> items;
							parts = def.split(',',QString::SkipEmptyParts);
							for(int i=0;i<parts.size();i++)
							items.push_back(parts[i].toInt());
							pv = new value_s(items);
						}
						else /* int */	pv = new value_s(0);
					}
					else
					{
						//qDebug()<<"//contains";
						pv = pEnt->fields[field_and_type[0]];
					}
					if (type != "stringlist" && 
						 type != "floatlist" &&
						 type != "intlist")
					{
						if (!def.isNull())
							pv->fromString(def);
					}
					//qDebug()<<"step 8 -"<<i<<"-"<<j;
					pEnt->fields[field_and_type[0]] = pv;
					//qDebug()<<"step 9 -"<<i<<"-"<<j;
				}
			}
		}
		//qDebug()<<"m_EntsMap"<<m_EntsMap;
	}

}

void cEntityTool::ProcessInput()
{
	bool shift_down   = !!(MGR->KeyState(Qt::Key_Shift  ) & KEY_ISDOWN);
	bool control_down = !!(MGR->KeyState(Qt::Key_Control) & KEY_ISDOWN);
	bool alt_down     = !!(MGR->KeyState(Qt::Key_Alt    ) & KEY_ISDOWN);
	bool space_down   = !!(MGR->KeyState(Qt::Key_Space  ) & KEY_ISDOWN);
	if (!shift_down && !control_down && !alt_down && !space_down)
	{
		if (MGR->KeyState(Qt::Key_Delete) & KEY_PRESSED)
		{
			for(int i=m_SelectedEnts.size()-1;i>=0;i--)
			{
				m_Ents.removeAt(m_Ents.indexOf(m_SelectedEnts[i]));
				REMOVE_OBJECT(m_SelectedEnts[i]);
			}
			m_SelectedEnts.clear();
			UpdatePropsDlg();
		}
	}
	if (!shift_down && control_down && !alt_down && !space_down)
	{
		if (MGR->KeyState(Qt::Key_V) & KEY_PRESSED)
		{
			for(int i=m_SelectedEnts.size()-1;i>=0;i--)
			{
				m_SelectedEnts[i]->m_Selected = false;
				
				cBaseEntity *pEnt = (cBaseEntity *)GET_OBJECT(entity);
				pEnt->SetEntityType(m_SelectedEnts[i]->m_pEntity->type_name);
				delete pEnt->m_pEntity;
				pEnt->m_pEntity = new entity_t(m_SelectedEnts[i]->m_pEntity);
				pEnt->m_pEntity->type_name = m_SelectedEnts[i]->m_pEntity->type_name;
				pEnt->m_Orig = m_SelectedEnts[i]->m_Orig + QVector2D(16,16);
				m_Ents.push_back(pEnt);
				//REMOVE_OBJECT(m_SelectedEnts[i]);
			}
			m_SelectedEnts.clear();
			UpdatePropsDlg();
		}
	}
	if (!shift_down && !control_down && alt_down && !space_down)
	{
		if (MGR->KeyState(Qt::Key_Return) & KEY_PRESSED)
		{
			MGR->RegisterInput(Qt::Key_Alt,true);
			ShowPropsDlg();
		}
	}
	if (!shift_down /*&& !control_down*/ && !alt_down && !space_down)
	{
		if (MGR->KeyState(Qt::LeftButton) & KEY_PRESSED)
		{
			m_Action = ETA_NONE;
			m_SelectStart = MGR->MouseCoords();
			for(int i=m_Ents.size()-1;i>=0;i--)
			{
				if (m_Ents[i]->HasPoint(m_SelectStart))
				{
					if (m_Ents[i]->m_Selected)
					{
						m_Action = ETA_MOVE;
						break;
					}
				}
			}
			if (m_Action == ETA_NONE)
			{
				if (MGR->KeyState(Qt::Key_Control) & KEY_ISUP)
				{
					for(int i=m_SelectedEnts.size()-1;i>=0;i--)
						m_SelectedEnts[i]->m_Selected = false;
					m_SelectedEnts.clear();
					UpdatePropsDlg();
				}
				for(int i=m_Ents.size()-1;i>=0;i--)
				{
					if (m_Ents[i]->HasPoint(m_SelectStart))
					{
						m_Ents[i]->m_Selected = true;
						m_SelectedEnts.push_back(m_Ents[i]);
						m_Action = ETA_MOVE;
						UpdatePropsDlg();
						break;
					}
				}
				if (m_Action == ETA_NONE)
					m_Action = ETA_SELECT;
			}
		}
	}
	if (m_Action == ETA_SELECT)
	{
		if (MGR->KeyState(Qt::LeftButton) & KEY_RELEASED)
		{
			QVector2D selectEnd = MGR->MouseCoords();
			for(int i=m_Ents.size()-1;i>=0;i--)
			{
				if (m_Ents[i]->m_Orig.x() < std::min(m_SelectStart.x(),selectEnd.x())) continue;
				if (m_Ents[i]->m_Orig.x() > std::max(m_SelectStart.x(),selectEnd.x())) continue;
				if (m_Ents[i]->m_Orig.y() < std::min(m_SelectStart.y(),selectEnd.y())) continue;
				if (m_Ents[i]->m_Orig.y() > std::max(m_SelectStart.y(),selectEnd.y())) continue;
				
				m_Ents[i]->m_Selected = true;
				m_SelectedEnts.push_back(m_Ents[i]);
			}
			m_Action = ETA_NONE;
			UpdatePropsDlg();
		}
	}
	if (m_Action == ETA_MOVE)
	{
		m_MouseAccum += gMouseMove;
		
		for(int i=0;i<2;i++)
		{
			if (fabs(m_MouseAccum[i]) > MGR->GridCellSize())
			{
				QVector2D cells = m_MouseAccum/MGR->GridCellSize();
				cells[0] = (int)cells[0];
				cells[1] = (int)cells[1];
				cells *= MGR->GridCellSize();
				m_MouseAccum -= cells;
				
				for(int i=m_SelectedEnts.size()-1;i>=0;i--)
				{
					m_SelectedEnts[i]->m_Orig += cells;
				}
				
				//m_Backs[m_CurBackIdx]->m_Origin[i] += cells;
			}
		}
		if (MGR->KeyState(Qt::LeftButton) & KEY_RELEASED)
			m_Action = ETA_NONE;
	}
	if (shift_down && !control_down && !alt_down && !space_down)
	{
		if (MGR->KeyState(Qt::LeftButton) & KEY_PRESSED)
		{
			QVector2D mcoords_grid = MGR->MouseCoords(true);
			cBaseEntity *pEnt = (cBaseEntity *)MGR->AllocObject("entity");
			pEnt->m_Orig = mcoords_grid;
			pEnt->SetEntityType(gpMainWND->ui->list_Entities->currentText());
			
			//SET_OBJECT_DEPTH(pEnt,-100000000);
			
			m_Ents.push_back(pEnt);
		}
	}
}

void cEntityTool::DrawMe()
{
	if (m_Action == ETA_SELECT)
	{
		QVector2D &s = m_SelectStart, e = MGR->MouseCoords();
		glColor3f(1.f,1.f,1.f);
		glLineStipple(1,0x3333);
		glEnable(GL_LINE_STIPPLE);
		MGR->OutlineRect(s,e);
		glDisable(GL_LINE_STIPPLE);
	}
}

void cEntityTool::ClearALL()
{
	for(int i=m_Ents.size()-1;i>=0;i--)
		REMOVE_OBJECT(m_Ents[i]);
	
	m_Ents.clear();
}

void cEntityTool::UpdatePropsDlg()
{
	if (g_pProps->isVisible())
	{
		if (m_SelectedEnts.size() == 1)
		{
			if (m_SelectedEnts[0]->m_pEntity)
			{
				gIgnoreChange = true;
				g_pProps->ShowNormal();
				g_pProps->ui->list_entTypes->setEnabled(true);
				g_pProps->SetCallingTool(this);
				//g_pProps->SetEntsList(m_EntsMap.keys());
				g_pProps->SelectObject(m_SelectedEnts[0]);
				gIgnoreChange = false;
			}
			else
				g_pProps->ShowWrongSel("No entity exists");
		}
		else if (m_SelectedEnts.size() > 1)
			g_pProps->ShowWrongSel("Multiple objects selected");
		else
			g_pProps->ShowWrongSel("No selection");
	}
}

void cEntityTool::ShowPropsDlg()
{
	g_pProps->show();
	UpdatePropsDlg();
}

void cEntityTool::changeDepth()
{
	if (!m_SelectedEnts.size()) return;
	
	bool ok;
	int depth = QInputDialog::getInt(
				gpMainWND,
				"Change Depth",
				"Change Depth",
				m_SelectedEnts[0]->m_Depth,
				-2147483647,
				2147483647,
				1, &ok);
	if (ok)
	{
		for(int i = m_SelectedEnts.size()-1;i>=0;i--)
			SET_OBJECT_DEPTH(m_SelectedEnts[i],depth);
	}
}

void cEntityTool::ShiftAll()
{
	bool ok;
	int shx = QInputDialog::getInt(
				gpMainWND,
				"Shift X",
				"Shift X",
				0, 
				-999999.0,
				 999999.0,
				1, &ok);
	if (ok)
	{
		int shy = QInputDialog::getInt(
					gpMainWND,
					"Shift Y",
					"Shift Y",
					0, 
					-999999.0,
					 999999.0,
					1, &ok);
		if (ok)
		{
			for(int i = m_Ents.size();i >= 0; i--)
				m_Ents[i]->m_Orig += QVector2D(shx,shy);
			//m_Layers[m_CurLayerIdx]->ShiftLayer(QVector2D(shx,shy));
		}
	}
}
