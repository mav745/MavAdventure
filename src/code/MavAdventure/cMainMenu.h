#ifndef CMAINMENU_H
#define CMAINMENU_H

#include "cBaseMenu.h"
#include <QMap>

class cMainMenu : public cBaseMenu
{
	Q_OBJECT
public:
	explicit cMainMenu(QObject *parent = 0);
	void RenderMe();
public slots:
	void Selection(QString item);
};

class cOptionsMenu : public cBaseMenu
{
	Q_OBJECT
public:
	explicit cOptionsMenu(QObject *parent = 0);
};


class cControllsMenu : public cBaseMenu
{
	Q_OBJECT
public:
	
	static custom_font_s *s_KeysFont;
	
	explicit cControllsMenu(QObject *parent = 0);
	
	QMap<int, QString>m_keyNames;
	
	//key_set_s m_KeySets[8];
	
	QVector2D m_KeyOrigs[8];
	int m_NewKeys[8];
	
	int m_JoyTex;
	QVector2D m_TexOrigs[8];
	int m_ButTexs[8];
	
	bool m_Wait;
	
	int m_MaxWidth;
	
	void RenderMe();
public slots:
	void keyPressed(int key);
	void Selection(QString item);
};

#endif // CMAINMENU_H
