#include "cBaseAnimating.h"
#include "cMainManager.h"
#include "cSprite.h"
#include "util.h"

#include <QFile>

cBaseAnimating::cBaseAnimating(QObject *parent) :
	cBaseObject(parent)
{
	m_AnimName.clear();
	m_pTexture = NULL;
}
void cBaseAnimating::AddAnim(
		QString filename, QString animname, int numframes, bool loop)
{
	if (m_Anims.keys().contains(animname)) return;
	
	m_ImageIndex = 0;
	m_pTexture = cSprite::getSprite(filename,numframes);
	if (!m_pTexture) return;
	
	m_Anims[animname] = 
			new anim_s(m_pTexture,loop);
	
	SetAnim(animname);
}
void cBaseAnimating::DeleteAnim(QString animname)
{
	if (!m_Anims.keys().contains(animname)) return;
	cSprite *pSpr = m_Anims[animname]->spr;
	delete m_Anims[animname];
	if (m_pTexture == pSpr)
		m_pTexture = NULL;
}
void cBaseAnimating::SetAnim(QString animname)
{
	if (!m_Anims.keys().contains(animname)) return;
	m_AnimName = animname;
	cSprite *pSpr = m_Anims[animname]->spr;
	if (m_pTexture != pSpr)
		m_ImageIndex = 0;
	m_pTexture = pSpr;
	m_Loop = m_Anims[animname]->loop;
}
void cBaseAnimating::RenderMe() {Draw();}

void cBaseAnimating::Spawn()
{
	if (!m_SpawnSprite.isEmpty())
	{
		AddAnim(m_SpawnSprite,"idle",m_SpawnNumFrames,m_Loop);
	}
}

void cBaseAnimating::EventAnimEnded()
{
	
}
void cBaseAnimating::Draw()
{
	if (!m_pTexture) return;
	
	float idxPrev = m_ImageIndex;
	m_ImageIndex = m_pTexture->SetFrame(m_ImageIndex+m_ImageSpeed*MGR->m_DeltaTime,m_Loop);
	if (!m_Loop)
	{
		if (m_ImageIndex == 0.f && idxPrev > 0.f)
			EventAnimEnded();
		if (m_ImageIndex == m_pTexture->m_numFrames-1 && idxPrev < m_pTexture->m_numFrames-1)
			EventAnimEnded();
	}
	cBaseObject::Draw();
}
void cBaseAnimating::ParseKeyValue(const QString &key, const QString &value)
{
	if (key == "sprite_file")
	{
		if (value.right(4) == ".png" || value.right(4) == ".ang")
		{
			m_SpawnSprite = value.right(value.size()-1);
		}
		else if (value.right(4) == ".txt")
		{
			QFile animFile(value);
			if (animFile.open(QFile::ReadOnly))
			{
				while(!animFile.atEnd())
				{
					QByteArray animName = animFile.readLine();
					QByteArray sprite   = animFile.readLine();
					int numframes       = animFile.readLine().toInt();
					bool loop           = animFile.readLine().toInt();
					AddAnim(sprite,animName,numframes,loop);
				}
				animFile.close();
			}
		}
	}
	else if (key == "x_scale")
	{
		m_ImageScale[0] = value.toFloat();
	}
	else if (key == "y_scale")
	{
		m_ImageScale[1] = value.toFloat();
	}
	else if (key == "num_frames")
	{
		m_SpawnNumFrames = value.toInt();
	}
	else if (key == "loop")
	{
		if (value == "FALSE") m_Loop = false;
		else m_Loop = true;
	}
	else if (key == "image_speed")
	{
		m_ImageSpeed = value.toFloat();
	}
	else if (key == "image_alpha")
	{
		m_ImageAlpha = value.toFloat();
	}
	else if (key == "start_frame")
	{
		m_ImageIndex = value.toInt();
	}
	else
		cBaseObject::ParseKeyValue(key,value);
}
