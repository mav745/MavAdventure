#ifndef TOOLBUTTONS_H
#define TOOLBUTTONS_H

#include "cBaseToolButton.h"
#include <cLineTool.h>
#include <cTileTool.h>
#include <cBacksTool.h>
#include <cTriggsTool.h>
#include <cEntityTool.h>


class cLineToolButton : public cBaseToolButton
{
	Q_OBJECT
public:
	explicit cLineToolButton(QWidget *parent = 0);
	
signals:
	
public slots:
	void AssignMe();
};
class cTileToolButton : public cBaseToolButton
{
	Q_OBJECT
public:
	explicit cTileToolButton(QWidget *parent = 0);
	
signals:
	
public slots:
	void AssignMe();
};
class cBacksToolButton : public cBaseToolButton
{
	Q_OBJECT
public:
	explicit cBacksToolButton(QWidget *parent = 0);
	
signals:
	
public slots:
	void AssignMe();
};
class cTriggsToolButton : public cBaseToolButton
{
	Q_OBJECT
public:
	explicit cTriggsToolButton(QWidget *parent = 0);
	
signals:
	
public slots:
	void AssignMe();
};
class cEntitiesToolButton : public cBaseToolButton
{
	Q_OBJECT
public:
	explicit cEntitiesToolButton(QWidget *parent = 0);
	
signals:
	
public slots:
	void AssignMe();
};
class cRoomToolButton : public cBaseToolButton
{
	Q_OBJECT
public:
	explicit cRoomToolButton(QWidget *parent = 0);
	
signals:
	
public slots:
	void AssignMe();
};

#endif //TOOLBUTTONS_H