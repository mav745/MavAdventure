#ifndef CBASEPLAYER_H
#define CBASEPLAYER_H

#include "cBaseTalker.h"

#include <QObject>
#include <QVector4D>

struct frame_s
{
	QVector2D orig;
	QVector2D vel;
	QVector4D wall;
};

class cBaseWeapon;

class cBasePlayer : public cBaseTalker
{
	Q_OBJECT
	
public:
	explicit cBasePlayer(QObject *parent = 0);

	~cBasePlayer();
	
	float m_RenderZ;
	
	bool m_ControlsActive;
	int m_Buttons;
	QVector2D m_Wishdir;
	float m_DepthSpeed;
	bool m_bOnGround;
	
	inline bool IsPlayer() {return true;}
	
	void CheckStuck();
	
	void FlyMove();
	
	void Attack(void);
	void Step(void);
	
	void Draw();
	
	cBaseWeapon *m_Weapon;
	
	virtual void AirAccelerate(const QVector2D &wishdir, float wishspeed, float accel) = 0;
	virtual void Accelerate(const QVector2D &wishdir, float wishspeed, float accel) = 0;
	virtual void Friction() = 0;
	virtual void Gravity() = 0;
	virtual void Jump() = 0;
	virtual void Special() = 0;
	virtual void CategorizePosition() = 0;
	virtual void WalkMove() = 0;
	virtual void PlayerMove() = 0;
	virtual void SetAnimation() = 0;
	virtual QVector2D ClipVelocity(const QVector2D &vel, const QVector2D &norm);
	
signals:
	
public slots:
	virtual void createHud(void);
};

#endif // CBASEPLAYER_H
