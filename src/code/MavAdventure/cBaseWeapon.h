#ifndef CBASEWEAPON_H
#define CBASEWEAPON_H

#include "cBaseAnimating.h"

class cBaseTalker;

enum WEAPON_ID
{
	EWI_UNKNOWN = 0,
	
	EWI_M_GAUSS,
	EWI_M_SKIMMER,
	EWI_H_CROSSBOW,
	EWI_I_GAUSS,
	EWI_F_BLASTER,
	
	EWI_GAUSS,
	EWI_CROSSBOW,
	//TODO
	
	EWI_MAX
};

enum WEAPON_SIZE
{
	EWT_SMALL,
	EWT_MEDIUM,
	
	EWT_MAX
};

class cBaseWeapon : public cBaseAnimating
{
	Q_OBJECT
	
public:
	cBaseWeapon(QObject *parent = 0);
	virtual bool Attack(void);
	virtual bool Attack2(void);
	void Draw();
	
	static cBaseWeapon * SpawnWeapon(QString name);
	
	WEAPON_ID m_Id;
	WEAPON_SIZE m_WeapSize;
	
	QVector2D m_AimVec;
	
	cBaseTalker *m_pOwner;
	
	float m_NextAttack;
};

#endif // CBASEWEAPON_H
