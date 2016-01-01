#include "cBaseWeapon.h"
#include <cMainManager.h>
#include <util.h>

#include <cgauss.h>

cBaseWeapon::cBaseWeapon(QObject *parent) : cBaseAnimating(parent)
{
	m_NextAttack = 0.f;
	m_pOwner     = NULL;
	m_Id         = EWI_UNKNOWN;
	m_WeapSize   = EWT_MEDIUM;
}

bool cBaseWeapon::Attack()
{
	if (MGR->m_Time < m_NextAttack) return false;
	
	return true;
}

bool cBaseWeapon::Attack2()
{
	return false;
}

void cBaseWeapon::Draw()
{
	cBaseAnimating::Draw();
}

cBaseWeapon *cBaseWeapon::SpawnWeapon(QString name)
{
	if (name == "weapon_gauss") return new cGauss;
	
	return NULL;
}

