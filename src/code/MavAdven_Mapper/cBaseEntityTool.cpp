#include "cBaseEntityTool.h"
#include <cBase.h>

cBaseEntityTool::cBaseEntityTool(QObject *parent) :
	cBaseTool(parent)
{
}

entity_type_s *cBaseEntityTool::EntCreateByName(const QString &name)
{
	if (!m_EntsMap.contains(name)) return NULL;
	return new entity_type_s(*m_EntsMap[name]);
}
