#include <cMainManager.h>
#include "triggers.h"
#include <cGameRoom.h>
#include <QDebug>
#include <math.h>

#include <QDir>
#include <QFile>
#include <util.h>

extern QMap<int, quint8> gKeysMap;


//========TELEPORT=========
void cTriggerTeleport::Touching(cBaseObject *pOther)
{
	if (!m_Target.isEmpty())
	{
		if (m_TargetName.isEmpty())
		{
			cBaseObject *pTarg = m_pMyRoom->ObjectByTargetname(m_Target);
			if (pTarg)
			{
				pOther->m_Orig = pTarg->m_Orig;
				pOther->m_Velocity = QVector2D(0,0);
			}
		}
	}
}
void cTriggerTeleport::ParseKeyValue(const QString &key, const QString &value)
{
	if (key == "target")
	{
		m_Target = value;
	}
	else
		cBaseTrigger::ParseKeyValue(key,value);
}

void cTriggerTeleport::Use(cBaseObject *pOther)
{
	if (!m_TargetName.isEmpty())
	{
		cBaseObject *pTarg = m_pMyRoom->ObjectByTargetname(m_Target);
		if (pTarg)
		{
			pOther->m_Orig = pTarg->m_Orig;
			
			pOther->m_Velocity = QVector2D(0,0);
		}
	}
}


//========DOOR=========
QString g_LandmarkName;
QVector2D g_LandmarkOffs;
int cTriggerDoor::s_IconLocked = 0;
int cTriggerDoor::s_IconLeft   = 0;
int cTriggerDoor::s_IconUp     = 0;
custom_font_s *cTriggerDoor::s_LocsFont = 0;
QMap<QString,QByteArray> cTriggerDoor::s_LocsLocale;
QVector<cBaseObject*> g_TrasnferList;
QVector<cBaseObject*> g_TrasnferedList;

cTriggerDoor::cTriggerDoor(QObject *parent) : cBaseTrigger(parent)
{
	//m_Arrow = NULL;
	if (!cTriggerDoor::s_LocsFont) 
	{
		cTriggerDoor::s_LocsFont = new custom_font_s("/res/fonts/locs_font.png");
		
		cTriggerDoor::s_IconLocked = MGR->bindTexture(
												  QImage(QDir::currentPath()+"/res/sprites/hud/loc_arrow_blocked.png"),
												  GL_TEXTURE_2D,
												  GL_RGBA,
												  QGLContext::NoBindOption);
		cTriggerDoor::s_IconLeft = MGR->bindTexture(
												  QImage(QDir::currentPath()+"/res/sprites/hud/loc_arrow_left.png"),
												  GL_TEXTURE_2D,
												  GL_RGBA,
												  QGLContext::NoBindOption);
		cTriggerDoor::s_IconUp = MGR->bindTexture(
												  QImage(QDir::currentPath()+"/res/sprites/hud/loc_arrow_up.png"),
												  GL_TEXTURE_2D,
												  GL_RGBA,
												  QGLContext::NoBindOption);
		//qDebug()<<"s_IconLeft"<<s_IconLeft;
		
		QFile locs("locale/locations_"+g_Language+".txt");
		if (locs.open(QFile::ReadOnly))
		{
			while(!locs.atEnd())
			{
				QByteArray info = locs.readLine();
				QList<QByteArray> info_parts = info.split(';');
				info_parts[1].resize(info_parts[1].size()-1);
				s_LocsLocale[info_parts[0]] = info_parts[1];
			}
			locs.close();
		}
	}
	m_Visible = false;
}

cTriggerDoor::~cTriggerDoor()
{
//	if (m_Arrow) m_pMyRoom->DeleteObject(m_Arrow);
}

