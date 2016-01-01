#include "cMainManager.h"
#include "util.h"
#include "cSprite.h"
#include <cGameRoom.h>
#include <cTileManager.h>
#include <QDebug>
#include "cBaseWeapon.h"
#include "cHud.h"
#include "cMaverick.h"
#include "cSegment.h"
#include <math.h>
#include <cBigDialog.h>
#include <triggers.h>

using namespace std;

#define SV_STEPSIZE 22.f
#define SV_ACCELERATE 10.f
#define SV_FRICTION 4.f
#define SV_SURF_UPSPEED -160.f

extern bool g_Recording;

QVector2D g_StartOrig;

//extern bool g_bFrameAdvance;
//extern QVector2D g_FrameAdvanceVel, g_FrameAdvanceOrig;

cMaverick::cMaverick(QObject *parent) :
	cBasePlayer(parent)
{
	m_fWallJump = 0.f;
	m_fCircleRun = 0.f;
	m_fNextBaseJump = 0.f;
	m_Depth = -1;
	m_ImageSpeed = 0.5f;
	
	//m_Orig[X] = 320;
	//m_Orig[Y] = 258;
	
	//g_FrameAdvanceVel = QVector2D(0,0);
	//g_FrameAdvanceOrig = m_Orig;
	
	g_StartOrig = m_Orig;
	AddAnimWithUpgrades("res/sprites/charas/mav/jump.png","jump",13,false);
	AddAnimWithUpgrades("res/sprites/charas/mav/bhop.png","bhop",13,false);
	AddAnimWithUpgrades("res/sprites/charas/mav/bhop_faster.png","bhop_faster",13,false);
	AddAnimWithUpgrades("res/sprites/charas/mav/bhop_superfast.png","bhop_super",13,false);
	AddAnimWithUpgrades("res/sprites/charas/mav/idle.png","idle");
	AddAnimWithUpgrades("res/sprites/charas/mav/run.ang" ,"run" ,16);
	AddAnimWithUpgrades("res/sprites/charas/mav/walk.png","walk",16);
	AddAnimWithUpgrades("res/sprites/charas/mav/spin.png","spin",17,false);
	AddAnimWithUpgrades("res/sprites/charas/mav/circlerun.png" ,"circlerun" ,19);

	SetAnim("idle");
	
	m_ControlsActive = true;
	
	m_TracX = 0.f;
}

void cMaverick::AddAnimWithUpgrades(QString filename, QString animname, int numframes, bool loop)
{
	static QString parts[] = {"_no","_u","_uf","_f","_df","_d"};
	AddAnim(filename,animname,numframes,loop);
	
	filename.replace("charas/mav","charas/mav/shoes");
	AddAnim(filename,animname+"_boots",numframes,loop);
	
	filename.replace("charas/mav/shoes","charas/mav/arms");
	AddAnim(filename,animname+"_arms",numframes,loop);
	
	filename.remove(".png");
	filename.remove(".ang");
	
	filename.replace("charas/mav/arms","charas/mav/gauss");
	for(int i=0;i<6;i++)
		AddAnim(filename+parts[i],animname+"_gauss"+parts[i],numframes,loop);
}

void cMaverick::Spawn()
{
	cBasePlayer::Spawn();
	
	cHud *pHud = (cHud*)m_pMyRoom->CreateObject("hud");
	pHud->m_CharaIdx = CHARA_MAV;
	
	int left,right,window,window_v,top,down;
	window   = 32;//64;
	window_v = 64;
	left  = 320-window/2;
	right = 320+window/2;
	top   = 240-window_v/2;
	down  = 240+window_v/2;
	
	float addx = UTIL_Sign(m_Velocity[0])*(max(0.f,(float)fabs(m_Velocity[0])-300.f))*MGR->m_FrameTime*40.f;
	addx = min(addx, 290.f);
	addx = max(addx,-290.f);
	
	float add_spd_x = 5.f;
	if (fabs(m_TracX - addx) < 30.f) add_spd_x = 2.f;
	
	if (m_TracX < addx-add_spd_x) m_TracX += add_spd_x;
	else if (m_TracX > addx+add_spd_x) m_TracX -= add_spd_x;
	else m_TracX = addx;
	
	QVector2D trackOrig;
	
	trackOrig[0] = m_Orig[0]+m_TracX;
	trackOrig[1] = m_Orig[1];
	
	if (m_pMyRoom->m_Size[XX] > 640)
	{
		if (trackOrig[XX] > right)
			m_pMyRoom->ScrollRoom(QVector2D(trackOrig[XX]-right,0),true);
		if (trackOrig[XX] < left)
			m_pMyRoom->ScrollRoom(QVector2D(trackOrig[XX]-left,0),true);
	}
	if (m_pMyRoom->m_Size[YY] > 480)
	{
		if (trackOrig[YY] > down)
			m_pMyRoom->ScrollRoom(QVector2D(0,trackOrig[YY]-down),true);
		if (trackOrig[YY] < top)
			m_pMyRoom->ScrollRoom(QVector2D(0,trackOrig[YY]-top),true);
	}
	
	CategorizePosition();
}

