#ifndef CBASETOOLBUTTON_H
#define CBASETOOLBUTTON_H

#include <QPushButton>
#include "cBaseTool.h"



class QFrame;

class cBaseToolButton : public QPushButton
{
	Q_OBJECT
public:
	explicit cBaseToolButton(QWidget *parent = 0);
	~cBaseToolButton()
	{
		if (m_pTool) delete m_pTool;
	}
	void UncheckOthers(void);
	
	cBaseTool * m_pTool;
	//QFrame *m_ToolPanel;
	int m_PageIdx;
	
	void HidePanel();
	void ShowPanel();
signals:
	
public slots:
	
	void IAmClicked(bool check);
	virtual void AssignMe();
};

#endif // CBASETOOLBUTTON_H
