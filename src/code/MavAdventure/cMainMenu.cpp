#include "cMainMenu.h"
#include <cMainManager.h>
#include <cGameRoom.h>

#include <util.h>

#include <QFile>
#include <QFontMetrics>
#include <cBaseObject.h>

#include <algorithm>

#include <QDateTime>
#include <QDebug>
#include <QThread>

#include "cConsole.h"
#include <QApplication>

#include <QDir>

extern QThread *g_pConThread;

using namespace std;

cMainMenu::cMainMenu(QObject *parent) :
	cBaseMenu(parent)
{
	m_Items.push_back("#Story");
	m_Items.push_back("#FreePlay");
	m_Items.push_back("#Controls");
	m_Items.push_back("#Extras");
	m_Items.push_back(" ");
	m_Items.push_back("#Exit");
	
	connect(this,SIGNAL(itemSelected(QString)),this,SLOT(Selection(QString)));
}
void cMainMenu::Selection(QString item)
{
	if (item == "#Exit")
	{
		QApplication::quit();
		//g_pCon->moveToThread(QApplication::instance()->thread());
		//g_pConThread->exit();
		//g_pConThread->terminate();
		//g_pConThread->wait();
		//delete g_pConThread;
	}
}

void cMainMenu::RenderMe()
{
	MGR->m_pCurrentRoom->ObjectByTargetname("cur_l")->m_Visible = true;
	MGR->m_pCurrentRoom->ObjectByTargetname("cur_r")->m_Visible = true;
	
	cBaseMenu::RenderMe();
	
	if (!m_pFont) return;
	
	QFontMetrics fm(*m_pFont);
	int h = 28;//fm.height()+2;
	
	cBaseObject *curL, *curR;
	
	QList<cBaseObject *> objs = MGR->m_pCurrentRoom->ObjectsByTargetname("cur_l");
	if (!objs.size()) return;
	curL = objs[0];
	
	objs = MGR->m_pCurrentRoom->ObjectsByTargetname("cur_r");
	if (!objs.size()) return;
	curR = objs[0];
	
	int w = s_pMenuFont->strWidth(s_Locale[m_Items[m_Cursor]]);
	
	QVector2D vL, vR;
	vL[YY] = m_Orig.y()+m_Cursor*h+h-10.f;
	vR[YY] = vL[YY];
	vL[XX] = m_Orig.x()-w/2-24;
	vR[XX] = m_Orig.x()+w/2+24;
	
	for(int i=0;i<2;i++)
	{
		curL->m_Velocity[i] = min(8.f,max(vL[i]-curL->m_Orig[i],-8.f));
		curR->m_Velocity[i] = min(8.f,max(vR[i]-curR->m_Orig[i],-8.f));
	}
}

cOptionsMenu::cOptionsMenu(QObject *parent) :
	cBaseMenu(parent)
{
	
}

custom_font_s *cControllsMenu::s_KeysFont = 0;

