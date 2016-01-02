#include "cLine.h"

#ifndef CBASEEXPANDED_H
#define CBASEEXPANDED_H

class cBaseExpanded : public cLine
{
	Q_OBJECT
public:
	explicit cBaseExpanded(QObject *parent = 0);
	
	bool RenderMe();
	
	bool m_ToRemove;
signals:
	
public slots:
	
};

#endif // CBASEEXPANDED_H
