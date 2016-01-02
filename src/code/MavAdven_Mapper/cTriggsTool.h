#ifndef CTRIGGSTOOL_H
#define CTRIGGSTOOL_H

#include "cBaseEntityTool.h"
#include <QVector>
#include <QVector2D>

enum TRIGGER_TOOL_ACTION
{
	TTA_NONE = 0,
	TTA_SELECT,
	TTA_MOVE,
	TTA_CREATE
};

class cBaseTrigger;

class cTriggsTool : public cBaseEntityTool
{
	Q_OBJECT
public:
	explicit cTriggsTool(QObject *parent = 0);
	
	QVector<cBaseTrigger *>m_Triggs;
	QVector<cBaseTrigger *>m_SelectedTriggs;
	
	QVector2D m_CreateStart;
	QVector2D m_SelectStart;
	
	QVector2D m_MouseAccum;
	
	TRIGGER_TOOL_ACTION m_Action;
	
	void ProcessInput();
	
	void DrawMe(void);
	
	void ClearALL();
	
	void UpdatePropsDlg();
	void ShowPropsDlg();
signals:
	
public slots:
	void ShiftAll();
	
};

extern cTriggsTool *g_pTriggsTool;

#endif // CTRIGGSTOOL_H
