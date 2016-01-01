#include "mainwindow.h"

#include <cGameRoom.h>
#include "cBigDialog.h"
#include <QString>
#include <QFont>
#include <QFontMetrics>

#include <qstringlist.h>
#include <cMainManager.h>
#include <util.h>

#include <QDebug>
#include <QDateTime>
#include <QDir>

extern QMap<int, quint8> gKeysMap;

//QFont *g_pBigDialogFont;
int gAdvArrow, gBack, gIconOver;
#define BIG_FONT_H 19

custom_font_s *g_pBigDialogFont, *g_pBigDialogNameFont;

int cBigDialog::s_MissingTex = 0;
QMap<QByteArray,QByteArray> cBigDialog::s_NamesLocale;

cBigDialog::cBigDialog(QObject *parent) : 
	QObject(parent)
{
	QFile locs("locale/names_"+g_Language+".txt");
	if (locs.open(QFile::ReadOnly))
	{
		while(!locs.atEnd())
		{
			QByteArray info = locs.readLine();
			QList<QByteArray> info_parts = info.split(';');
			info_parts[1].resize(info_parts[1].size()-1);
			s_NamesLocale[info_parts[0]] = info_parts[1];
		}
		locs.close();
	}
	
	m_Visible = false;
	
	//m_AdvTimer.setInterval(100);
	m_AdvTimer.setSingleShot(false);
	connect(&m_AdvTimer,SIGNAL(timeout()),this,SLOT(AdvanceDialog()));
	m_AdvTimer.start(100);
	m_ToAdvance = false;
	m_Auto = false;
	m_ClearUp = false;
	m_CurDlgPart = NULL;

	if (!gAdvArrow)
	{
		g_pBigDialogFont = new custom_font_s("/res/fonts/dialog_font.png");
		g_pBigDialogNameFont = new custom_font_s("/res/fonts/dialog_font_bold.png");
		
		gAdvArrow = MGR->bindTexture(QImage("res/sprites/hud/dialog_big/adv_arrow.png"),
											  GL_TEXTURE_2D,
											  GL_RGBA,
											  QGLContext::InvertedYBindOption);
		gBack     = MGR->bindTexture(QImage("res/sprites/hud/dialog_big/back.png"),
											  GL_TEXTURE_2D,
											  GL_RGBA,
											  QGLContext::InvertedYBindOption);
		gIconOver = MGR->bindTexture(QImage("res/sprites/hud/dialog_big/icon_overlay.png"),
											  GL_TEXTURE_2D,
											  GL_RGBA,
											  QGLContext::InvertedYBindOption);
		s_MissingTex = MGR->bindTexture(QImage("res/sprites/big_icons/missing.png"),
												  GL_TEXTURE_2D,
												  GL_RGBA,
												  QGLContext::InvertedYBindOption);
	}
}
void cBigDialog::AddDialog(int talker_tex, 
		QByteArray name,
		QByteArray text,
		bool autoadvance,
		bool start_now, bool draw_left)
{
	
	
	if (!m_DialogParts.size())
		start_now = true;
	
	if (start_now)
	{
		while(!m_DialogParts.empty())
		{
			delete m_DialogParts.at(0);
			m_DialogParts.pop_front();
		}
		m_Lines.clear();
		m_DrawY = 0;
		m_CurLine.clear();
		m_ToAdvance = false;
		m_Auto = false;
		m_Visible = true;
		//AdvanceDialog();
		//emit PhraseStarts();
		m_AdvTimer.setInterval(0);
	}
	dialog_part_s *part = new dialog_part_s;
	part->autoadv = autoadvance;
	part->talker_tex = talker_tex;
	part->talker_name = name;
	part->text = text;
	part->draw_left = draw_left;
	m_DialogParts.push_back(part);
	
	
	//DBG_PRINT("Dialog added...");
}
void cBigDialog::KeyPressed(int key)
{
}
void cBigDialog::KeyReleased(int key)
{

}
bool cBigDialog::IsSkipping(dialog_part_s *part)
{
	if (!part->autoadv)
		//return part->fastforward;
		if (
			(UTIL_keyState(butA) & KEY_ISDOWN) || 
			(UTIL_keyState(butB) & KEY_ISDOWN) || 
			(UTIL_keyState(butC) & KEY_ISDOWN))
			{
				return true;
			}
	return false;
}
void cBigDialog::AdvanceDialog()
{
//	DBG_PRINT("AdvanceDialog");
	
	if (!m_Visible	) return;
	
	m_AdvTimer.setInterval(56);
	
	if (m_ToAdvance)
	{
		m_AdvTimer.setInterval(2);
		
		int key = 0;
		if (!m_Auto)
		{
			if (UTIL_keyState(butA) & KEY_PRESSED) key = butA;
			if (UTIL_keyState(butB) & KEY_PRESSED) key = butB;
			if (UTIL_keyState(butC) & KEY_PRESSED) key = butC;
		}
		
		if (key || m_Auto)
		{
			m_Lines.clear();
			m_CurLine.clear();
			m_ToAdvance = false;
			
			if (key)
			{
				gKeysMap[key] |=  KEY_ISUP;
				gKeysMap[key] &= ~KEY_ISDOWN;
			}
			m_Auto = false;
		}
		return;
	}
//	if (m_Auto)
//	{
//		m_Lines.clear();
//		//m_CurLine.clear();
//		m_Auto = false;
//	}
	
	dialog_part_s *pPart = m_DialogParts.at(0);
	if (m_CurDlgPart != pPart)
		m_AdvTimer.setInterval(800);
	m_CurDlgPart = pPart;
	m_Auto       = pPart->autoadv;
	m_TalkerTex  = pPart->talker_tex;
	m_TalkerName = pPart->talker_name;
	//qDebug()<<"m_TalkerName"<<m_TalkerName;
	m_Left		 = pPart->draw_left;
	
	if (pPart->text.isEmpty() && m_CurWord.isEmpty()) //кончился текст и не печатаем слово
	{
		if (m_Lines.size() || !m_CurLine.isEmpty()) //в диалоге виден текст
		{
			m_ToAdvance = true;
			return;
		}
		m_DialogParts.removeAt(0); 
		if (!m_DialogParts.size()) //кончились части диалога
		{
			m_Visible = false;
			emit DialogEnds();
			return; //EXIT
		}
		else
		{
			if (m_Auto)// || !IsSkipping(pPart))
				m_AdvTimer.setInterval(1200);
		}
	}
	else //текст не кончился ИЛИ печатаем слово
	{
		if (m_CurWord.isEmpty()) //не печатаем слово
		{
			QList<QByteArray> words = pPart->text.split(' ');
			if (words.size())
			{
				//QFontMetrics fm(*g_pBigDialogFont);
				m_CurWord = words[0]+" ";
				//qDebug()<<m_CurWord;
				if (/*fm.width(m_CurWord)*/g_pBigDialogFont->strWidth(words[0]+" ") > 640-128)
				{
					DBG_PRINT_S("WARNING: Word \""+m_CurWord+"\" is too long! Skipping");
					pPart->text.remove(0,m_CurWord.size());//pPart->text.resize(pPart->text.size() - m_CurWord.size());
					return;
				}
				bool fitting = true;
				
				int width = g_pBigDialogFont->strWidth(m_CurLine+m_CurWord);
				if (width >= (640-128))
				{
					if (m_Lines.size() >= 2)
						fitting = false;
					else
					{
						m_Lines.push_back(m_CurLine);
						m_CurLine.clear();
						m_CurWord.clear();
					}
				}
				if (!fitting)
				{
					//DBG_PRINT_S("INFO: doesn't fit");
					m_CurWord.clear();
					m_ToAdvance = true;
					if (m_Auto)
						m_AdvTimer.setInterval(1200);
					return;
				}
				pPart->text.remove(0,m_CurWord.size());
				//pPart->text.resize(pPart->text.size() - m_CurWord.size());
			}
		}
		else //печатаем слово
		{
			//взять одну букву из слова
			uchar ch = m_CurWord[0];
			m_CurWord.remove(0,1);
			
			//напечатать её в строку
			m_CurLine += ch;
			
//			if (IsSkipping(pPart)) //быстрый текст
//				m_AdvTimer.setInterval(0);
//			else
//			{
				//доп. задержка на знаках припинания
				if (ch == ' ') m_AdvTimer.setInterval(0);
				if (ch == ',') m_AdvTimer.setInterval(300);
				else if (ch == '.' || ch == '!' || ch == '?') m_AdvTimer.setInterval(500);
//			}
		}
		if (IsSkipping(pPart)) //быстрый текст
			m_AdvTimer.setInterval(0);
	}
}
void cBigDialog::RenderDialog()
{
	if (!m_Visible) return;
	
	//TODO:
	//draw black quad at the bottom of the screen,
	//glColor3f(0,0,0);
	
	glColor3f(1,1,1);
	
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,gBack);
	
	glBegin(GL_QUADS);
	if (m_Left)
	{
		glTexCoord2f(0.f,1.f);
		glVertex2f(  0.f, 0.f);
		
		glTexCoord2f(1.f,1.f);
		glVertex2f(640.f, 0.f);
		
		glTexCoord2f(1.f,0.f);
		glVertex2f(640.f,109.f);
		
		glTexCoord2f(0.f,0.f);
		glVertex2f(  0.f,109.f);
	}
	else
	{
		glTexCoord2f(1.f,1.f);
		glVertex2f(  0.f, 0.f);
		
		glTexCoord2f(0.f,1.f);
		glVertex2f(640.f, 0.f);
		
		glTexCoord2f(0.f,0.f);
		glVertex2f(640.f,109.f);
		
		glTexCoord2f(1.f,0.f);
		glVertex2f(  0.f,109.f);
	}
	glEnd();
	
	//glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D,m_TalkerTex);
	
	//talker texture quad on the left of it
	//and current text to the right of talker
	int textX = 124;
	int nameX = 363;
	if (!m_Left)
	{
		textX -= 109;
		nameX -= 109;
		MGR->drawTexture(QRectF(640.f-109,0.f,109,109),m_TalkerTex);
		MGR->drawTexture(QRectF(640.f,0.f,-109,109),gIconOver);
	}
	else
	{
		MGR->drawTexture(QRectF(0,0,109,109),m_TalkerTex);
		MGR->drawTexture(QRectF(0,0,109,109),gIconOver);
	}
	
	//MGR->drawFontPush(g_pBigDialogFont);
	int i=0;
	for(;i<m_Lines.size();i++)
	{
		//MGR->drawText(textX,(i+1)*(BIG_FONT_H+8),m_Lines[i]);
		MGR->drawTextCustom(textX,(i-1)*(g_pBigDialogFont->height)+55,m_Lines[i],g_pBigDialogFont);
	}

	//MGR->drawText(textX,(i+1)*(BIG_FONT_H+8),m_CurLine);
	MGR->drawTextCustom(textX,(i-1)*(g_pBigDialogFont->height)+55,m_CurLine,g_pBigDialogFont);
	
	MGR->drawTextCustom(nameX, 2,m_TalkerName,g_pBigDialogNameFont,ETA_CENTER);
	
	//MGR->drawFontPop();
	if (m_ToAdvance && !m_Auto) //Draw manual advance arrow if flagged
	{
		if ((QDateTime::currentDateTime().time().msec() % 1000) < 500)
			MGR->drawTexture(QPointF(textX+(640-109-40),109-16),gAdvArrow);
	}
}
void cBigDialog::AddDialog(QString talkertex_fname, QByteArray name, QByteArray text, bool autoadvance, bool start_now, bool draw_left)
{
	
	
	int iTex = s_MissingTex;
	if (!talkertex_fname.isEmpty())
	{
		QImage tex(talkertex_fname);
		
		if (!tex.isNull())
			iTex = MGR->bindTexture(tex);
	}
	AddDialog(iTex,
				 name,
				 text,
				 autoadvance,
				 start_now,draw_left);
}

