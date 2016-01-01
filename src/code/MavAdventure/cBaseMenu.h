#ifndef CBASEMENU_H
#define CBASEMENU_H

#include <cBaseRenderable.h>

#include <QVector>
#include <QString>
#include <QVector2D>

class QFont;
struct custom_font_s;

enum ALIGN_TYPE
{
	ALIG_LEFT = 0,
	ALIG_CENTER,
	ALIG_RIGHT
};

class cBaseMenu : public cBaseRenderable
{
	Q_OBJECT
public:
	
	static QMap<QString,QByteArray> s_Locale;
	static custom_font_s *s_pMenuFont;
	
	explicit cBaseMenu(QObject *parent = 0);
	~cBaseMenu();
	
	QVector2D m_Orig;
	QFont *m_pFont;
	
	int  m_Align;
	
	QVector<QByteArray>m_Items;
	int m_Cursor;
	bool m_Ready;
	//virtual void ItemSelected(const QString &item);
	void RenderMe();
	void Step();
	inline bool WarpCursor(void)
	{
		if (m_Cursor < 0)
		{
			m_Cursor = m_Items.size()-1;
			emit menuOut(true);
			return true;
		}
		if (m_Cursor > m_Items.size()-1)
		{
			m_Cursor = 0;
			emit menuOut(false);
			return true;
		}
		return false;
	}

signals:
	void itemSelected(QString item);
	void menuOut(bool up);
public slots:
	virtual void keyPressed(int key);
	void keyReleased(int key);
};

#endif // CBASEMENU_H
