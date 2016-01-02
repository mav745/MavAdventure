#include "cLineTool.h"
#include <cGLManager.h>
#include <cSegment.h>
#include <mainwindow.h>
#include <ui_mainwindow.h>

#include <QDebug>

#include <QInputDialog>
#include <keys.h>

#include <algorithm>

using namespace std;

extern QVector2D gMouseMove;

cLineTool *g_pLineTool;

cLineTool::cLineTool(QObject *parent) :
	cBaseTool(parent)
{
	m_Visible = true;
	g_pLineTool = this;
	connect(gpMainWND->ui->but_lineShift,SIGNAL(clicked()),this,SLOT(shiftLines()));
	connect(gpMainWND->ui->but_Flip,SIGNAL(clicked()),this,SLOT(flipLines()));
	connect(gpMainWND->ui->but_lineJDown,SIGNAL(clicked()),this,SLOT(jumpFlagLines()));
	connect(gpMainWND->ui->but_MirrorH,SIGNAL(clicked()),this,SLOT(mirrorHor()));
	connect(gpMainWND->ui->but_MirrorV,SIGNAL(clicked()),this,SLOT(mirrorVer()));
	connect(gpMainWND->ui->but_linesVisible,SIGNAL(clicked()),this,SLOT(SwitchVisibility()));
	
	gpMainWND->ui->list_linesMode->addItem("Line");
	gpMainWND->ui->list_linesMode->addItem("Polygon");
	
	//gpMainWND->ui->spin_CircSides->hide();
	gpMainWND->ui->spin_CircSides->setMaximum(24);
	gpMainWND->ui->spin_CircSides->setMinimum( 3);
	
	m_Selecting = false;
}
void cLineTool::SwitchVisibility()
{
	m_Visible = !m_Visible;
}

void cLineTool::ClearALL()
{
	while(m_Lines.size())
	{
		cLine *pLine = (cLine *)m_Lines[0];
		m_Lines.removeAt(0);
		REMOVE_OBJECT(pLine);
	}
	
	m_Lines.clear();
	m_SelectedLines.clear();
	m_CopiedLines.clear();
}

//void cLineTool::DeleteLine(cLine *pLine)
//{
////	QMap<long long, cSegment*>::iterator it;
////	int found = 0;
////	for(it = m_Lines.begin();it != m_Lines.end();it++)
////	{
////		if (it.value() == pLine)
////		{
//////			cSegment *seg = it.value(), *oseg;
			
//////			for(int i=seg->m_Neighs.size()-1;i>=0;i--)
//////			{
//////				oseg = seg->m_Neighs[i];
//////				int idx = oseg->m_Neighs.indexOf(seg);
				
//////				if (idx != -1)
//////					oseg->m_Neighs.removeAt(idx);
//////			}
////			m_Lines.erase(it);
////			found++;
////			if (found == 2)
////				return;
////		}
////	}
//}

void cLineTool::jumpFlagLines()
{
	for(int i=m_SelectedLines.size()-1;i>=0;i--)
	{
		m_SelectedLines[i]->m_JumpDown = !m_SelectedLines[i]->m_JumpDown;
	}
}

void cLineTool::mirrorHor()
{
	cLine *pLine;
	int mirrorEdge, minX;//, maxX;
	minX =  9999999;
	//maxX = -9999999;
	for(int i=m_SelectedLines.size()-1;i>=0;i--)
	{
		pLine = m_SelectedLines[i];
		
		if (pLine->m_S.x() < minX) minX = pLine->m_S.x();
		//if (pLine->m_S.x() > maxX) maxX = pLine->m_S.x();
		
		if (pLine->m_E.x() < minX) minX = pLine->m_E.x();
		//if (pLine->m_E.x() > maxX) maxX = pLine->m_E.x();
	}
	mirrorEdge = minX;
	for(int i=m_SelectedLines.size()-1;i>=0;i--)
	{
		pLine = m_SelectedLines[i];
		pLine->m_S.rx() = 2*mirrorEdge-pLine->m_S.rx();
		pLine->m_E.rx() = 2*mirrorEdge-pLine->m_E.rx();
		pLine->CalcMe();
	}
	flipLines();
}

