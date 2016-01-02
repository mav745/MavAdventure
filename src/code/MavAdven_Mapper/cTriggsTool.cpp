#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <cGLManager.h>
#include "cTriggsTool.h"
#include <keys.h>
#include <triggers.h>

#include <QFile>
#include <cdlgentprops.h>
#include <ui_cdlgentprops.h>

#include <QInputDialog>

#include <QDebug>

extern bool gIgnoreChange;

//extern QMap<QString, entity_type_s*>g_TriggEnts;
extern QVector2D gMouseMove;

cTriggsTool *g_pTriggsTool;

//void ReassignEnt (const QString &newname)
//{
//	if (!g_pTriggsTool->m_EntsMap.contains(newname)) return;
	
//	qDebug()<<"1";
	
//	if (g_pTriggsTool->m_SelectedTriggs[0]->m_pEntity)
//		delete g_pTriggsTool->m_SelectedTriggs[0]->m_pEntity;
	
//	qDebug()<<"2";
	
//	g_pTriggsTool->m_SelectedTriggs[0]->m_pEntity = 
//			new entity_t(*g_TriggEnts[newname]);
	
//	qDebug()<<"3";
//	qDebug()<<g_pTriggsTool->m_SelectedTriggs[0]->m_pEntity;
//}

cTriggsTool::cTriggsTool(QObject *parent) :
	cBaseEntityTool(parent)
{
	g_pTriggsTool = this;

	m_Action = TTA_NONE;
	m_MouseAccum = QVector2D(0,0);
	
	connect(gpMainWND->ui->but_ShiftAllTrigs,SIGNAL(clicked()),this,SLOT(ShiftAll()));
	
	QFile triggsFGD("fgds/triggers.fgd");
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
			QStringList smallparts = parts[i].split("{",QString::SkipEmptyParts);
			QStringList entityheadparts = smallparts[0].split(":",QString::SkipEmptyParts);
			
			entity_type_s *pEnt;
			
			if (entityheadparts.size()>1)
			{
				pEnt = new entity_type_s(m_EntsMap[entityheadparts[1]]);
			}
			else
				pEnt = new entity_type_s;
			
			pEnt->type_name = entityheadparts[0];
			if (pEnt->type_name[0] == '@')
				pEnt->hidden = true;

			m_EntsMap[entityheadparts[0]] = pEnt;
			
			if (smallparts.size()>1)
			{
				QStringList fieldsparts = smallparts[1].split(";",QString::SkipEmptyParts);
				for(int j=0;j<fieldsparts.size();j++)
				{
					QStringList field_and_type = 
							fieldsparts[j].split(":",QString::SkipEmptyParts);
					
					value_s *pv;
					
					QStringList type_and_def = field_and_type[1].split("(",QString::SkipEmptyParts);
					QString type, def = "";
					type = type_and_def[0];
					if (type_and_def.size() > 1)
					{
						def = type_and_def[1];
						def.remove(")");
					}
						  if (type == "float")	  pv = new value_s(0.f);
					else if (type == "vector2")  pv = new value_s(0.f,0.f);
					else if (type == "color")	  pv = new value_s(0.f,0.f,0.f);
					else if (type == "string")   pv = new value_s(QString());
					else if (type == "file")     pv = new value_s(QByteArray());
					else if (type == "bool")	  pv = new value_s(false);
					else if (type == "stringlist")
					{
						//qDebug()<<field_and_type[0];
						QList<QString> parts;
						QList<QString> items;
						parts = def.split(',',QString::SkipEmptyParts);
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
						parts = def.split(' ',QString::SkipEmptyParts);
						for(int i=0;i<parts.size();i++)
						items.push_back(parts[i].toInt());
						pv = new value_s(items);
					}
					else /* int */					  pv = new value_s(0);
					
					if (!(type == "stringlist" || 
						   type == "floatlist" ||
						   type == "intlist"))
					{
					if (!def.isNull())
						pv->fromString(def);
					}
						  
					pEnt->fields[field_and_type[0]] = pv;
				}
			}
		}
	}
}

void cTriggsTool::ClearALL()
{
	for(int i=m_Triggs.size()-1;i>=0;i--)
		REMOVE_OBJECT(m_Triggs[i]);
	
	m_Triggs.clear();
}
void cTriggsTool::ShowPropsDlg()
{
	g_pProps->show();
	UpdatePropsDlg();
}

void cTriggsTool::ShiftAll()
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
			for(int i = m_Triggs.size();i >= 0; i--)
			{
				for(int j = m_Triggs[i]->m_Parts.size();j >= 0; j--)
				{
					m_Triggs[i]->m_Parts[j]->s += QVector2D(shx,shy);
					m_Triggs[i]->m_Parts[j]->e += QVector2D(shx,shy);
					m_Triggs[i]->m_Parts[j]->RecalcBounds();
				}
				m_Triggs[i]->RecountMinMax();
			}
		}
	}
}

