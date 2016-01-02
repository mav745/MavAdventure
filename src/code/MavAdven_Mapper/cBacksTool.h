#ifndef CBACKSTOOL_H
#define CBACKSTOOL_H

#include "cBaseTool.h"
#include <QVector>
#include <QVector2D>
//#include "cBackground.h"

class cBackground;

class cBacksTool : public cBaseTool
{
	Q_OBJECT
public:
	explicit cBacksTool(QObject *parent = 0);
	
	QVector2D m_MouseAccum;
	
	QVector<cBackground *>m_Backs;
	int m_CurBackIdx;
	
	virtual void DrawMe(void);
	
	virtual void ProcessInput(void);
	
	void UpdateBacksList();
	void ClearALL();
	cBackground *AddBack(QString fname, int depth);
signals:
	
public slots:
	void addBack();
	void removeBack();
	void changeBack();
	void changeBackDepth();
	
	void visibility();
	
	void scrFac();
	void scrSpd();
	void orig();
	
	void setHRep(bool state);
	void setVRep(bool state);
	
	void backSelected(QString name);
};

extern cBacksTool *g_pBacksTool;

#endif // CBACKSTOOL_H
