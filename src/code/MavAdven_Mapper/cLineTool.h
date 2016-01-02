#include "cBaseTool.h"

#ifndef CLINETOOL_H
#define CLINETOOL_H

#include <QVector2D>
#include <QVector>
#include <QMap>
class cSegment;
class cLine;

class cLineTool : public cBaseTool
{
	Q_OBJECT
public:
	explicit cLineTool(QObject *parent = 0);
	
	bool m_Visible;
	
	QVector2D m_Start;
	QVector2D m_End;
	
	bool m_Selecting;
	QVector2D m_SelStart;
	
	QVector<cLine *>m_Lines;
	//QMap<long long,cSegment *>m_Lines;
	QVector<cLine *>m_SelectedLines;
	QVector<cLine *>m_CopiedLines;
	
	void ProcessInput();
	
	void DrawMe(void);
	//void DeleteLine(cLine *pLine);
	void ClearALL();
signals:
	
public slots:
	void shiftLines();
	void flipLines();
	void jumpFlagLines();
	void mirrorHor();
	void SwitchVisibility();
	void mirrorVer();
};

extern cLineTool *g_pLineTool;

#endif // CLINETOOL_H
