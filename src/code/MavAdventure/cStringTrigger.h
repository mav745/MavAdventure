#include "cBaseTrigger.h"

#ifndef CSTRINGTRIGGER_H
#define CSTRINGTRIGGER_H

class cStringTrigger : public cBaseTrigger
{
	Q_OBJECT
public:
	explicit cStringTrigger(QObject *parent = 0);
	
	QString m_Filter;
	
	
	
signals:
	void Activated(void);
	
public slots:
	
	void CheckString(QString str);
};

#endif // CSTRINGTRIGGER_H
