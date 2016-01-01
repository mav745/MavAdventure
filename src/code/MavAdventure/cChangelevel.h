#ifndef CCHANGELEVEL_H
#define CCHANGELEVEL_H

#include <cBaseTrigger.h>
//#include <QVector>

//class cBaseObject;

class cChangelevel : public cBaseTrigger
{
	Q_OBJECT
public:
	explicit cChangelevel(QObject *parent = 0);
	
	QString m_NextRoom;
	QString m_LandMark;
	//QVector<cBaseObject *>m_Transfer;
signals:
	
public slots:
	
	void ChangeLevel();
};

#endif // CCHANGELEVEL_H
