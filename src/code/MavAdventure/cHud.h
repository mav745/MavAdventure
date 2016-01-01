#ifndef CHUD_H
#define CHUD_H

#include "cBaseAnimating.h"

class cSprite;

enum
{
	CHARA_MAV = 0,
	CHARA_HEL,
	CHARA_IZO,
	CHARA_FAR
};

class cHud : public cBaseAnimating
{
	Q_OBJECT
public:
	explicit cHud(QObject *parent = 0);
	
	cSprite *m_pCharaSign;
	cSprite *m_pCharaHealth;
	
	int m_CharaIdx;
	
	void Step(void);
	void Draw();
};

#endif // CHUD_H