cControllsMenu::cControllsMenu(QObject *parent) :
	cBaseMenu(parent)
{
	QFile pF("controls.cfg");
	if (pF.open(QFile::ReadOnly))
	{
		QDataStream DS(&pF);
		DS>>butUp>>butDown>>butLeft>>butRight>>butA>>butB>>butC>>butStart;
		pF.close();
	}
	
	m_MaxWidth = 0;
	
	m_Wait = false;
	
	m_NewKeys[0] = butUp;
	m_NewKeys[1] = butLeft;
	m_NewKeys[2] = butDown;
	m_NewKeys[3] = butRight;
	m_NewKeys[4] = butStart;
	m_NewKeys[5] = butC;
	m_NewKeys[6] = butB;
	m_NewKeys[7] = butA;
	
	QString butTexNames[8] =
	{
		"up",
		"left",
		"down",
		"right",
		"start",
		"c",
		"b",
		"a"
	};
	for(int i=0;i<8;i++)
	{
		m_ButTexs[i] = MGR->bindTexture(QImage(QDir::currentPath()+"/res/sprites/menu/"+butTexNames[i]+".png"),
												  GL_TEXTURE_2D,
											     GL_RGBA,
											     QGLContext::InvertedYBindOption);
	}
	m_Items.push_back("#up");			
	m_KeyOrigs[0] = QVector2D(206-8,168);
	m_TexOrigs[0] = QVector2D(206,238);
			
	m_Items.push_back("#left");		
	m_KeyOrigs[1] = QVector2D(184-8,201);
	m_TexOrigs[1] = QVector2D(185,260);
	
	m_Items.push_back("#down");		
	m_KeyOrigs[2] = QVector2D(206-8,384);
	m_TexOrigs[2] = QVector2D(206,277);
			
	m_Items.push_back("#right");		
	m_KeyOrigs[3] = QVector2D(229-8,412);
	m_TexOrigs[3] = QVector2D(223,260);
			
	m_Items.push_back("#start");		
	m_KeyOrigs[4] = QVector2D(360+8,168);
	m_TexOrigs[4] = QVector2D(337,213);
			
	m_Items.push_back("#special");	
	m_KeyOrigs[5] = QVector2D(413+8,201);
	m_TexOrigs[5] = QVector2D(392,239);
			
	m_Items.push_back("#jump");		
	m_KeyOrigs[6] = QVector2D(388+8,384);
	m_TexOrigs[6] = QVector2D(362,253);
			
	m_Items.push_back("#attack");		
	m_KeyOrigs[7] = QVector2D(359+8,412);
	m_TexOrigs[7] = QVector2D(333,269);
			
	m_Items.push_back(" ");
	m_Items.push_back("#OK");
	m_Items.push_back("#Cancel");
	
	QFile kNames("settings/keynames.txt");
	if (kNames.open(QFile::ReadOnly))
	{
		QList<QByteArray> parts = kNames.readAll().split(',');
		QList<QByteArray> small_parts;
		for(int i=parts.size()-1;i>=0;i--)
		{
			small_parts = parts[i].split('=');
			bool ok;
			m_keyNames[small_parts[1].toUInt(&ok,16)] = small_parts[0];
		}
		kNames.close();
	}
	connect(this,SIGNAL(itemSelected(QString)),this,SLOT(Selection(QString)));
	
	//QFile pF("controls.cfg");
	if (pF.open(QFile::WriteOnly))
	{
		QDataStream DS(&pF);
		DS<<butUp<<butDown<<butLeft<<butRight<<butA<<butB<<butC<<butStart;
		pF.close();
	}
	
	m_JoyTex = MGR->bindTexture(QImage(QDir::currentPath()+"/res/backs/controls_back.png"),
										 GL_TEXTURE_2D,
									    GL_RGBA,
									    QGLContext::InvertedYBindOption);
	if (!s_KeysFont)
		s_KeysFont = new custom_font_s("/res/fonts/controls_font.png");
}
void cControllsMenu::keyPressed(int key)
{
	if (!m_Visible) return;
	
	if (!m_Ready) return;
	
	if (!m_Wait)
		cBaseMenu::keyPressed(key);
	else
	{
		m_NewKeys[m_Cursor] = key;
		m_Wait = false;
		return;
	}
	if (key == butA || key == butStart)
	{
		if (m_Cursor <= 7)
		{
			m_Wait = true;
		}
	}
}