void cTriggsTool::UpdatePropsDlg()
{
	if (g_pProps->isVisible())
	{
		if (m_SelectedTriggs.size() == 1)
		{
			if (m_SelectedTriggs[0]->m_pEntity)
			{
				gIgnoreChange = true;
				g_pProps->ShowNormal();
				g_pProps->ui->list_entTypes->setEnabled(true);
				g_pProps->SetCallingTool(this);
				//g_pProps->SetEntsList(m_EntsMap.keys());
				g_pProps->SelectObject(m_SelectedTriggs[0]);
				gIgnoreChange = false;
			}
			else
				g_pProps->ShowWrongSel("No entity exists");
		}
		else if (m_SelectedTriggs.size() > 1)
			g_pProps->ShowWrongSel("Multiple objects selected");
		else
			g_pProps->ShowWrongSel("No selection");
	}
}

void cTriggsTool::ProcessInput()
{
	bool shift_down   = !!(MGR->KeyState(Qt::Key_Shift  ) & KEY_ISDOWN);
	bool control_down = !!(MGR->KeyState(Qt::Key_Control) & KEY_ISDOWN);
	bool alt_down     = !!(MGR->KeyState(Qt::Key_Alt    ) & KEY_ISDOWN);
	bool space_down   = !!(MGR->KeyState(Qt::Key_Space  ) & KEY_ISDOWN);
	
	if (!shift_down && !control_down && !alt_down && !space_down)
	{
		if (MGR->KeyState(Qt::Key_Delete) & KEY_PRESSED)
		{
			for(int i=m_SelectedTriggs.size()-1;i>=0;i--)
			{
				m_Triggs.removeAt(m_Triggs.indexOf(m_SelectedTriggs[i]));
				REMOVE_OBJECT(m_SelectedTriggs[i]);
			}
			m_SelectedTriggs.clear();
			UpdatePropsDlg();
		}
	}
	if (!shift_down && !control_down &&  alt_down && !space_down)
	{
		if (MGR->KeyState(Qt::Key_Return) & KEY_PRESSED)
		{
			MGR->RegisterInput(Qt::Key_Alt,true);
			ShowPropsDlg();
		}
	}
	if (!shift_down && control_down && !alt_down && !space_down)
	{
//		if (MGR->KeyState(Qt::Key_T) & KEY_PRESSED)
//		{
//			for(int i=m_SelectedTriggs.size()-1;i>=0;i--)
//			{
//				if (m_SelectedTriggs[i]->m_pEntity) continue;
				
//				m_SelectedTriggs[i]->SetEntityType(m_EntsMap.keys()[0]);
//				m_SelectedTriggs[i]->m_Selected = false;
//			}
//			m_SelectedTriggs.clear();
//		}
		if (MGR->KeyState(Qt::Key_A) & KEY_PRESSED)
		{
			m_SelectedTriggs = m_Triggs;
			for(int i=m_Triggs.size()-1;i>=0;i--)
				m_Triggs[i]->m_Selected = true;
			UpdatePropsDlg();
		}
		if (MGR->KeyState(Qt::Key_E) & KEY_PRESSED)
		{
			//m_SelectedTriggs = m_Triggs;
			if (m_SelectedTriggs.size() >= 2)
			{
				int sz = m_SelectedTriggs.size();
				cBaseTrigger *pT = m_SelectedTriggs[0];
				for(int i=1;i<sz;i++)
				{
					pT->Merge(m_SelectedTriggs[i]);
					m_Triggs.removeAt(m_Triggs.indexOf(m_SelectedTriggs[i]));
					REMOVE_OBJECT(m_SelectedTriggs[i]);
				}
				m_SelectedTriggs.clear();
				m_SelectedTriggs.push_back(pT);
				
				UpdatePropsDlg();
			}
		}
	}
	if (!shift_down /*&& !control_down*/ && !alt_down && !space_down)
	{
		if (MGR->KeyState(Qt::LeftButton) & KEY_PRESSED)
		{
			m_Action = TTA_NONE;
			if (!control_down)
			{
				for(int i=m_SelectedTriggs.size()-1;i>=0;i--)
					m_SelectedTriggs[i]->m_Selected = false;
				m_SelectedTriggs.clear();
			}
			
			for(int i=m_Triggs.size()-1;i>=0;i--)
			{
				if (m_Triggs[i]->m_Selected) continue;
				
				if (m_Triggs[i]->ShapeHasPoint(MGR->MouseCoords()))
				{
					m_Triggs[i]->m_Selected = true;
					m_SelectedTriggs.push_back(m_Triggs[i]);
				}
			}
			for(int i=m_SelectedTriggs.size()-1;i>=0;i--)
			{
				if (m_SelectedTriggs[i]->ShapeHasPoint(MGR->MouseCoords()))
				{
					m_Action = TTA_MOVE;
					break;
				}
			}
			if (m_Action == TTA_NONE)
			{
				m_SelectStart = MGR->MouseCoords();
				m_Action = TTA_SELECT;
				//m_Selecting = true;
				
			}
			UpdatePropsDlg();
		}
	}
	if (shift_down && !control_down && !alt_down && !space_down)
	{
		if (MGR->KeyState(Qt::LeftButton) & KEY_PRESSED)
		{
			m_CreateStart = MGR->MouseCoords(true);
			m_Action = TTA_CREATE;
			//m_Creating = true;
		}
	}
	if (m_Action == TTA_MOVE)
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
				
				for(int i=m_SelectedTriggs.size()-1;i>=0;i--)
					m_SelectedTriggs[i]->Shift(cells);
				
				//m_Backs[m_CurBackIdx]->m_Origin[i] += cells;
			}
		}
		if (MGR->KeyState(Qt::LeftButton) & KEY_RELEASED)
			m_Action = TTA_NONE;
	}
	if (m_Action == TTA_CREATE)
	{
		if (MGR->KeyState(Qt::LeftButton) & KEY_RELEASED)
		{
			//m_Creating = false;
			m_Action = TTA_NONE;
//			if (gpMainWND->ui->list_TriggsMode->currentText() == "Rectangle")
//			{
			
			if (m_CreateStart.x() != MGR->MouseCoords(true).x() &&
				 m_CreateStart.y() != MGR->MouseCoords(true).y())
			{
				cBaseTrigger *pT = (cBaseTrigger *)GET_OBJECT(trigger);
				pT->AddRectShape(m_CreateStart,MGR->MouseCoords(true));
				pT->SetEntityType("trigger_once"/*m_EntsMap.keys()[0]*/);
				m_Triggs.push_back(pT);
			}
			
//			}
//			else
//			{
//				cBaseTrigger *pT = (cBaseTrigger *)GET_OBJECT(trigger);
//				pT->AddCircShape(m_CreateStart,(MGR->MouseCoords(true)-m_CreateStart).length());
//				m_Triggs.push_back(pT);
//			}
		}
	}
	if (m_Action == TTA_SELECT)
	{
		if (MGR->KeyState(Qt::LeftButton) & KEY_RELEASED)
		{
			m_Action = TTA_NONE;
			
			for(int i=m_Triggs.size()-1;i>=0;i--)
			{
				if (m_Triggs[i]->Crosses(m_SelectStart,MGR->MouseCoords()))
				{
					m_Triggs[i]->m_Selected = true;
					m_SelectedTriggs.push_back(m_Triggs[i]);
				}
			}
			
			UpdatePropsDlg();
		}
	}
}

