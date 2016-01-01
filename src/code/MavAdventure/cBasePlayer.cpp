#include "cMainManager.h"
#include "util.h"
#include "cGameRoom.h"
#include "cBasePlayer.h"
#include "cSegment.h"
#include "math.h"
#include <cBaseTrigger.h>
#include <QTimer>
#include <cLine.h>
#include <cBaseWeapon.h>

QTimer g_HudTimer;

using namespace std;

bool g_Recording = true;
QVector<frame_s>g_History;

cBasePlayer::cBasePlayer(QObject *parent) :
	cBaseTalker(parent)
{
	m_Weapon = NULL;
	m_bOnGround = false;
	m_DepthSpeed = 0.f;
	connect(&g_HudTimer,SIGNAL(timeout()),this,SLOT(createHud()));
	g_HudTimer.setSingleShot(true);
	g_HudTimer.start(100);
}

cBasePlayer::~cBasePlayer()
{
	
}

void cBasePlayer::CheckStuck()
{
	cSegment *seg;
	for(int i=m_pMyRoom->m_Walls.size()-1;i>=0;i--)
	{
		seg = m_pMyRoom->m_Walls[i];
		if (seg->m_pLine->m_JumpDown) continue;
		
		float dist = QVector2D::dotProduct(m_Orig - seg->m_S,seg->m_N);
		if (dist < 0.f && dist > -16.f)
		{
			QVector2D t = seg->m_T.normalized();
			float s_dist,e_dist;
			s_dist = QVector2D::dotProduct(m_Orig - seg->m_S,t);
			e_dist = QVector2D::dotProduct(m_Orig - seg->m_E,t);
			if (UTIL_Sign(s_dist) != UTIL_Sign(e_dist) && QVector2D::dotProduct(m_Velocity,seg->m_N) < 0.f)
			{
				//m_Velocity = QVector2D(0,0);
				qDebug()<<"Stuck!";
				m_Orig = seg->m_S + t*s_dist+seg->m_N;
				return;
			}
		}
	}
}

void cBasePlayer::createHud()
{
	m_pMyRoom->CreateObject("hud");
}

void cBasePlayer::Step()
{
//	QVector2D vel = m_Velocity;
//	m_Velocity = QVector2D(0,0);
//	cBaseObject::Step();
//	m_Velocity = vel;
	
	while (g_History.size() > 520)
	{
		g_History.removeAt(0);
	}
	glColor3f(0,1,0);
	for(int i=g_History.size()-1;i>=1;i--)
	{
		MGR->drawLine(
					g_History[i].orig-MGR->m_pCurrentRoom->m_View.orig,
					g_History[i-1].orig-MGR->m_pCurrentRoom->m_View.orig);
	}
	PlayerMove();
	
	Attack();
	SetAnimation();
	
	if (m_RenderZ < m_Orig[1] || !m_bOnGround) m_RenderZ = m_Orig[1];
	else
	{
		if (m_RenderZ > m_Orig[1] + 18.f)
			m_RenderZ = m_Orig[1] + 18.f;
		else
		{
			if (MGR->m_FrameTime * 200.f < fabs(m_RenderZ - m_Orig[1]))
				m_RenderZ -= MGR->m_FrameTime * 400.f;
			else
				m_RenderZ = m_Orig[1];
		}
	}
	
	for(int i=m_pMyRoom->m_pTriggers.size()-1;i>=0;i--)
	{
		if (m_pMyRoom->m_pTriggers[i]->Crosses(
				 m_Orig-VEC_EXPAND,
				 m_Orig+VEC_EXPAND))
		{
			m_pMyRoom->m_pTriggers[i]->Touching(this);
		}
	}
}

void cBasePlayer::Draw()
{
	float oldZ = m_Orig[1];
	m_Orig[1] = m_RenderZ;
	cBaseTalker::Draw();
	m_Orig[1] = oldZ;
}

QVector2D cBasePlayer::ClipVelocity(const QVector2D &vel, const QVector2D &norm)
{
	return vel - norm*QVector2D::dotProduct(norm,vel);
}

//bool g_bDebugPrint;
//bool g_bFrameAdvance = false;
//QVector2D g_FrameAdvanceVel, g_FrameAdvanceOrig;