void cControllsMenu::Selection(QString item)
{
	if (item == "#OK")
	{
		int *keys[] = 
		{
			&butUp,
			&butLeft,
			&butDown,
			&butRight,
			&butStart,
			&butC,
			&butB,
			&butA  
		};
		for(int i=0;i<8;i++)
		{
			*keys[i] = m_NewKeys[i];
		}
		QFile pF("controls.cfg");
		if (pF.open(QFile::WriteOnly))
		{
			QDataStream DS(&pF);
			DS<<butUp<<butDown<<butLeft<<butRight<<butA<<butB<<butC<<butStart;
			pF.close();
		}
	}
	else if (item == "#Cancel")
	{
		int *keys[] = 
		{
			&butUp,
			&butLeft,
			&butDown,
			&butRight,
			&butStart,
			&butC,
			&butB,
			&butA  
		};
		for(int i=0;i<8;i++)
		{
			m_NewKeys[i] = *keys[i];
		}
	}
}
void cControllsMenu::RenderMe(void)
{
	MGR->m_pCurrentRoom->ObjectByTargetname("cur_l")->m_Visible = false;
	MGR->m_pCurrentRoom->ObjectByTargetname("cur_r")->m_Visible = false;
	
	//MGR->drawTexture(QPointF(0,0),m_JoyTex);
	glBindTexture(GL_TEXTURE_2D,m_JoyTex);
	glBegin(GL_QUADS);
	glTexCoord2f(0,1); glVertex2f(0,0);
	glTexCoord2f(1,1); glVertex2f(640,0);
	glTexCoord2f(1,0); glVertex2f(640,480);
	glTexCoord2f(0,0); glVertex2f(0,480);
	glEnd();
	
	
	TEXT_ALIGN align = ETA_LEFT;
	for(int i=0;i<8;i++)
	{
		if (m_KeyOrigs[i].x() < 320) 
			align = ETA_RIGHT;
		else 
			align = ETA_LEFT;
		if (i == m_Cursor)
		{
			if (m_Wait)
			{
				if (int(MGR->m_Time*100)%50 < 25 )
					glColor3f(1.0f,1.0f,0);
				else glColor3f(0.9f,0.51f,0);
			}
			else
				glColor3f(1.0f,1.0f,0);
		}
		else glColor3f(0.9f,0.51f,0);
		MGR->drawTextCustom(m_KeyOrigs[i].x(),m_KeyOrigs[i].y()-24,s_Locale[m_Items[i]],s_KeysFont,align);
	}
	for(int i=9;i<=10;i++)
	{
		if (i == m_Cursor) glColor3f(1,1,1);
		else glColor3f(0.75,0.75,0.75);
		
		MGR->drawTextCustom(541,250+40*(i-9),s_Locale[m_Items[i]],s_pMenuFont,ETA_CENTER);
	}
	if (m_Cursor <= 7)
	{
		if (!m_Wait || (int(MGR->m_Time*100)%50 < 25 ))
		{
			glColor3f(1,1,1);
			MGR->drawTexture(m_TexOrigs[m_Cursor].toPointF(),m_ButTexs[m_Cursor]);
		}
	}
	
	//MGR->drawTextCustom(541,290,m_Items[10],s_pMenuFont,ETA_CENTER);
	for(int i=0;i<8;i++)
	{
		if (m_KeyOrigs[i].x() < 320) 
			align = ETA_RIGHT;
		else 
			align = ETA_LEFT;
		if (i == m_Cursor)
		{
			if (m_Wait)
			{
				if (int(MGR->m_Time*100)%50 < 25 )
					glColor3f(1,1,1);
				else glColor3f(0.75,0.75,0.75);
			}
			else
				glColor3f(1,1,1);
		}
		else glColor3f(0.75,0.75,0.75);
		MGR->drawTextCustom(m_KeyOrigs[i].x(),m_KeyOrigs[i].y()-18,m_keyNames[m_NewKeys[i]].toLatin1(),s_pMenuFont,align);
	}
	glColor3f(1,1,1);
//	if (!m_pFont) return;
	
//	MGR->drawFontPush(m_pFont);
	
//	QFontMetrics fm(*m_pFont);
//	int sz = m_Items.size();
//	int h = fm.height()+2;
//	int w;
	
//	//glColor4f(0.f,0.f,0.f,0.5f);
//	glBegin(GL_QUADS);
//	glVertex2f(m_Orig.x()-m_MaxWidth/2-48,m_Orig.y());
//	glVertex2f(m_Orig.x()+m_MaxWidth/2+48,m_Orig.y());
//	glVertex2f(m_Orig.x()+m_MaxWidth/2+48,m_Orig.y()+m_Items.size()*h+16);
//	glVertex2f(m_Orig.x()-m_MaxWidth/2-48,m_Orig.y()+m_Items.size()*h+16);
//	glEnd();

//	int maxW = 0;
//	for(int i=0;i<sz;i++)
//	{
//		QString str = m_Items[i];
//		if (i < 8)
//			str += "   -   "+m_keyNames[m_NewKeys[i]];
//		if (maxW < fm.width(str))
//			maxW = fm.width(str);
//		if (i == m_Cursor)
//		{
//			cBaseObject *curL, *curR;
//			curL = MGR->m_pCurrentRoom->ObjectsByTargetname("cur_l")[0];
//			curR = MGR->m_pCurrentRoom->ObjectsByTargetname("cur_r")[0];
			
//			QVector2D vL, vR;
//			vL[Y] = m_Orig.y()+i*h+h-8.f;
//			vR[Y] = vL[Y];
//			vL[X] = m_Orig.x()-fm.width(str)/2-24;
//			vR[X] = m_Orig.x()+fm.width(str)/2+24;
			
//			for(int j=0;j<2;j++)
//			{
//				curL->m_Velocity[j] = min(8.f,max(vL[j]-curL->m_Orig[j],-8.f));
//				curR->m_Velocity[j] = min(8.f,max(vR[j]-curR->m_Orig[j],-8.f));
//			}
//			//qDebug()<<"curL"<<curL<<"m_Velocity"<<curL->m_Velocity;
//		}
//		if (m_Align == ALIG_LEFT)
//			w = 0;
//		else
//			w = -fm.width(str);
//		if (m_Align == ALIG_CENTER) w >>= 1;
//		else if (m_Align == ALIG_RIGHT) w = -w;
//		MGR->drawColorPush(0,0,0);
//		for(int n = -1;n<=1;n++)
//		for(int m = -1;m<=1;m++)
//		{
//			MGR->drawText(
//						m_Orig.x()+w+n*2,
//						m_Orig.y()+i*h+h+m*2,str);
//		}
//		MGR->drawColorPop();
//		if (m_Wait)
//		{
//			if (m_Cursor == i)
//			{
//				if ((QDateTime::currentDateTime().time().msec() % 500) < 250)
//					MGR->drawColorPush(255,255,255);
//				else
//					MGR->drawColorPush(128,128,128);
//			}
//			else
//				MGR->drawColorPush(255,255,255);
//		}
//		else
//			MGR->drawColorPush(255,255,255);
//		MGR->drawText(m_Orig.x()+w,m_Orig.y()+i*h+h,str);
//		MGR->drawColorPop();
//	}
//	m_MaxWidth = maxW;
//	MGR->drawFontPop();
}
