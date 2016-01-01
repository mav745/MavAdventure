#include "cgauss.h"
#include <cBaseTalker.h>
#include <cMainManager.h>
#include <qfile.h>
#include <util.h>
#include <math.h>
#include <QDebug>

cGauss::cGauss(QObject *parent) : cBaseWeapon(parent)
{
	m_Charge = 0.f;
	m_Id = EWI_GAUSS;
	QFile file("settings/accel_standart.gauss");
	if (file.open(QFile::ReadOnly))
	{
		int x,y,num;
		QDataStream DS(&file);
		DS.setByteOrder(QDataStream::LittleEndian);
		DS>>num;
		qDebug()<<"num"<<num;
		
		for(int i=0;i<num;i++)
		{
			DS>>x>>y;
			m_AccelGraph.push_back(QVector2D(x,y));
		}
		
		file.close();
	}
}

bool cGauss::Attack()
{
	//if (!cBaseWeapon::Attack()) return false;
	
	m_NextAttack = MGR->m_Time + GAUSS_BEAM_TIME;
	
	m_BeamTime   = MGR->m_Time + GAUSS_BEAM_TIME;
	
	return true;
}
bool cGauss::Attack2()
{
	m_AimVec = -m_AimVec;
	
	m_NextAttack = MGR->m_Time + GAUSS_BEAM_TIME;
	
	m_BeamTime   = MGR->m_Time + GAUSS_BEAM_TIME;
	
	float accel = getAccel(m_pOwner->getSpeed());
	qDebug()<<"accel"<<accel;
	m_pOwner->m_Velocity[0] -= m_AimVec[0]*accel;
	m_pOwner->m_Velocity[1] -= m_AimVec[1]*350.f;
	
	return true;
}

float cGauss::getAccel(float speed)
{
	qDebug()<<"speed"<<speed;
	float f;
	for(int i=0;i<m_AccelGraph.size()-1;i++)
	{
		if (speed >= m_AccelGraph[i].x() && speed < m_AccelGraph[i+1].x())
		{
			f = 1.f-(speed-m_AccelGraph[i].x())/(m_AccelGraph[i+1].x()-m_AccelGraph[i].x());
			
			return m_AccelGraph[i].y()*f + m_AccelGraph[i+1].y()*(1.f-f);
		}
	}
	return 0.f;
}

void cGauss::Draw()
{
	cBaseWeapon::Draw();
	
	if (m_BeamTime > MGR->m_Time)
	{
		QVector2D beamStart = m_Orig   +m_AimVec* 32;
		QVector2D beamEnd   = beamStart+m_AimVec*900;
		//QVector aimTang(m_AimVec.y(),-m_AimVec.x());
		glLineWidth(6);
		glColor4f(1.f,1.f,1.f,(m_BeamTime - MGR->m_Time)/GAUSS_BEAM_TIME);
		MGR->drawLine(beamStart,beamEnd);
		glLineWidth(1);
	}
}

void cGauss::Step()
{
	cBaseWeapon::Step();
	m_Charge += MGR->m_DeltaTime*400;
	if (m_Charge > 100.f) m_Charge = 100.f;
}