void cTriggsTool::DrawMe()
{
	if (m_Action == TTA_SELECT)
	{
		QVector2D &s = m_SelectStart, e = MGR->MouseCoords();
		glColor3f(1.f,1.f,1.f);
		glLineStipple(1,0x3333);
		glEnable(GL_LINE_STIPPLE);
		MGR->OutlineRect(s,e);
		glDisable(GL_LINE_STIPPLE);
	}
	if (m_Action == TTA_CREATE)
	{
		QVector2D &s = m_CreateStart, e = MGR->MouseCoords(true);
		
//		if (gpMainWND->ui->list_TriggsMode->currentText() == "Rectangle")
//		{
		glColor3f(1.f,0.5f,0.f);
		glLineStipple(2,0x3333);
		glEnable(GL_LINE_STIPPLE);
		MGR->OutlineRect(s,e);
		glDisable(GL_LINE_STIPPLE);
		
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glTranslatef(MGR->m_View.x(),MGR->m_View.y(),0.f);
		
		glDisable(GL_TEXTURE_2D);
		glPointSize(4.f);
		
		glBegin(GL_POINTS);
		glVertex2f(s.x(),s.y());
		glVertex2f(s.x(),e.y());
		glVertex2f(e.x(),e.y());
		glVertex2f(e.x(),s.y());
		glEnd();
		
		glPointSize(1.f);
		glEnable(GL_TEXTURE_2D);
		glLoadIdentity();
//		}
//		else
//		{
//			glColor3f(1.f,0.5f,0.f);

//			glDisable(GL_TEXTURE_2D);
//			glPointSize(4.f);

//			glMatrixMode(GL_MODELVIEW);
//			glLoadIdentity();
//			glTranslatef(MGR->m_View.x(),MGR->m_View.y(),0.f);
//			glBegin(GL_POINTS);
//			glVertex2f(s.x(),s.y());
//			glEnd();

//			glLineStipple(2,0x3333);
//			glEnable(GL_LINE_STIPPLE);
//			float rad = (MGR->MouseCoords(true)-m_CreateStart).length();
//			float aStep = atan2(32.f,rad)/2.f;
//			QVector2D p;

//			glBegin(GL_LINE_LOOP);
//			for(float f = M_PI*2.f;f>0.f;f -= aStep)
//			{
//				p = m_CreateStart+rad*QVector2D(cos(f),sin(f));
//				glTexCoord2f(p.x()/64.f,-p.y()/64.f);
//				glVertex2f(p.x(),p.y());
//			}
//			glEnd();

//			glLoadIdentity();

//			glPointSize(1.f);
//			glEnable(GL_TEXTURE_2D);

//			MGR->DrawLine(s,e);
//			glDisable(GL_LINE_STIPPLE);
//		}
	}
}

//void cTriggsTool::ClearALL()
//{
//	for(int i=m_Triggs.size()-1;i>=0;i--)
//		REMOVE_OBJECT(m_Triggs[i]);
	
//	m_Triggs.clear();
//}