void cLineTool::mirrorVer()
{
	cLine *pLine;
	int mirrorEdge, minY;//, maxY;
	minY =  9999999;
	//maxY = -9999999;
	for(int i=m_SelectedLines.size()-1;i>=0;i--)
	{
		pLine = m_SelectedLines[i];
		
		if (pLine->m_S.y() < minY) minY = pLine->m_S.y();
		//if (pLine->m_S.x() > maxY) maxY = pLine->m_S.x();
		
		if (pLine->m_E.y() < minY) minY = pLine->m_E.y();
		//if (pLine->m_E.x() > maxY) maxY = pLine->m_E.x();
	}
	mirrorEdge = minY;
	for(int i=m_SelectedLines.size()-1;i>=0;i--)
	{
		pLine = m_SelectedLines[i];
		pLine->m_S.ry() = 2*mirrorEdge-pLine->m_S.ry();
		pLine->m_E.ry() = 2*mirrorEdge-pLine->m_E.ry();
		pLine->CalcMe();
	}
	flipLines();
}

void cLineTool::flipLines()
{
	cLine *pLine;
	for(int i=m_SelectedLines.size()-1;i>=0;i--)
	{
		pLine = m_SelectedLines[i];
		QPoint s = pLine->m_S;
		pLine->m_S = pLine->m_E;
		pLine->m_E = s;
		pLine->CalcMe();
	}
}

