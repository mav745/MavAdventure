#ifndef CBASEANIMATING_H
#define CBASEANIMATING_H

#include <cBaseObject.h>
#include <QMap>

struct anim_s
{
	anim_s(cSprite *sprite=NULL, bool lp = true)
	{
		spr = sprite;
		loop = lp;
	}

	cSprite *spr;
	bool loop;
};

class cBaseAnimating : public cBaseObject
{
	Q_OBJECT
	
protected:
	QString m_AnimName;
	QMap<QString,anim_s *>m_Anims;
	bool m_Loop;

private:
	QString m_SpawnSprite;
	int m_SpawnNumFrames;
	
public:
	explicit cBaseAnimating(QObject *parent = 0);
	
	void AddAnim(QString filename, QString animname, int numframes = 1, bool loop = true);
	void DeleteAnim(QString animname);
	void SetAnim(QString animname);
	
	void Draw();
	void RenderMe();
	void Spawn();
	
	virtual void EventAnimEnded();
	void ParseKeyValue(const QString &key, const QString &value);
signals:
public slots:
	
};

#endif // CBASEANIMATING_H