float cMaverick::getSpeed()
{
	return QVector2D(m_Velocity.x(),m_DepthSpeed).length();
}

void cMaverick::SetAnimation()
{
	if (fabs(m_Velocity[XX]) <= 1.0f)
	{
		if (m_bOnGround)
		{
			//qDebug()<<"idle";
			if (fabs(m_DepthSpeed) <= 1.0f)
				SetAnim("idle");
		}
		else
		{
			if (m_fWallJump >= MGR->m_Time)
			{
				SetAnim("spin");
				m_ImageSpeed = 0.5f;
			}
			else
			{
				SetAnim("jump");
				
				m_ImageIndex = 6.f + (m_Velocity[1])/25.f;
				if (m_ImageIndex > 12.f) m_ImageIndex = 12.f;
				if (m_ImageIndex <  0.f) m_ImageIndex =  0.f;
			}		
		}
	}
	else 
	{
		if (m_bOnGround)
		{
			if (m_fCircleRun >= MGR->m_Time)
			{
				SetAnim("circlerun");
				//qDebug()<<"index"<<m_ImageIndex;
				if (m_ImageIndex < 9)
					m_ImageSpeed = 0.4f;
				else
					m_ImageSpeed = 0.35f;
				if (fabs(m_Velocity[XX]) <= 250.f) m_fCircleRun = 0.f;
			}
			else
			{
				m_ImageScale[XX] = UTIL_Sign(m_Velocity[XX]);
				if (fabs(m_Velocity[XX]) <= 100.f)
				{
					SetAnim("walk");
					m_ImageSpeed = fabs(m_Velocity[XX])/250.f;
				}
				else
				{
					
					SetAnim("run");
					m_ImageSpeed = fabs(m_Velocity[XX])/470.f;
				}
			}
		}
		else
		{
			if (m_fWallJump >= MGR->m_Time)
			{
				SetAnim("spin");
				m_ImageSpeed = 0.5f;
			}
			else
			{
				m_ImageScale[XX] = UTIL_Sign(m_Velocity[XX]);
				
				if (fabs(m_Velocity[XX]) <= 150.f)
					SetAnim("jump");
				else if (fabs(m_Velocity[XX]) <= 400.f)
					SetAnim("bhop");
				else if (fabs(m_Velocity[XX]) <= 700.f)
					SetAnim("bhop_faster");
				else
					SetAnim("bhop_super");
				
				m_ImageIndex = 6.f + (m_Velocity[1])/25.f;
				if (m_ImageIndex > 12.f) m_ImageIndex = 12.f;
				if (m_ImageIndex <  0.f) m_ImageIndex =  0.f;
			}
		}
	}
}
void cMaverick::Accelerate(const QVector2D &wishdir, float wishspeed, float accel)
{
//	if (!g_bFrameAdvance) return;
	
	float currentspeed = QVector2D::dotProduct(
									QVector2D(m_Velocity[0],m_DepthSpeed), 
									wishdir);
	if (currentspeed < 0.f) currentspeed = 0.f;

	float addspeed = wishspeed - currentspeed;

	if (addspeed < 0.f) return;
	
	float accelspeed = accel * MGR->m_FrameTime * wishspeed;

	if (accelspeed > addspeed)
			accelspeed = addspeed;
	
	m_Velocity[0] += wishdir[0]*accelspeed;
	m_DepthSpeed  += wishdir[1]*accelspeed;

}
void cMaverick::AirAccelerate(const QVector2D &wishdir, float wishspeed, float accel)
{
//	if (!g_bFrameAdvance) return;
	
	float wishspd = wishspeed;
	
	float currentspeed = QVector2D::dotProduct(
									QVector2D(m_Velocity[0],m_DepthSpeed), 
									wishdir);
	if (currentspeed < 0.f) currentspeed = 0.f;

	if (wishspd > 30.f) wishspd = 30.f;
	
	float addspeed = wishspd - currentspeed;
	
	if (addspeed < 0.f) return;

	float accelspeed = accel * MGR->m_FrameTime * wishspeed;

	if (accelspeed > addspeed)
			accelspeed = addspeed;
	
	m_Velocity[0] += wishdir[0]*accelspeed;
	m_DepthSpeed  += wishdir[1]*accelspeed;
}
void cMaverick::Friction(void)
{
//	if (!g_bFrameAdvance) return;
	
	float speed = QVector2D(m_Velocity[0],m_DepthSpeed).length();//sqrt(vel[0]*vel[0] +vel[1]*vel[1] + vel[2]*vel[2]);
		
	// If too slow, return
	if (speed < 0.1f)
	{
		return;
	}
	
	float drop = 0.f;
	float control = (speed < 100.f) ? 100.f : speed;
	// Add the amount to t'he drop amount.
	drop += control*SV_FRICTION*MGR->m_FrameTime;
	
	float newspeed = speed - drop;
	if (newspeed < 0)
		newspeed = 0;
	newspeed /= speed;
	m_Velocity[0] = m_Velocity[0] * newspeed;
	m_DepthSpeed  = m_DepthSpeed  * newspeed;
}
void cMaverick::Gravity(void)
{
//	if (!g_bFrameAdvance) return;
	
	if (m_Velocity[1] < SV_SURF_UPSPEED)
	{
		traceresult_s tr;
		m_pMyRoom->TracePlayer(m_Orig,m_Orig+QVector2D(0,2),&tr);
				
		if (tr.fraction < 1.f)
			return;
	}
	m_Velocity[1] += 800.f*MGR->m_FrameTime;
}
void cMaverick::Jump(void)
{
	if (m_bOnGround)
	{
		traceresult_s tr;

		m_pMyRoom->TracePlayer(m_Orig,m_Orig+QVector2D(0,-2),&tr);
		m_Orig[1] = tr.endpos[1];
		//m_fNextBaseJump = MGR->m_Time + 0.1f;
		//m_fBaseJumpTime = MGR->m_Time + 0.4f;
		m_bOnGround = false;
		//m_BaseJumpReady = true;

		if (m_Velocity[1] >= 0.f)
		{
			m_Velocity[1] = -sqrtf(2.f * 800.f * 45.f);
		}
		else
		{
			m_Velocity[1] = -sqrtf(2.f * 800.f * 64.f);
			if (UTIL_keyState(butLeft)  & KEY_ISDOWN) 
				m_Velocity[0] = min(-120.f/**min(tr.normal[1],-0.2f)*/,m_Velocity[0]);
			if (UTIL_keyState(butRight) & KEY_ISDOWN) 
				m_Velocity[0] = max( 120.f/**min(tr.normal[1],-0.2f)*/,m_Velocity[0]);
		}
		return;
	}
//	else
//	{
//		if (m_BaseJumpReady && m_fNextBaseJump < MGR->m_Time && m_fBaseJumpTime > MGR->m_Time)
//		{
//			traceresult_s tr;
//			if (UTIL_keyState(butLeft)  & KEY_ISDOWN)
//				m_pMyRoom->TracePlayer(m_Orig,m_Orig+QVector2D(-6,6),&tr);
//			if (UTIL_keyState(butRight)  & KEY_ISDOWN)
//				m_pMyRoom->TracePlayer(m_Orig,m_Orig+QVector2D( 6,6),&tr);
//			if (tr.fraction < 1.f)
//			{
//				//qDebug()<<"basejumping"<<tr.normal[1];
//				if (tr.normal[1] <= -0.9f)
//				{
//					qDebug()<<"basejumped"<<MGR->m_Time;
//					m_Velocity[1] = -sqrtf(2.f * 800.f * 77.f);
//					if (UTIL_keyState(butLeft)  & KEY_ISDOWN)
//					{
//						//m_Velocity[0] -= 120.f;
//						m_Velocity[0] = min(-120.f/**min(tr.normal[1],-0.2f)*/,m_Velocity[0]);
//					}
//					if (UTIL_keyState(butRight) & KEY_ISDOWN) 
//					{
//						//m_Velocity[0] += 120.f;
//						m_Velocity[0] = max( 120.f/**min(tr.normal[1],-0.2f)*/,m_Velocity[0]);
//					}
//					//m_fNextBaseJump = MGR->m_Time + 0.1f;
					
//					m_BaseJumpReady = false;
//				}
//			}
//		}
//	}
}
void cMaverick::Special(void)
{
	
}
void cMaverick::CategorizePosition()
{
	traceresult_s tr;
	if (g_BootsUpgrade && m_Velocity.y() > 80.f)
	{
		if (!(UTIL_keyState(butB) & KEY_ISDOWN) /*&& (UTIL_keyState(butC) & KEY_ISDOWN)*/)
		{
			m_pMyRoom->TracePlayer(m_Orig,m_Orig+QVector2D(0,3),&tr);
			if (tr.fraction < 1.f)
			{
				m_bOnGround = false;
				return;
			}
		}
	}
	
	tr.Reset();
	m_pMyRoom->TracePlayer(m_Orig,m_Orig+QVector2D(0,3),&tr);

//	if (tr.fraction < 1.f && tr.normal[1] <= -0.7)
//	{
//		m_Velocity[1] = 0.f;
//		m_Orig = tr.endpos;
//	}
	if (/*g_BootsUpgrade && */m_Velocity.y() < SV_SURF_UPSPEED)
	{
		m_bOnGround = false;
		return;
	}
	
	if (tr.fraction < 1.f && tr.normal[1] <= -0.7)
	{
		if (!m_bOnGround)
		{
			float newvel = m_Velocity[0] + tr.normal[0]*fabs(m_Velocity[1]);
			if (fabs(newvel) < 500.f && fabs(newvel) > fabs(m_Velocity[0]) 
				&& UTIL_Sign(m_Velocity[0]) == UTIL_Sign(newvel))
					m_Velocity[0] = newvel;
		}
		//if (!m_bOnGround) qDebug()<<"glued";
		m_bOnGround = true;
		m_Orig[1] = tr.endpos[1];//+QVector2D(0,-1);
		
		
		//
	}
	else
	{
		m_bOnGround = false;
	}
}
void cMaverick::WalkMove()
{
	traceresult_s tr;
	float upmove = 0;
	
	QVector2D origOrig = m_Orig, origVel = m_Velocity, down, downVel;
	FlyMove();
	
	down = m_Orig;
	downVel = m_Velocity;
	float downmove = (m_Orig - origOrig).length();
	
	m_Orig = origOrig;
	m_Velocity = origVel;
	
	m_pMyRoom->TracePlayer(m_Orig,m_Orig+QVector2D(0,-SV_STEPSIZE),&tr);
	m_Orig = tr.endpos;
	
	g_Recording = false;
	FlyMove();
	g_Recording = true;
			
	tr.Reset();
	m_pMyRoom->TracePlayer(m_Orig,m_Orig+QVector2D(0, SV_STEPSIZE),&tr);
	m_Orig = tr.endpos;
	
	if ( tr.normal.y() > -0.7f)
		goto usedown;
	
	upmove = (m_Orig - origOrig).length();
	if (downmove > upmove)
	{
usedown:
		m_Orig = down;
		m_Velocity = downVel;
	}
	else
	{
		//m_bOnGround = true;
		m_Velocity[1] = downVel[1];
	}
}
//bool g_bFrameAdvanceMode = false;
void cMaverick::PlayerMove()
{
//	if (UTIL_keyState(butC) & KEY_PRESSED)
//		g_bFrameAdvance = true;
//	if (UTIL_keyState(butStart) & KEY_PRESSED)
//		g_bFrameAdvanceMode = !g_bFrameAdvanceMode;
	bool prevOnground = m_bOnGround;
	if (m_ControlsActive && !g_pBigDialog->m_Visible)
	{
		if (UTIL_keyState(butB) & KEY_ISDOWN)
		{
			Jump();
		}
		if (m_bOnGround)
		{
			QVector2D wishdir(0.f,0.f);
			bool toAccel = false;
			
			if (UTIL_keyState(butLeft)  & KEY_ISDOWN) {wishdir[0] -= 1.f;toAccel = true;}
			if (UTIL_keyState(butRight) & KEY_ISDOWN) {wishdir[0] += 1.f;toAccel = true;}
			
			if (toAccel)
			{
				if (/*!(UTIL_keyState(butC)& KEY_ISDOWN) || */QVector2D(m_Velocity[0],m_DepthSpeed).length() < 100.f)
						m_Wishdir = wishdir;
				
				float dir_now, dir_ideal;
				dir_now   = atan2(m_Wishdir.y(),m_Wishdir.x());
				dir_ideal = atan2(  wishdir.y(),  wishdir.x());
				float a,b;
				a = 2.f*M_PI - dir_ideal + dir_now;
				a = UTIL_FModulo(a,2.f*M_PI);
				if (a < 0.f) a += 2.f*M_PI;
				b = 2.f*M_PI - a;
				if (a < b) dir_now -= min(a,1.85f*float(M_PI)*MGR->m_FrameTime);
				else       dir_now += min(b,1.85f*float(M_PI)*MGR->m_FrameTime);
				
				m_Wishdir = QVector2D(cosf(dir_now),sinf(dir_now));
				
//				QVector2D curVel = QVector2D(m_Velocity[0],m_DepthSpeed);
//				float curVelLength = curVel.length();
				
//				if (!(UTIL_keyState(butC)& KEY_ISDOWN))
//					Accelerate(m_Wishdir.normalized(),220.f,SV_ACCELERATE);
//				else
				{
					if (min(a,b) <= 0.1f)
					{
						if (fabs(m_Velocity[0]) < 20.f || UTIL_Sign(m_Wishdir[0]) == UTIL_Sign(m_Velocity[0]))
						{
							if (fabs(m_Velocity[0]) < 220.f)
							{
								float add = 220.f-fabs(m_Velocity[0]);
								add = min(add,1500.f*MGR->m_FrameTime);
								m_Velocity[0] += wishdir.x()*add;
							}
						}
						else
						{
							m_Velocity[0] -= 50.f*MGR->m_FrameTime*UTIL_Sign(m_Velocity[0]);
						}
						m_fCircleRun = 0.f;
						//	Accelerate(m_Wishdir.normalized(),220.f,SV_ACCELERATE);
					}
					else
					{
						if (m_fCircleRun < MGR->m_Time)
							m_fCircleRun = MGR->m_Time + 1.5f;
						//m_bCircleRun = true;
						if (a < b)
							Accelerate(QVector2D(cosf(dir_now-M_PI_2),sinf(dir_now-M_PI_2)),220.f,SV_ACCELERATE);
						else
							Accelerate(QVector2D(cosf(dir_now+M_PI_2),sinf(dir_now+M_PI_2)),220.f,SV_ACCELERATE);
					}
//					curVel = QVector2D(m_Velocity[0],m_DepthSpeed);
//					qDebug()<<"vels3:"<<curVelLength<<curVel.length();
				}
			}
		}
		else
		{
//			if (g_bFrameAdvance)
			{
				QVector2D wishdir(0.f,0.f);
				bool toAccel = false;
				
				if (UTIL_keyState(butLeft)  & KEY_ISDOWN) {wishdir[0] -= 1.f;toAccel = true;}
				if (UTIL_keyState(butRight) & KEY_ISDOWN) {wishdir[0] += 1.f;toAccel = true;}
	
				if (fabs(m_DepthSpeed) >= 20.f 
					 && !(UTIL_keyState(butLeft) & KEY_ISDOWN)
					 && !(UTIL_keyState(butRight) & KEY_ISDOWN))
				{
					m_DepthSpeed = 0.f;
				}
				
				if (toAccel)
				{
					float curVelLength = QVector2D(m_Velocity[0],m_DepthSpeed).length();
					
					float dir_now, dir_ideal;
					dir_now   = atan2(m_DepthSpeed,m_Velocity[0]);//atan2(m_Wishdir.y(),m_Wishdir.x());
					dir_ideal = atan2(  wishdir.y(),  wishdir.x());
					float a,b;
					a = 2.f*M_PI - dir_ideal + dir_now;
					a = UTIL_FModulo(a,2.f*M_PI);
					if (a < 0.f) a += 2.f*M_PI;
					b = 2.f*M_PI - a;
					if (a < b) dir_now -= min(a,1.2f*float(M_PI)*MGR->m_FrameTime);
					else       dir_now += min(b,1.2f*float(M_PI)*MGR->m_FrameTime);
					
					m_Wishdir = QVector2D(cosf(dir_now),sinf(dir_now));
					if (fabs(m_Velocity[0]) < 30.f)
					{
						m_Velocity[0] = 30.f*wishdir[0];
					}
					else
					{
						if (min(a,b) <= 0.01f)
						{
							if (fabs(m_Velocity[0]) < 500.f)
							{
								m_Velocity[0] += 5600.f/(10.f+0.26f*fabs(m_Velocity[0]))*m_Wishdir[0]*MGR->m_FrameTime;
							}
							else
							{
								if (UTIL_Sign(m_Wishdir[0]) != UTIL_Sign(m_Velocity[0]))
									m_Velocity[0] += 5600.f/(10.f+0.24f*fabs(m_Velocity[0]))*m_Wishdir[0]*MGR->m_FrameTime;
							}
						}
						else
						{
							m_Velocity[0] = m_Wishdir[0]*curVelLength;
							m_DepthSpeed  = m_Wishdir[1]*curVelLength;
						}
					}
					m_Wishdir = QVector2D(m_Velocity[0],m_DepthSpeed).normalized();
				}
			}
		}
		
		//qDebug()<<"m_Velocity[0]"<<m_Velocity[0];
		//glColor3f(1.f,0.f,0.f);
		//MGR->drawLine(QVector2D(320.f,240.f),QVector2D(320.f,240.f)+m_Wishdir*64.f);
		//MGR->drawText(320,200,QString().number(m_Velocity[0]));//QVector2D(m_Velocity[0],m_DepthSpeed).length()));
	}
	
	QVector2D velocityPrev = m_Velocity;
	
	CheckStuck();
	
	if (m_bOnGround)
	{
		m_fWallJump = 0.f;
		Friction();
		
		m_Velocity[1] = 0.f;
		
		WalkMove();
		//FlyMove();
		CategorizePosition();
	}
	else
	{
		//qDebug()<<"vel[1]"<<m_Velocity[1];
		Gravity();
		
		//WalkMove();
		FlyMove();
		
		CategorizePosition();
	}
	
	
			
	if (!m_bOnGround && m_fWallJump < MGR->m_Time &&
		 fabs(velocityPrev[0]) > 350.f && m_Velocity[1] < -450.f && fabs(m_Velocity[0]) < 30.f)
	{
		m_fWallJump = MGR->m_Time + 0.5f;
		//qDebug()<<"walljump";
	}
	if (!prevOnground && m_bOnGround)
	{
		m_Wishdir[1] = 0.f;
		m_Wishdir.normalize();
	}
	
//	g_bFrameAdvance = false || g_bFrameAdvanceMode;
}
QVector2D cMaverick::ClipVelocity(const QVector2D &vel, const QVector2D &norm)
{
	QVector2D normVel = vel.normalized();
	QVector2D newVel;
	newVel = vel - norm*QVector2D::dotProduct(norm,vel);
	if (QVector2D::dotProduct(norm,normVel) >= 0.951056516f) //flat angle too keep speed
	{
		newVel = newVel.normalized()*vel.length();
	}
	return newVel;
}