//======================================================================\\
//                                                                      \\
//======================================================================\\

cInfoDialog::cInfoDialog(QObject *parent) : cBaseObject(parent)
{
	
}
void cInfoDialog::Use(cBaseObject *pOther)
{
	for(int i=0;i<m_DialogParts.size();i++)
	{
		qDebug()<<"m_DialogParts[i].talker_name"<<m_DialogParts[i].talker_name;
		g_pBigDialog->AddDialog(
					m_DialogParts[i].talker_texname,
					m_DialogParts[i].talker_name,
					m_DialogParts[i].text,
					m_DialogParts[i].autoadv,
					false,
					m_DialogParts[i].draw_left);
	}
	connect(g_pBigDialog,SIGNAL(DialogEnds()),this,SLOT(DialogDone()));
	pOther->m_Velocity = QVector2D(0,0);
}

void cInfoDialog::DialogDone()
{
	QList<cBaseObject*>targets = m_pMyRoom->ObjectsByTargetname(m_Target);
	for(int i=0;i<targets.size();i++)
		targets[i]->Use(this);
	m_Flags |= FL_KILLME;
}
void cInfoDialog::ParseKeyValue(const QString &key, const QString &value)
{
	if (key == "dialog_file")
	{
		QString filename = value+"_"+g_Language+".txt";
		QFile dlgFile(QDir::currentPath()+"/"+filename);
		if (dlgFile.open(QFile::ReadOnly))
		{
			bool autoadv = false;
			if (dlgFile.readLine() == "AUTO") autoadv = true;

			while(!dlgFile.atEnd())
			{
				dialog_part_s part;
				part.autoadv     = autoadv;
				part.draw_left   = (dlgFile.readLine().simplified() == "LEFT");
				part.talker_name =  dlgFile.readLine().simplified();
				part.talker_texname = QDir::currentPath()+"/res/sprites/big_icons/"+
										part.talker_name.right(part.talker_name.size()-1)+"/"+
										  dlgFile.readLine().simplified()+".png";

				part.text			= dlgFile.readLine().simplified();
				qDebug()<<"part.talker_name"<<part.talker_name;
				if (cBigDialog::s_NamesLocale.contains(part.talker_name))
					part.talker_name = cBigDialog::s_NamesLocale[part.talker_name];
				qDebug()<<"part.talker_name"<<part.talker_name;
				m_DialogParts.push_back(part);
			}
			dlgFile.close();
		}
	}
	else
		cBaseObject::ParseKeyValue(key,value);
}
