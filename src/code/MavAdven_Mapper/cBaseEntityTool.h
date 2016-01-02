#ifndef CBASEENTITYTOOL_H
#define CBASEENTITYTOOL_H

#include "cBaseTool.h"
#include <cBase.h>

typedef QMap<QString, entity_type_s*> entity_map_t;

class cBaseEntityTool : public cBaseTool
{
	Q_OBJECT
public:
	explicit cBaseEntityTool(QObject *parent = 0);
	
	entity_map_t m_EntsMap;
	
	entity_type_s *EntCreateByName(const QString &name);
	
signals:
	
public slots:
	
};

#endif // CBASEENTITYTOOL_H