void cMaverick::Draw()
{
	//qDebug()<<"m_Orig"<<m_Orig;
	
	if (m_Flags & FL_TRANSFER)
	{
		return;
	}
	cBasePlayer::Draw();

	float old_image_speed = m_ImageSpeed;
	m_ImageSpeed = 0.f;
	cSprite * old_anim = m_pTexture;
	
	if (g_BootsUpgrade)
	{
		if (int((g_fBootsUpgTime - MGR->m_Time)*1000.f)%100 < 50 || g_fBootsUpgTime == 0.f)
		{
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
		}
		else
		{
			glBlendFunc(GL_SRC_ALPHA,GL_ONE);
		}
		if (m_Anims.contains(m_AnimName+"_boots"))
		{
			m_pTexture = m_Anims[m_AnimName+"_boots"]->spr;
			m_Loop     = m_Anims[m_AnimName+"_boots"]->loop;
			cBasePlayer::Draw();
		}
		
		glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	}
	if (m_Weapon)
	{
		QString post = "_f";
		if (fabs(m_Velocity.x() < 10.f)) post = "_no";
		qDebug()<<fabs(m_Velocity.x());
		if (UTIL_keyState(butRight) & KEY_ISDOWN || UTIL_keyState(butLeft)& KEY_ISDOWN)
		{
			if (UTIL_keyState(butUp)& KEY_ISDOWN) post = "_uf";
			else if (UTIL_keyState(butDown)& KEY_ISDOWN) post = "_df";
			else post = "_f";
		}
		else
		{
			if (UTIL_keyState(butUp  )& KEY_ISDOWN) post = "_u";
			else if (UTIL_keyState(butDown)& KEY_ISDOWN) post = "_d";
		}
		
		if (m_Anims.contains(m_AnimName+"_gauss"+post))
		{
			m_pTexture = m_Anims[m_AnimName+"_gauss"+post]->spr;
			m_Loop     = m_Anims[m_AnimName+"_gauss"+post]->loop;
			cBasePlayer::Draw();
		}
	}
	else
	{
		if (m_Anims.contains(m_AnimName+"_arms"))
		{
			m_pTexture = m_Anims[m_AnimName+"_arms"]->spr;
			m_Loop     = m_Anims[m_AnimName+"_arms"]->loop;
			cBasePlayer::Draw();
		}
	}
	
	m_pTexture = old_anim;
	m_ImageSpeed = old_image_speed;
	
	//m_Anims[m_AnimName+"_boots"]->spr->DrawFrame(m_Orig,m_ImageIndex);
	
//	glColor3f(0.f,1.f,1.f);
	
//	MGR->drawLine(m_Orig+QVector2D(-15,-30),m_Orig+QVector2D( 15,-30));
//	MGR->drawLine(m_Orig+QVector2D( 15,-30),m_Orig+QVector2D( 15, 30));
//	MGR->drawLine(m_Orig+QVector2D( 15, 30),m_Orig+QVector2D(-15, 30));
//	MGR->drawLine(m_Orig+QVector2D(-15, 30),m_Orig+QVector2D(-15,-30));
//	glColor3f(0.f,1.f,0.f);
	//	MGR->drawLine(QVector2D(0,0),QVector2D(640,480));
}