//void cTriggerDoor::ScrollMe(const QVector2D &scrll)
//{
//	cBaseTrigger::ScrollMe(scrll);
	
	
//}
void cTriggerDoor::Spawn()
{
	cBaseTrigger::Spawn();
	
//	m_Arrow = (cBaseAnimating*)m_pMyRoom->CreateObject("base_anim");
//	if (m_TransferKey == butLeft || m_TransferKey == butRight)
//	{
//		m_Arrow->AddAnim("res/sprites/hud/loc_arrow_left.png","idle",10);
//		if (m_TransferKey == butRight)
//			m_Arrow->m_ImageScale[0] = -1;
//	}
//	if (m_TransferKey == butUp || m_TransferKey == butDown)
//	{
//		m_Arrow->AddAnim("res/sprites/hud/loc_arrow_up.png","idle",10);
//		if (m_TransferKey == butDown)
//			m_Arrow->m_ImageScale[1] = -1;
//	}
//	m_Arrow->SetAnim("idle");
//	m_Arrow->m_Visible = false;
//	m_Arrow->m_ImageSpeed = 0.3f;
	
	m_pMyRoom->ChangeObjectDepth(this,INT_MIN+1);
//	m_pMyRoom->ChangeObjectDepth(m_Arrow,INT_MIN-1);
}