void cBasePlayer::FlyMove()
{
	float remaintime = MGR->m_FrameTime;
	QVector2D originalVel = m_Velocity;
//	if (g_Recording)
//	{
//		frame_s frame1;
//		frame1.orig = m_Orig+MGR->m_pCurrentRoom->m_View.orig;
//		frame1.vel = m_Velocity;
//		frame1.wall = QVector4D(0,0,0,0);
//		g_History.push_back(frame1);
//	}

	traceresult_s tr;
	
	int numplanes = 0;
	QVector2D planes[5];
	
//	glMatrixMode(GL_MODELVIEW);
//	glLoadIdentity();
//	glColor3f(0.f,1.f,0.f);
//	MGR->drawLine(QVector2D(0,0),QVector2D(640,480));
	
	
	
	float allFraction = 0.f;
	float prevSpd = m_Velocity.length();

	//QVector2D origOrig = m_Orig;
	for(int i=0;i<4;i++)
	{
//		g_bDebugPrint = true;
		m_pMyRoom->TracePlayer(m_Orig,m_Orig+m_Velocity*remaintime,&tr);
//		g_bDebugPrint = false;
//		glColor3f(0.f,1.f,0.f);
//		MGR->drawLine(
//					QVector2D(320,240)+(m_Orig-origOrig)*10.f,
//					QVector2D(320,240)+(m_Orig-origOrig+m_Velocity*remaintime)*10.f);
//		if (tr.fraction != 1.0)
//		{
//			glLineWidth(2.f);
//			MGR->drawLine(
//						QVector2D(320,240)+(tr.wall->m_S-origOrig)*10.f,
//						QVector2D(320,240)+(tr.wall->m_E-origOrig)*10.f);
//			MGR->drawRect(
//						QVector2D(320,240)+(tr.endpos-origOrig)*10.f-QVector2D(2,2),
//						QVector2D(320,240)+(tr.endpos-origOrig)*10.f+QVector2D(2,2));
//			glLineWidth(1.f);
//		}
//		if (!g_bFrameAdvance) return;
		
		//qDebug()<<"tr.fraction"<<tr.fraction;
		
		allFraction += tr.fraction;
				
		if (tr.fraction != 0.f)
		{
			m_Orig = tr.endpos;
			numplanes = 0;
		}
		if (tr.fraction == 1.f)
			break;
		
		remaintime -= tr.fraction*remaintime;
		if (numplanes >= 5) break;
		
		planes[numplanes] = tr.normal;
		
		numplanes++;
		
		if (remaintime <= 0.f)
			break;
		
		if (!m_bOnGround)
		{
			//qDebug()<<"numplanes"<<numplanes;
			for(int j=0;j<numplanes;j++)
			{
				
				//float prevZVel = m_Velocity[1];
				m_Velocity = ClipVelocity(m_Velocity,planes[j]);
				tr.Reset();
				if (m_Velocity[1] <= -40)
				{
					m_pMyRoom->TracePlayer(m_Orig,m_Orig+QVector2D(0,2),&tr);
					if (tr.fraction != 1.f && tr.normal[1] <= -0.7)
						m_Velocity = m_Velocity.normalized()*prevSpd*.99f;
				}
				//m_Velocity -= planes[j]*QVector2D::dotProduct(planes[j],m_Velocity);
			}
		}
		else
		{
			int k;
			for (k=0 ; k<numplanes ; k++)
			{
				m_Velocity = ClipVelocity(m_Velocity,planes[k]);
				//m_Velocity -= planes[k]*QVector2D::dotProduct(planes[k],m_Velocity);
				int j;
				for (j=0 ; j<numplanes ; j++)
				{
					if (j != k)
					{
						// Are we now moving against this plane?
						if (QVector2D::dotProduct (m_Velocity, planes[j]) < 0.f)
							break;	// not ok
					}
				}
				if (j == numplanes)  // Didn't have to clip, so we're ok
					break;
			}
			
			// Did we go all the way through plane set
			if (k != numplanes)
			{	// go along this plane
				// pmove->velocity is set in clipping call, no need to set again.
				;  
			}
			else
			{	// go along the crease
				if (numplanes != 2)
				{
					//Con_Printf ("clip velocity, numplanes == %i\n",numplanes);
					//VectorCopy (vec3_origin, pmove->velocity);
					m_Velocity = QVector2D(0,0);
					//Con_DPrintf("Trapped 4\n");

					break;
				}
				QVector3D dir = QVector3D::crossProduct(
							QVector3D(planes[0].x(),0.f,planes[0].y()),
							QVector3D(planes[1].x(),0.f,planes[1].y())
						);
				//CrossProduct (planes[0], planes[1], dir);
				float d = QVector3D::dotProduct(
								 dir, 
								 QVector3D(m_Velocity[0],m_DepthSpeed,m_Velocity[1]));
				//VectorScale (dir, d, pmove->velocity );
				m_Velocity[0] = dir[0]*d;
				//m_DepthSpeed  = dir[1]*d;
				m_Velocity[1] = dir[2]*d;
			}
			// Half-Life:
			// if original velocity is against the original velocity, stop dead
			// to avoid tiny occilations in sloping corners
			
			if (QVector2D::dotProduct(m_Velocity,originalVel) <= 0.f)
			{
				m_Velocity = QVector2D(0,0);
				break;
			}
		}
	}
	if (allFraction == 0.f)
	{
		m_Velocity = QVector2D(0,0);
	}
	//qDebug()<<"numplanes"<<numplanes;
	
}

void cBasePlayer::Attack()
{
	if (!m_pWeapon)
	{
		//qDebug()<<"Attack with no weapon!";
		return;
	}
	if ((UTIL_keyState(butA) & KEY_PRESSED) || (UTIL_keyState(butC) & KEY_PRESSED))
	{
		if (MGR->m_Time > m_pWeapon->m_NextAttack)
		{
			if ((UTIL_keyState(butLeft) & KEY_ISDOWN))
				m_pWeapon->m_AimVec.setX(-1);
			else if ((UTIL_keyState(butRight) & KEY_ISDOWN))
				m_pWeapon->m_AimVec.setX( 1);
			// !right && !left
			else if ( ((UTIL_keyState(butUp) & KEY_ISDOWN) || (UTIL_keyState(butDown) & KEY_ISDOWN)) )
				m_pWeapon->m_AimVec.setX(0); 
			else m_pWeapon->m_AimVec.setX(m_ImageScale.x()); 
				
			
			if (UTIL_keyState(butUp) & KEY_ISDOWN)
				m_pWeapon->m_AimVec.setY(-1); 
			else if (UTIL_keyState(butDown) & KEY_ISDOWN)
				m_pWeapon->m_AimVec.setY(1); 
			else
				m_pWeapon->m_AimVec.setY(0); 
			m_pWeapon->m_AimVec.normalize();
	
			//qDebug()<<"aimvec"<<m_pWeapon->m_AimVec;

			m_pWeapon->m_Orig = m_Orig+QVector2D(0,-12);
			if (UTIL_keyState(butA) & KEY_PRESSED)
				m_pWeapon->Attack();
			if (UTIL_keyState(butC) & KEY_PRESSED)
				m_pWeapon->Attack2();
		}
	}
}
