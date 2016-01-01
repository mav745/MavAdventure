#ifndef CGAUSS_H
#define CGAUSS_H

#include <cBaseWeapon.h>
#include <qvector.h>

#define GAUSS_BEAM_TIME 0.5f

class cGauss : public cBaseWeapon
{
	Q_OBJECT
	
public:
	cGauss(QObject *parent = 0);
	
	float m_Charge;
	float m_BeamTime;
	
	bool Attack();
	void Draw();
	void Step();
	bool Attack2();
	
	QVector<QVector2D> m_AccelGraph;
	float getAccel(float speed);
};

#endif // CGAUSS_H