void cTriggerDoor::Step()
{
	cBaseTrigger::Step();
	
//	QVector2D orig = GetCenter();
//	if (orig.x() < 32)
//		m_Arrow->m_Orig[0] = orig.x() + 32;
//	else if (orig.x() > 640-32)
//		m_Arrow->m_Orig[0] = orig.x() + 640-32;
//	else
//		m_Arrow->m_Orig[0] = orig.x();
//	m_Arrow->m_Orig[1] = orig.y()-64;
	
//	if (!m_Locked)
//		m_Arrow->m_Visible = m_Visible;
//	else
//		m_Arrow->m_Visible = false;
}
void cTriggerDoor::Touching(cBaseObject *pOther)
{
	if (pOther->IsPlayer())
	{
		//m_Arrow->m_Visible = true;
		m_Visible = true;
		
		if (!m_Locked && UTIL_keyState(m_TransferKey) & KEY_PRESSED)
		{
			if (!m_Landmark.isEmpty())
			{
				cBaseObject *pEnt = m_pMyRoom->ObjectByTargetname(m_Landmark);
				//qDebug()<<"pEnt"<<pEnt<<m_Landmark;
				if (pEnt)
				{
					g_LandmarkName = m_Landmark;
					g_TrasnferList.push_back(pOther);
					pOther->m_Flags |= FL_TRANSFER;
					
					pOther->m_Velocity = QVector2D(0,0);
					pOther->m_Orig -= pEnt->m_Orig;
					qDebug()<<"saved orig"<<pOther->m_Orig<<g_LandmarkName;
					MGR->DisableInput();
					if (!m_NextMap.contains(".2dl"))
						MGR->Changelevel(m_NextMap+".2dl",true);
					else
						MGR->Changelevel(m_NextMap,true);
				}
			}
			else if (!m_Target.isEmpty())
			{
				gKeysMap[m_TransferKey] |= KEY_ISUP;
				gKeysMap[m_TransferKey] &= ~KEY_ISDOWN;
				gKeysMap[m_TransferKey] &= ~KEY_PRESSED;
				UTIL_UseTargets(m_pMyRoom,m_Target,pOther);
			}
		}
	}
}
void cTriggerDoor::RenderMe()
{
	//qDebug()<<"door active"<<m_Mins<<m_Maxs;
	
	glColor3f(1,1,1);
	QVector2D orig = GetCenter();
	if (orig.x() < 32)
	{
		orig[0] = 32;
		MGR->drawTextCustom(GetCenter().x(),GetCenter().y()-48,s_LocsLocale[m_NextLocName],cTriggerDoor::s_LocsFont,ETA_LEFT);
	}
	else if (orig.x() > 640-32)
	{
		orig[0] = 640-32;
		MGR->drawTextCustom(GetCenter().x(),GetCenter().y()-48,s_LocsLocale[m_NextLocName],cTriggerDoor::s_LocsFont,ETA_RIGHT);
	}
	else
		MGR->drawTextCustom(GetCenter().x(),GetCenter().y()-48,s_LocsLocale[m_NextLocName],cTriggerDoor::s_LocsFont,ETA_CENTER);
	
	orig[1] -= 64;
	
	if (m_Locked)
	{
		MGR->drawTexture(orig.toPointF()-QPointF(15.f,15.f),s_IconLocked);
	}
	else
	{
		float add = 4.f*cos(MGR->m_Time*10.f);
		if (m_TransferKey == butLeft || m_TransferKey == butRight)
		{
			glBindTexture(GL_TEXTURE_2D,cTriggerDoor::s_IconLeft);
			if (m_TransferKey == butRight)
			{
				glBegin(GL_QUADS);
				glTexCoord2f(1.f,0.f); glVertex2f(orig.x()-17.f+add,orig.y()-17.f);
				glTexCoord2f(1.f,1.f); glVertex2f(orig.x()-17.f+add,orig.y()+17.f);
				glTexCoord2f(0.f,1.f); glVertex2f(orig.x()+17.f+add,orig.y()+17.f);
				glTexCoord2f(0.f,0.f); glVertex2f(orig.x()+17.f+add,orig.y()-17.f);
				glEnd();
			}
			else
			{
				glBegin(GL_QUADS);
				glTexCoord2f(0.f,0.f); glVertex2f(orig.x()-17.f+add,orig.y()-17.f);
				glTexCoord2f(0.f,1.f); glVertex2f(orig.x()-17.f+add,orig.y()+17.f);
				glTexCoord2f(1.f,1.f); glVertex2f(orig.x()+17.f+add,orig.y()+17.f);
				glTexCoord2f(1.f,0.f); glVertex2f(orig.x()+17.f+add,orig.y()-17.f);
				glEnd();
			}
		}
		if (m_TransferKey == butUp || m_TransferKey == butDown)
		{
			glBindTexture(GL_TEXTURE_2D,cTriggerDoor::s_IconUp);
			if (m_TransferKey == butDown)
			{
				glBegin(GL_QUADS);
				glTexCoord2f(0.f,1.f); glVertex2f(orig.x()-17.f,orig.y()-17.f+add);
				glTexCoord2f(0.f,0.f); glVertex2f(orig.x()-17.f,orig.y()+17.f+add);
				glTexCoord2f(1.f,0.f); glVertex2f(orig.x()+17.f,orig.y()+17.f+add);
				glTexCoord2f(1.f,1.f); glVertex2f(orig.x()+17.f,orig.y()-17.f+add);
				glEnd();
			}
			else
			{
				glBegin(GL_QUADS);
				glTexCoord2f(0.f,0.f); glVertex2f(orig.x()-17.f,orig.y()-17.f+add);
				glTexCoord2f(0.f,1.f); glVertex2f(orig.x()-17.f,orig.y()+17.f+add);
				glTexCoord2f(1.f,1.f); glVertex2f(orig.x()+17.f,orig.y()+17.f+add);
				glTexCoord2f(1.f,0.f); glVertex2f(orig.x()+17.f,orig.y()-17.f+add);
				glEnd();
			}
		}
		
	}
	m_Visible = false;
}
void cTriggerDoor::ParseKeyValue(const QString &key, const QString &value)
{
	if (key == "next_map")
	{
		m_NextMap = value;
	}
	else if (key == "next_location")
	{
		m_NextLocName = value;
	}
	else if (key == "landmark")
	{
		m_Landmark = value;
	}
	else if (key == "transfer_key")
	{
		if (value == "left")
			m_TransferKey = butLeft;
		else if (value == "right")
			m_TransferKey = butRight;
		else if (value == "down")
			m_TransferKey = butDown;
		else //if (value == "up")
			m_TransferKey = butUp;
		//qDebug()<<"m_TransferKey"<<value;
	}
	else if (key == "locked")
	{
		if (value == "FALSE") m_Locked = false;
		else m_Locked = true;
	}
	else
		cBaseTrigger::ParseKeyValue(key,value);
}

void cTriggerOnce::Touching(cBaseObject *pOther)
{
	if (m_Used) return;
	m_Used = true;
	UTIL_UseTargets(m_pMyRoom,m_Target,pOther);
	m_pMyRoom->DeleteObject(this);
}
