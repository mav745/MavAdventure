#ifndef CMAVERICK_H
#define CMAVERICK_H

#include "cBasePlayer.h"

class cSprite;

class cMaverick : public cBasePlayer
{
	Q_OBJECT
public:
	explicit cMaverick(QObject *parent = 0);
	
	float m_fWallJump;
	float m_fCircleRun;
	//bool  m_bCircleRun;
	float m_fNextBaseJump;
	float m_fBaseJumpTime;
	bool  m_BaseJumpReady;
	float m_TracX;
	//QVector2D m_TrackOrig;
	
	void AddAnimWithUpgrades(QString filename, QString animname, int numframes = 1, bool loop = true);
	
	void Spawn();
	
	float getSpeed();
	void Step(void);
	void SetAnimation();
	void Accelerate(const QVector2D &wishdir, float wishspeed, float accel);
	void AirAccelerate(const QVector2D &wishdir, float wishspeed, float accel);
	void Friction();
	void Gravity();
	void Jump();
	void Special();
	void CategorizePosition();
	void WalkMove();
	void PlayerMove();
	QVector2D ClipVelocity(const QVector2D &vel, const QVector2D &norm);
	void Draw();
	void ParseKeyValue(const QString &key, const QString &value);
signals:
	
public slots:
	virtual void createHud(void);
};

#endif // CMAVERICK_H
