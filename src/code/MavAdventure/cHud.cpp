#include "cGameRoom.h"
#include "cHud.h"
#include <cSprite.h>
const float 
	g_HudLeftX = 78.f, 
	g_HudLeftY = 25.f;
cHud::cHud(QObject *parent) : cBaseAnimating(parent)
{
	AddAnim("res/sprites/hud/left_hud_gauss.png","gauss_norm");
	AddAnim("res/sprites/hud/left_hud_gauss-disabled.png","gauss_dis",2);
	AddAnim("res/sprites/hud/left_hud_no_gauss.png","gauss_brok",2);
	AddAnim("res/sprites/hud/left_hud_default.png","default");

	//m_Depth   = INT_MIN;
	m_Orig[0] = g_HudLeftX+8.f;
	m_Orig[1] = g_HudLeftY+8.f;//480-g_HudLeftY-8.f;
	m_ImageSpeed = 0.02f;
	SetAnim("gauss_dis");
	
	m_ScrollFac = QVector2D(0,0);
	
	m_pCharaHealth = cSprite::getSprite("res/sprites/hud/health_left.png",56,EFD_VER);
	m_pCharaHealth->m_Orig = QPointF(0,0);
	
	m_pCharaSign = cSprite::getSprite("res/sprites/hud/chara_sign.png",4);
	m_CharaIdx   = CHARA_MAV;
}

void cHud::Step(void)
{
	
}

void cHud::Draw()
{
	cBaseAnimating::Draw();
	
	float hp = 100.f;
	int hpIdx = 56.f*hp/100.f;
	
	m_pCharaSign->DrawFrame(m_Orig,m_CharaIdx);
	m_pCharaHealth->DrawFrame(m_Orig+QVector2D(40-g_HudLeftX,21-g_HudLeftY),hpIdx);
}
