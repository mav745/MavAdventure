#ifndef CENTITYTOOL_H
#define CENTITYTOOL_H

#include "cBaseEntityTool.h"
#include <cBaseEntity.h>
#include <QVector>
#include <QVector2D>


enum ENTITY_TOOL_ACTION
{
	ETA_NONE = 0,
	ETA_SELECT,
	ETA_MOVE,
	//ETA_CREATE
};

class cEntityTool : public cBaseEntityTool
{
	Q_OBJECT
public:
	explicit cEntityTool(QObject *parent = 0);
	
	QVector<cBaseEntity *>m_Ents;
	QVector<cBaseEntity *>m_SelectedEnts;
	
	//QVector2D m_CreateStart;
	QVector2D m_SelectStart;
	
	QVector2D m_MouseAccum;
	
	ENTITY_TOOL_ACTION m_Action;
	
	void ProcessInput();
	void DrawMe(void);
	void ClearALL();
	
	void UpdatePropsDlg();
	void ShowPropsDlg();
signals:
	
public slots:
	void changeDepth();
	void ShiftAll();
};

extern cEntityTool *g_pEntityTool;

#endif // CENTITYTOOL_H
