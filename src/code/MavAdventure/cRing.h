#ifndef CRING_H
#define CRING_H

#include <cBaseAnimating.h>

class cRing : public cBaseAnimating
{
	Q_OBJECT
public:
	explicit cRing(QObject *parent = 0);
	
	void RenderMe();
signals:
	
public slots:
	
};

#endif // CRING_H