void cMaverick::ParseKeyValue(const QString &key, const QString &value)
{
	if (key == "has_gauss")
	{
		//if (value == "TRUE")
		//{
			m_pWeapon = (cBaseWeapon*)m_pMyRoom->CreateObject("weapon_gauss");
			m_pWeapon->m_pOwner = this;
			DBG_PRINT_S("mav with gauss");
		//}
	}
	else
		cBasePlayer::ParseKeyValue(key,value);
}

void cMaverick::createHud()
{
	
}

void cMaverick::Step()
{
	if (m_Flags & FL_TRANSFER)
		return;
	
	cBasePlayer::Step();
	
	if (m_Flags & FL_TRANSFER)
		return;
	
	int left,right,window,window_v,top,down;
	window   = 32;//64;
	window_v = 64;
	left  = 320-window/2;
	right = 320+window/2;
	top   = 240-window_v/2;
	down  = 240+window_v/2;
	
	float addx = UTIL_Sign(m_Velocity[0])*(max(0.f,(float)fabs(m_Velocity[0])-300.f))*MGR->m_FrameTime*40.f;
	addx = min(addx, 290.f);
	addx = max(addx,-290.f);
	
	float add_spd_x = 5.f;
	if (fabs(m_TracX - addx) < 30.f) add_spd_x = 2.f;
	
	if (m_TracX < addx-add_spd_x) m_TracX += add_spd_x;
	else if (m_TracX > addx+add_spd_x) m_TracX -= add_spd_x;
	else m_TracX = addx;
	
	QVector2D trackOrig;
	
	trackOrig[0] = m_Orig[0]+m_TracX;
	trackOrig[1] = m_Orig[1];
	
	if (m_pMyRoom->m_Size[XX] > 640)
	{
		if (trackOrig[XX] > right)
		{
			m_pMyRoom->ScrollRoom(QVector2D(trackOrig[XX]-right,0));
			//g_StartOrig[X] -= trackOrig[X]-right;
		}
		if (trackOrig[XX] < left)
		{
			m_pMyRoom->ScrollRoom(QVector2D(trackOrig[XX]-left,0));
			//g_StartOrig[X] -= trackOrig[X]-left;
		}
	}
	if (m_pMyRoom->m_Size[YY] > 480)
	{
		if (trackOrig[YY] > down)
		{
			m_pMyRoom->ScrollRoom(QVector2D(0,trackOrig[YY]-down));
			//g_StartOrig[Y] += trackOrig[Y]-down;
		}
		if (trackOrig[YY] < top)
		{
			m_pMyRoom->ScrollRoom(QVector2D(0,trackOrig[YY]-top));
			//g_StartOrig[Y] += trackOrig[Y]-top;
		}
	}
	//qDebug()<<"speed"<<m_Velocity.x();
}
