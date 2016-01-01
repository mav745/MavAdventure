#include "cBigDialog.h"
#include <QString>
#include <QFont>
#include <QFontMetrics>

QFont *g_pBigDialogFont;


cBigDialog::cBigDialog(QObject *parent) : 
	QObject(parent)
{
	if (!g_pBigDialogFont)
		g_pBigDialogFont = new QFont("Verdana", 32);
	m_Visible = false;
	
	m_AdvTimer.setInterval(200);
	m_AdvTimer.setSingleShot(false);
	//connect(&m_AdvTimer,SIGNAL(timeout),this,SLOT(AdvanceDialog));
}

//drawFontPush(new QFont("Arial", 10));

void cBigDialog::AddDialog(
		int talker_tex, 
		QString text,
		bool autoadvance,
		bool start_now)
{
	//QFontMetrics fm(*g_pBigDialogFont);
	//int width=fm.width(str);
	
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
	}
	dialog_part_s *part = new dialog_part_s;
	part->autoadv = autoadvance;
	part->talker_tex = talker_tex;
	part->text = text;
	m_DialogParts.push_back(part);
	//m_Auto = 
}

void cBigDialog::AdvanceDialog()
{
	if (!m_Visible) return;
	
	if (!m_DialogParts.size()) return;
	
	// check only once in a while (implement some timer or timestamp)
	
	
//	TODO:
//	take first char of remaining text (and remove it from it)
//	append it to the current_text
//	check current_text width to fit the area
//	if not
//	{
//		advance m_DrawY
//		{
//			if m_DrawY is too low
//			{
//				if manually advancing
//					flag manual advance state
//				else
//					add extra wait time (flag for reset)
//			}
//			else push new empty current_text
//		}
//	}
	
}

void cBigDialog::RenderDialog()
{
	//TODO:
	//draw black quad at the bottom of the screen,
	//talker texture quad on the left of it
	//and current text to the right of talker
	
	//Draw manual advance arrow if flagged
}
