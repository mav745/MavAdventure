#ifndef CBASETALKER_H
#define CBASETALKER_H

#include "cBaseAnimating.h"

#include <QObject>

class cBaseWeapon;

enum TALKER_EMOTION
{
	ETE_NEUTRAL,
	ETE_SAD,
	ETE_HAPPY,
	ETE_SURPRISED,
	ETE_SHOCKED,
	ETE_THINKING,
	ETE_SURE,
	//TODO
	
	ETE_MAX
};

class cBaseTalker : public cBaseAnimating
{
	Q_OBJECT
	
public:
	explicit cBaseTalker(QObject *parent = 0);
	
	cBaseWeapon *m_pWeapon;
	int *m_Emotions;
	
	virtual void Talk(TALKER_EMOTION emote, const QString &text);
	virtual float getSpeed();
	~cBaseTalker();
};

#endif // CBASETALKER_H
