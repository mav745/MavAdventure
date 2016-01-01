#ifndef CNPCHELLEN_H
#define CNPCHELLEN_H

#include "cBaseTalker.h"



class cNpcHellen : public cBaseTalker
{
	Q_OBJECT
public:
	explicit cNpcHellen(QObject *parent = 0);
	~cNpcHellen();
	
	void Step();
};

#endif // CNPCHELLEN_H
