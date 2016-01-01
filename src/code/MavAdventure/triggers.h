#ifndef TRIGGERS_H
#define TRIGGERS_H

#include <cBaseTrigger.h>
#include <cBaseAnimating.h>

struct custom_font_s;

class cTriggerOnce : public cBaseTrigger
{
public:
	bool m_Used;
	explicit cTriggerOnce(QObject *parent = 0) : cBaseTrigger(parent)
	{
		m_Used = false;
	}
	void Touching(cBaseObject *pOther);
};

class cTriggerTeleport : public cBaseTrigger
{
public:
	explicit cTriggerTeleport(QObject *parent = 0) : cBaseTrigger(parent){}
	
	void Touching(cBaseObject *pOther);
	void ParseKeyValue(const QString &key, const QString &value);
	void Use(cBaseObject *pOther);
};

extern QString g_LandmarkName;
extern QVector2D g_LandmarkOffs;
extern QVector<cBaseObject*> g_TrasnferList;
extern QVector<cBaseObject*> g_TrasnferedList;

class cTriggerDoor : public cBaseTrigger
{
public:
	explicit cTriggerDoor(QObject *parent = 0);
	~cTriggerDoor();
	
	static int s_IconLocked;
	static int s_IconLeft;
	static int s_IconUp;
	static custom_font_s *s_LocsFont;
	static QMap<QString,QByteArray>s_LocsLocale;
	
	//cBaseAnimating *m_Arrow;
	
	QString m_NextMap;
	QString m_NextLocName;
	QString m_Landmark;
	int     m_TransferKey;
	bool    m_Locked;
	
	//void ScrollMe(const QVector2D &scrll);
	void Spawn();
	void Step();
	void Touching(cBaseObject *pOther);
	void RenderMe();
	void ParseKeyValue(const QString &key, const QString &value);

};

#endif // TRIGGERS_H