void cLineTool::shiftLines()
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
			for(int i=m_Lines.size()-1;i>=0;i--)
			{
				m_Lines[i]->m_S += QPoint(shx,shy);
				m_Lines[i]->m_E += QPoint(shx,shy);
				m_Lines[i]->CalcMe();
			}
			//m_Layers[m_CurLayerIdx]->ShiftLayer(QVector2D(shx,shy));
		}
	}
}
void cLineTool::ProcessInput()
{
	bool shift_down   = !!(MGR->KeyState(Qt::Key_Shift  ) & KEY_ISDOWN);
	bool control_down = !!(MGR->KeyState(Qt::Key_Control) & KEY_ISDOWN);
	bool alt_down     = !!(MGR->KeyState(Qt::Key_Alt    ) & KEY_ISDOWN);
	bool space_down   = !!(MGR->KeyState(Qt::Key_Space  ) & KEY_ISDOWN);
	//bool mbleft_pressed = !!(MGR->KeyState(Qt::LeftButton  ) & KEY_PRESSED);
	//bool mbleft_down    = !!(MGR->KeyState(Qt::LeftButton  ) & KEY_ISDOWN);

	if (MGR->KeyState(Qt::Key_V) & KEY_PRESSED)
	{
		if ( control_down && !space_down && !shift_down && !alt_down)
		{
			if (m_SelectedLines.size())
			{
				cSegment *pSeg;
				QVector<cLine *> newSelected;
				for(int i=m_SelectedLines.size()-1;i>=0;i--)
				{
					pSeg = (cSegment *)GET_OBJECT(segment);
					pSeg->m_S = m_SelectedLines[i]->m_S+QPoint(32,32);
					pSeg->m_E = m_SelectedLines[i]->m_E+QPoint(32,32);
					pSeg->CalcMe();
					pSeg->m_Selected = true;
					newSelected.push_back(pSeg);
					m_SelectedLines[i]->m_Selected = false;
					m_Lines.push_back(pSeg);
				}
				m_SelectedLines = newSelected;
			}
		}
	}
	if (MGR->KeyState(Qt::Key_A) & KEY_PRESSED)
	{
		if ( control_down && !space_down && !shift_down && !alt_down)
		{
			m_SelectedLines = m_Lines;
			for(int i=m_Lines.size()-1;i>=0;i--)
				m_Lines[i]->m_Selected = true;
		}
	}
	if (MGR->KeyState(Qt::RightButton  ) & KEY_PRESSED)
	{
		if ( control_down && !space_down && !shift_down && !alt_down)
		{
			int sz = m_Lines.size();
			if (sz)
			{
				cLine *pLine;
				for(int i=0;i<sz;i++)
				{
					pLine = m_Lines[i];
					if (pLine->ContaintsPoint(MGR->MouseCoords(),8.f))
					{
						QPoint s = pLine->m_S;
						pLine->m_S = pLine->m_E;
						pLine->m_E = s;
						pLine->CalcMe();
					}
				}
			}
		}
		if (!control_down && !space_down &&  shift_down && !alt_down)
		{
			int sz = m_Lines.size();
			if (sz)
			{
				cLine *pLine;
				for(int i=0;i<sz;i++)
				{
					pLine = m_Lines[i];
					if (pLine->ContaintsPoint(MGR->MouseCoords(),8.f))
					{
						pLine->m_JumpDown = !pLine->m_JumpDown;
					}
				}
			}
		}
	}
	if ((MGR->KeyState(Qt::Key_Left ) & KEY_PRESSED) ||
		 (MGR->KeyState(Qt::Key_Right) & KEY_PRESSED) ||
		 (MGR->KeyState(Qt::Key_Up   ) & KEY_PRESSED) ||
		 (MGR->KeyState(Qt::Key_Down ) & KEY_PRESSED))
	{
		QPoint shift(0,0);
		if (MGR->KeyState(Qt::Key_Left ) & KEY_PRESSED) shift.setX(-1);
		if (MGR->KeyState(Qt::Key_Right) & KEY_PRESSED) shift.setX( 1);
		if (MGR->KeyState(Qt::Key_Up   ) & KEY_PRESSED) shift.setY(-1);
		if (MGR->KeyState(Qt::Key_Down ) & KEY_PRESSED) shift.setY( 1);
		for(int i=m_SelectedLines.size()-1;i>=0;i--)
		{
			m_SelectedLines[i]->m_S += shift*MGR->GridCellSize();
			m_SelectedLines[i]->m_E += shift*MGR->GridCellSize();
			m_SelectedLines[i]->CalcMe();
		}
	}
//	if (MGR->KeyState(Qt::LeftButton  ) & KEY_ISDOWN)
//	{
//		if ( control_down && !space_down && shift_down && !alt_down)
//		{
			
//		}
//	}
	if (MGR->KeyState(Qt::LeftButton  ) & KEY_PRESSED)
	{
		if (!control_down && !space_down &&  shift_down && !alt_down)
			m_Start = MGR->MouseCoords(true);
		
		if (/*!control_down && */!space_down && !shift_down && !alt_down)
		{
			//QList<cSegment*>tLines = m_Lines.values();
			m_Selecting = true;
			m_SelStart = MGR->MouseCoords();
			
			int sz = m_Lines.size();
			if (!control_down)
				m_SelectedLines.clear();
			if (sz)
			{
				cLine *pLine;
				for(int i=0;i<sz;i++)
				{
					pLine = m_Lines[i];
					if (pLine->ContaintsPoint(MGR->MouseCoords(),16.f))
					{
						pLine->m_Selected = true;
						m_SelectedLines.push_back(pLine);
					}
					else if (!control_down)
						pLine->m_Selected = false;
				}
			}
		}
	}
	if (m_Selecting)
	{
		if (MGR->KeyState(Qt::LeftButton  ) & KEY_RELEASED)
		{
			m_Selecting = false;
			QVector2D selEnd = MGR->MouseCoords();
			int l,u,r,d;
			l = min(m_SelStart.x(),selEnd.x());
			r = max(m_SelStart.x(),selEnd.x());
			u = min(m_SelStart.y(),selEnd.y());
			d = max(m_SelStart.y(),selEnd.y());
			for(int i=m_Lines.size()-1;i>=0;i--)
			{
				if ((m_Lines[i]->m_S.x() >= l &&
					  m_Lines[i]->m_S.x() <= r &&
					  m_Lines[i]->m_S.y() >= u &&
					  m_Lines[i]->m_S.y() <= d) ||
					 
					 (m_Lines[i]->m_E.x() >= l &&
					  m_Lines[i]->m_E.x() <= r &&
					  m_Lines[i]->m_E.y() >= u &&
					  m_Lines[i]->m_E.y() <= d))
				{
					if (!m_Lines[i]->m_Selected)
					{
						m_SelectedLines.push_back(m_Lines[i]);
						m_Lines[i]->m_Selected = true;
					}
				}
			}
		}
	}
	if (MGR->KeyState(Qt::LeftButton  ) & KEY_RELEASED)
	{
		if (!control_down && !space_down && shift_down && !alt_down)
		{
			m_End = MGR->MouseCoords(true);
			if (m_End == m_Start) return;
		
			if (gpMainWND->ui->list_linesMode->currentText() == "Line")
			{
				cSegment *pLine = (cSegment *)GET_OBJECT(segment);
				pLine->m_S.rx() = m_Start.x();
				pLine->m_S.ry() = m_Start.y();
				pLine->m_E.rx() = m_End.x();
				pLine->m_E.ry() = m_End.y();
				pLine->CalcMe();
				m_Lines.push_back(pLine);
			}
			else
			{
				float sides = gpMainWND->ui->spin_CircSides->value();
				m_SelectedLines.clear();
				float start_ang = 0;
				if (sides != 4)
				{
					float step = 360.f/sides;
					//step *= M_PI/180.f;
					QVector2D s,e,c;
					float w,h;
					c = m_End + m_Start; c *= 0.5f;
					w = fabs(m_End.x()-m_Start.x())*0.5f;
					h = fabs(m_End.y()-m_Start.y())*0.5f;
					for(float a=start_ang;a<start_ang+360;a += step)
					{
						s = c + QVector2D(w*cos(a*M_PI/180.f),h*sin(a*M_PI/180.f));
						e = c + QVector2D(w*cos((a+step)*M_PI/180.f),h*sin((a+step)*M_PI/180.f));
						cSegment *pLine = (cSegment *)GET_OBJECT(segment);
						pLine->m_S.rx() = s.x();
						pLine->m_S.ry() = s.y();
						pLine->m_E.rx() = e.x();
						pLine->m_E.ry() = e.y();
						pLine->CalcMe();
						pLine->m_Selected = true;
						m_SelectedLines.push_back(pLine);
						m_Lines.push_back(pLine);
					}
				}
				else
				{
					float vx[] = {m_Start.x(),m_End.x(),m_End.x(),m_Start.x(),m_Start.x()};
					float vy[] = {m_Start.y(),m_Start.y(),m_End.y(),m_End.y(),m_Start.y()};
					
					for(int i=0;i<4;i++)
					{
						cSegment *pLine = (cSegment *)GET_OBJECT(segment);
						pLine->m_S.rx() = vx[i];
						pLine->m_S.ry() = vy[i];
						pLine->m_E.rx() = vx[i+1];
						pLine->m_E.ry() = vy[i+1];
						pLine->CalcMe();
						pLine->m_Selected = true;
						m_SelectedLines.push_back(pLine);
						m_Lines.push_back(pLine);
					}
				}
			}
			
//			long long s,e,os,oe;
//			s = pLine->m_S.x(); s <<= 32; s += pLine->m_S.y();
//			e = pLine->m_E.x(); e <<= 32; e += pLine->m_E.y();
//			QList<cSegment*>other_lines = m_Lines.values(s) + m_Lines.values(e);
//			for(int i=other_lines.size()-1;i>=0;i--)
//			{
//				os = other_lines[i]->m_S.x(); os <<= 32; os += other_lines[i]->m_S.y();
//				oe = other_lines[i]->m_E.x(); oe <<= 32; oe += other_lines[i]->m_E.y();
//				if ((other_lines[i]->m_S == pLine->m_E)||
//					 (other_lines[i]->m_E == pLine->m_S))
//				{
//					pLine->m_Neighs.push_back(other_lines[i]);
//					other_lines[i]->m_Neighs.push_back(pLine);
//				}
//			}
//			m_Lines.insertMulti(s,pLine);
//			m_Lines.insertMulti(e,pLine);
			
			m_Start = m_End;
		}
	}
	if (MGR->KeyState(Qt::Key_Delete  ) & KEY_PRESSED)
	{
		for(int i= m_SelectedLines.size()-1;i>=0;i--)
		{
			cLine *pLine = (cLine *)m_SelectedLines[i];
			m_Lines.removeAt(m_Lines.indexOf(pLine));
			//DeleteLine(pLine);
			//m_Lines.remove()
			REMOVE_OBJECT(pLine);
		}
		
		m_SelectedLines.clear();
	}
}
void cLineTool::DrawMe()
{
	bool shift_down   = !!(MGR->KeyState(Qt::Key_Shift  ) & KEY_ISDOWN);
	bool control_down = !!(MGR->KeyState(Qt::Key_Control) & KEY_ISDOWN);
	bool alt_down     = !!(MGR->KeyState(Qt::Key_Alt    ) & KEY_ISDOWN);
	bool space_down   = !!(MGR->KeyState(Qt::Key_Space  ) & KEY_ISDOWN);
	
	if ((MGR->KeyState(Qt::LeftButton) & KEY_ISDOWN) &&
		 !space_down &&
		 !control_down &&
		 !alt_down &&
		 shift_down)
	{
		//qDebug()<<"drawing line";
		glColor3f(0.75f,0.75f,0.75f);
		if (gpMainWND->ui->list_linesMode->currentText() == "Line")
		{
			glLineStipple(3,0x3333);
			glEnable(GL_LINE_STIPPLE);
			
			MGR->DrawLine(m_Start,MGR->MouseCoords(true));

			MGR->FillRect(m_Start-QVector2D(2,2),m_Start+QVector2D(2,2));
			MGR->FillRect(MGR->MouseCoords(true)-QVector2D(2,2),MGR->MouseCoords(true)+QVector2D(2,2));
			
			glDisable(GL_LINE_STIPPLE);
		}
		else
		{
			QVector2D mcrds = MGR->MouseCoords(true);
			QVector2D c1,c2,c3,c4,c;
			c1 = m_Start+QVector2D(mcrds.x(),m_Start.y());
			c2 = m_Start+QVector2D(m_Start.x(),mcrds.y());
			c3 = mcrds+QVector2D(mcrds.x(),m_Start.y());
			c4 = mcrds+QVector2D(m_Start.x(),mcrds.y());
			c1 *= 0.5f; c2 *= 0.5f; c3 *= 0.5f; c4 *= 0.5f;
			QVector2D l(3,3);
			MGR->FillRect(c1-l,c1+l);
			MGR->FillRect(c2-l,c2+l);
			MGR->FillRect(c3-l,c3+l);
			MGR->FillRect(c4-l,c4+l);
			
			c = m_Start+mcrds; c *= 0.5f;
			MGR->DrawLine(c-l,c+l);
			MGR->DrawLine(c-QVector2D(-3,3),c+QVector2D(-3,3));
			
			glLineStipple(3,0x3333);
			glEnable(GL_LINE_STIPPLE);
			
			MGR->OutlineRect(m_Start,mcrds);
			
			glDisable(GL_LINE_STIPPLE);
		}
	}
	if (m_Selecting)
	{
		glColor3f(1,1,1);
		glLineStipple(1,0x3333);
		glEnable(GL_LINE_STIPPLE);
		
		MGR->OutlineRect(m_SelStart,MGR->MouseCoords());
		
		glDisable(GL_LINE_STIPPLE);
	}
}
