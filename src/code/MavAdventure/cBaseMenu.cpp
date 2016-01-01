#include "cBaseMenu.h"
#include <cMainManager.h>
#include <util.h>

#include <QFile>
#include <QFontMetrics>
#include <QDir>
#include <QDebug>


QMap<QString,QByteArray> cBaseMenu::s_Locale;
custom_font_s *cBaseMenu::s_pMenuFont;

cBaseMenu::cBaseMenu(QObject *parent) :
	cBaseRenderable(parent)
{
	if (!s_pMenuFont)
	{
		s_pMenuFont = new custom_font_s("/res/fonts/mainmenu_font.png");
		//qDebug()<<"s_pMenuFont"<<s_pMenuFont;
		QFile locs("locale/menu_"+g_Language+".txt");
		if (locs.open(QFile::ReadOnly))
		{
			while(!locs.atEnd())
			{
				QByteArray info = locs.readLine();
				QList<QByteArray> info_parts = info.split(';');
				info_parts[1].resize(info_parts[1].size()-1);
				s_Locale[info_parts[0]] = info_parts[1];
			}
			locs.close();
		}
	}
	m_Ready = false;
	m_pFont = NULL;
	m_Align = ALIG_LEFT;
	m_Visible = false;
	m_Cursor = 0;
	connect(MGR,SIGNAL(keyPressed(int)),this,SLOT(keyPressed(int)));
	connect(MGR,SIGNAL(keyReleased(int)),this,SLOT(keyReleased(int)));
}

cBaseMenu::~cBaseMenu()
{
	if (m_pFont) delete m_pFont;
	m_pFont = NULL;
}
void cBaseMenu::keyPressed(int key)
{
	if (m_pMyRoom != MGR->m_pCurrentRoom) return;
	
	if (!m_Visible) return;
		
	if (m_Items.size() <= 1) return;
	
	if (key == butUp)
	{
		m_Cursor--;
		WarpCursor();
		while(m_Items[m_Cursor] == " ")
		{
			m_Cursor--;
			WarpCursor();
		}
	}
	if (key == butDown)
	{
		m_Cursor++;
		WarpCursor();
		while(m_Items[m_Cursor] == " ")
		{
			m_Cursor++;
			WarpCursor();
		}
		
	}
	if (key == butStart)
	{
		if (!m_Ready) return;
		
		DBG_PRINT(("Selected "+m_Items[m_Cursor]));
		emit itemSelected(m_Items[m_Cursor]);
	}
}

void cBaseMenu::keyReleased(int key)
{
	if (m_Visible)
	{
		//qDebug()<<"m_Ready"<<m_Ready;
		m_Ready = true;
	}
}
void cBaseMenu::RenderMe(void)
{
	//glColor3f(1.f,1.f,1.f);
	//MGR->drawTextCustom(10,10,"TEST",s_pMenuFont);//,ETA_LEFT);
	
	if (!m_Visible) 
	{
		m_Ready = false;
		return;
	}
	//if (!m_pFont) return;
	
	//MGR->drawFontPush(m_pFont);
	
	//QFontMetrics fm(*m_pFont);
	int sz = m_Items.size();
	int h = s_pMenuFont->height-2;//fm.height()+2;
	//int w;
	QByteArray str;
	for(int i=0;i<sz;i++)
	{
		if (!s_Locale.contains(m_Items[i]))
			str = m_Items[i];
		else
			str = s_Locale[m_Items[i]];
		
		MGR->drawTextCustom(m_Orig.x(),m_Orig.y()+i*h,str,s_pMenuFont,(TEXT_ALIGN)m_Align);
	}
	
	//MGR->drawFontPop();
}

void cBaseMenu::Step()
{
	if (!m_Visible) 
		m_Ready = false;
}
