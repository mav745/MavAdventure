#ifndef CBASERENDERABLE_H
#define CBASERENDERABLE_H

#include <QObject>

class cGameRoom;

class cBaseRenderable : public QObject
{
	Q_OBJECT
public:
	
	QString m_ClassName;
	
	int m_Flags;
	
	bool m_Visible;
	int m_Depth;
	
	cGameRoom *m_pMyRoom;
	
	explicit cBaseRenderable(QObject *parent = 0);
	
	virtual void Spawn();
	virtual void RenderMe(void){}
	virtual void Step(void) {}
	
signals:
	
public slots:
	void HideMe(void) {m_Visible = false;}
	void ShowMe(void) {m_Visible =  true;}
};

#endif // CBASERENDERABLE_H
