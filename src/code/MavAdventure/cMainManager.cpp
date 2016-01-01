#include <QDir>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "cMainManager.h"
#include <cTileManager.h>
#include "cBigDialog.h"

#include <objectTypes.h>

#include <math.h>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QElapsedTimer>
#include <QGLFunctions>
#include "cGameRoom.h"
#include <QImage>
#include <QGLContext>

#include <QOpenGLShader>
#include <QOpenGLShaderProgram>
#include <QOpenGLFramebufferObject>

#include <QKeyEvent>
#include <QMap>
#include <QTextCodec>
#include <QVector2D>

#include <util.h>

#include "cConsole.h"
#include "QThread"

extern QMap<int, quint8> gKeysMap;
extern QMap<int, quint8> gKeysMap_Old;
extern QVector2D gMouse;
extern MainWindow *gpMainWND;
bool g_Paused = false;

QElapsedTimer gElTmr;
QElapsedTimer gSimTmr;
QElapsedTimer gFpsTmr;
int g_Fps, g_CounterFps;
//QElapsedTimer gTailTmr;

cMainManager *cMainManager::g_pMainMgr = 0;
cBigDialog *g_pBigDialog = 0;

int gTestTileset;
QOpenGLFramebufferObject *g_pBuf;
QOpenGLShaderProgram *g_pFinalShader;

QThread *g_pConThread;

//bool g_DebugWalls;

void cMainManager::Map(const QString &mapname)
{
	cGameRoom *prevRoom = NULL;
	if (m_pCurrentRoom->m_Name != "main_menu")
	{
		prevRoom = m_pCurrentRoom;
	}
	if (mapname == "main_menu")
	{
		m_pCurrentRoom = MGR->RoomByName("main_menu");
	}
	else
	{
		QString mapfile = "res/maps/"+mapname;
		if (!mapfile.contains(".2dl")) mapfile += ".2dl";
		
		g_pCon->PrintLine("loading map "+mapfile);
		cGameRoom *newRoom = cGameRoom::LoadFromFile(mapfile);
		if (newRoom)
		{
			m_pCurrentRoom = newRoom;
			if (prevRoom) delete prevRoom;
		}
		else
			g_pCon->PrintLine("map "+mapfile+" not found");
	}
}

void cMainManager::ProcessInput(const QString &cmd)
{
	//g_pConThread->start();
	//g_pConInputTimer->stop();
	
	QStringList cmds = cmd.split(" ",QString::SkipEmptyParts);
	if (!cmds.size()) return;
	//qDebug()<<"cmds[0].toLatin1()"<<cmds[0].toLatin1();
	cvar_t *cvar = UTIL_GetCvar(cmds[0].toLatin1());
	if (cvar)
	{
		if (cmds.size() <= 1)
		{
			g_pCon->PrintLine(cmds[0] + " is " + cvar->value_s + " ( " + cvar->value_def + " )");
			return;
		}
		cvar->set(cmds[1].toLatin1());
		return;
	}
	
	if (cmds[0] == "map")
	{
		if (cmds.size() > 1)
		{	
			for(int i=0;i<g_TrasnferList.size();i++)
				delete g_TrasnferList[i];
			g_TrasnferList.clear();
			MGR->Map(cmds[1]);
		}
		else
			g_pCon->PrintLine("current map: "+m_pCurrentRoom->m_Name);
		return;
	}
	
//	if (cmds[0] == "debug_walls")
//	{
//		if (cmds.size() <= 1)
//		{
//			g_pCon->PrintLine("debug_walls is "+QString::number(g_DebugWalls));
//			return;
//		}
		
//		bool ok, state;
//		state = cmds[1].toInt(&ok);
//		if (!ok) return;
//		g_DebugWalls = state;
//	}
	if (cmds[0] == "quit")
	{
		QApplication::quit();
		return;
	}
	if (cmds[0] == "boots")
	{
		g_BootsUpgrade = !g_BootsUpgrade;
		g_fBootsUpgTime = m_Time + 1.f;
		return;
	}
	if (cmds[0] == "dlg")
	{
		//if (cmds.size() == 1) goto unknown;
		
		//if (m_pCurrentRoom->m_Name == "main_menu")
//			return;
		//cmds[1].replace("_"," ");
		
		g_pBigDialog->AddDialog(QDir::currentPath()+"/res/sprites/big_icons/mav/neutral.png",
				"Мэврик",
				"Согласен, Изо - темный тип. Но его клан в некотором роде представляет для меня небольшой интерес. Всё таки у них есть группа по фристайлу.",
				false,false,true);
		g_pBigDialog->AddDialog(QDir::currentPath()+"/res/sprites/big_icons/hellen/neutral.png",
				"Хэлен",
				"Не надо, Мэв. Я слышала, этот Изо - тот еще эгоист. Он использует новобранцев, а потом кидает их, забирая всё заработанное себе. И еще - мне кажется он затевает что-то темное, сообщество Кибер-Геймера постепенно \"отравляется\".",
				false,false,!true);
		g_pBigDialog->AddDialog(QDir::currentPath()+"/res/sprites/big_icons/mav/neutral.png",
				"Мэврик",
				"Я не пойду к нему в клан, не переживай так за меня.",
				false,false,true);
		g_pBigDialog->AddDialog(QDir::currentPath()+"/res/sprites/big_icons/hellen/neutral.png",
				"Хэлен",
				"Я верю тебе.",
				false,false,!true);
		g_pBigDialog->AddDialog(QDir::currentPath()+"/res/sprites/big_icons/hellen/neutral.png",
				"Хэлен",
				"Тут, кстати, намечается одно событие. LAN-Party, командный плэй-офф 2 на 2, призом которого являецо - не поверишь - Изумруд Хаоса. Тот самый.",
				false,false,!true);
		return;
	}
unknown:
	g_pCon->PrintLine("Unknown command: "+cmds[0]);
}



cMainManager::cMainManager(QWidget *parent) :
	QGLWidget(parent)
{
	g_pConThread = new QThread;
	g_pCon = new cConsole;
	g_pCon->moveToThread(g_pConThread);
	//connect(g_pConThread,SIGNAL(started()),g_pCon,SLOT(AskInput()));
	connect(g_pCon,SIGNAL(HasInput(QString)),this,SLOT(ProcessInput(QString)));
	g_pConThread->start();
	
	DBG_PRINT("Installing main manager...");
	g_pMainMgr = this;
	
	srand(QTime::currentTime().msec());
	
	m_pCurrentRoom = NULL;
	
	m_DeltaTime = 0.f;
	m_FrameTime = 0.f;
	
	m_pStepTimer = new QTimer;
	m_pStepTimer->setTimerType(Qt::PreciseTimer);
	m_pStepTimer->setInterval(1000/60);
	connect(m_pStepTimer,SIGNAL(timeout()),this,SLOT(UpdateEverything()));
	m_pStepTimer->setSingleShot(false);
	m_pStepTimer->start();
	
	gElTmr.start();
	gSimTmr.start();
	gFpsTmr.start();
	m_Time = 0;
	
	m_MiddleFade = m_UpperFade = QVector4D(0,0,0,0);
	
	m_pGLFns = NULL;
	
	drawColorPush(QVector3D(1.f,1.f,1.f));
	drawFontPush(new QFont("Arial", 10));
	
	setMouseTracking(true);
	
	DBG_PRINT("Main manager installed");
	
	
}
cMainManager::~cMainManager()
{
	m_pStepTimer->stop();
	QFont *pf;
	while((pf = drawFontPop()) != NULL) delete pf;
//	while(true)//m_pRooms.size())
//	{
//		if (m_pRooms.isEmpty()) break;
//		cGameRoom *pr = m_pRooms[0];
//		m_pRooms.pop_front();
//		delete pr;
//	}
	delete m_pStepTimer;
	
}

void cMainManager::mouseMoveEvent(QMouseEvent *me)
{
	QPoint m = me->pos();
	gMouse = QVector2D(m.x(),m.y());
}
cGameRoom *cMainManager::AddRoom(QString name)
{
	cGameRoom *pRm = new cGameRoom;
	
	if (!name.isEmpty())
		pRm->m_Name = name;
	
	m_pRooms.push_back(pRm);
	
	return pRm;
}
cGameRoom *cMainManager::RoomByName(QString name)
{
	//qDebug()<<"m_pRooms.size()"<<m_pRooms.size();
	for(int i=m_pRooms.size()-1;i>=0;i--)
	{
		if (m_pRooms[i]->m_Name == name) return m_pRooms[i];
	}
	return NULL;
}
void cMainManager::DeleteRoom(QString name)
{
	for(int i=m_pRooms.size()-1;i>=0;i--)
		if (m_pRooms[i]->m_Name == name)
		{
			cGameRoom *pRm = m_pRooms[i];
			m_pRooms.removeAt(i);
			delete pRm;
			return;
		}
}
void cMainManager::DeleteRoom(cGameRoom *room)
{
	DeleteRoom(room->m_Name);
}
void cMainManager::drawRect(const QVector2D &v1, const QVector2D &v2)
{
	glBindTexture(GL_TEXTURE_2D,0);
	glBegin(GL_QUADS);
	glVertex2f(floor(v1.x()),floor(v1.y()));
	glVertex2f(floor(v1.x()),floor(v2.y()));
	glVertex2f(floor(v2.x()),floor(v2.y()));
	glVertex2f(floor(v2.x()),floor(v1.y()));
	glEnd();
}
void cMainManager::ChangelevelDelayed()
{
	QString mappath = QDir::currentPath()+m_NextChMap;
	cGameRoom *prevRoom = NULL;
	if (m_pCurrentRoom->m_Name != "main_menu")
	{
		prevRoom = m_pCurrentRoom;
		//EnableInput();
	}
	
	cGameRoom *newRoom = cGameRoom::LoadFromFile(mappath);
	if (newRoom)
	{
		m_pCurrentRoom = newRoom;
		//EnableInput();
		
		cFader *pFader = (cFader *)m_pCurrentRoom->CreateObject("env_fade");
		pFader->m_FadeFlags = FFADE_IN;
		pFader->m_FadeSec   = 0.25f;
		pFader->m_Layer     = FDL_MIDDLE;
		connect(pFader,SIGNAL(fadeDone()),this,SLOT(EnableInput()));
		pFader->Fade();
		
		if (prevRoom) prevRoom->m_Flags |= FL_KILLME;
		
	}
	else
	{
		DBG_PRINT_S("ERROR Changelevel: map "+mappath+" not found");
		m_pCurrentRoom = MGR->RoomByName("main_menu");
		g_LandmarkName.clear();
		EnableInput();
	}
}
void cMainManager::Changelevel(const QString &mapname, bool fade)
{
	if (fade)
	{
		cFader *pFader = (cFader *)m_pCurrentRoom->CreateObject("env_fade");
		pFader->m_FadeFlags = FFADE_OUT | FFADE_STAYOUT;
		pFader->m_FadeSec   = 0.25f;
		pFader->m_Layer     = FDL_MIDDLE;
		m_NextChMap = mapname;
		connect(pFader,SIGNAL(fadeDone()),this,SLOT(ChangelevelDelayed()));
		
		pFader->Fade();
	}
	else
	{
		EnableInput();
		
		QString mappath = QDir::currentPath()+mapname;
		cGameRoom *prevRoom = NULL;
		if (m_pCurrentRoom->m_Name != "main_menu")
		{
			prevRoom = m_pCurrentRoom;
		}
		cGameRoom *newRoom = cGameRoom::LoadFromFile(mappath);
		if (newRoom)
		{
			m_pCurrentRoom = newRoom;
			if (prevRoom) prevRoom->m_Flags |= FL_KILLME;
		}
		else
		{
			DBG_PRINT_S("ERROR Changelevel: map "+mappath+" not found");
			m_pCurrentRoom = MGR->RoomByName("main_menu");
			g_LandmarkName.clear();
		}
	}
}
void cMainManager::drawLine(const QVector2D &v1, const QVector2D &v2)
{
	glBindTexture(GL_TEXTURE_2D,0);
	glBegin(GL_LINES);
	glVertex2f(floor(v1.x()),floor(v1.y()));
	glVertex2f(floor(v2.x()),floor(v2.y()));
	glEnd();
}
//void cMainManager::drawTextOutline(int x, int y, const QString &text,TEXT_ALIGN align, const QVector3D &outColor)
//{
//	QFontMetrics fm(*m_pFonts[0]);
	
//	if (align == ETA_RIGHT)
//		x -= fm.width(text);
//	if (align == ETA_CENTER)
//		x -= fm.width(text)*0.5f;
	
//	renderText(x,y,text,*m_pFonts[0]);
	
//	renderText(x,y,text,*m_pFonts[0]);
//}

void cMainManager::drawTextCustom(int x, int y, const QByteArray &text, custom_font_s *fnt,TEXT_ALIGN align)
{
	QByteArray text_simp = text.simplified();
	//qDebug()<<"text_simp"<<text_simp;
	if (align == ETA_RIGHT)
		x -= fnt->strWidth(text_simp);
	if (align == ETA_CENTER)
		x -= fnt->strWidth(text_simp)*0.5f;
	
	int num = std::min(text_simp.size(),256);
	
	float verts[2048];
	float tcoords[2048];
	float u_add, v_add = fnt->height;
	v_add /= fnt->tex_height;
	
	int xx = x;
	int w;
	uchar idx;
	for(int i=0;i<num;i++)
	{
		idx = text_simp.at(i);
		w = fnt->height;//fnt->width[idx];
		//qDebug()<<idx<<"w"<<w;
		// X                 // Y
		verts[i*8  ] = xx  ; verts[i*8+1] = y;
		verts[i*8+2] = xx  ; verts[i*8+3] = y+fnt->height;
		verts[i*8+4] = xx+w; verts[i*8+5] = y+fnt->height;
		verts[i*8+6] = xx+w; verts[i*8+7] = y;
		
		u_add = w; u_add /= fnt->tex_width;
		
		tcoords[i*8  ] = fnt->texcoords[idx].x()      ; tcoords[i*8+1] = fnt->texcoords[idx].y()      ;
		tcoords[i*8+2] = fnt->texcoords[idx].x()      ; tcoords[i*8+3] = fnt->texcoords[idx].y()+v_add;
		tcoords[i*8+4] = fnt->texcoords[idx].x()+u_add; tcoords[i*8+5] = fnt->texcoords[idx].y()+v_add;
		tcoords[i*8+6] = fnt->texcoords[idx].x()+u_add; tcoords[i*8+7] = fnt->texcoords[idx].y()      ;
		
		xx += fnt->width[idx];//w;
	}
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	m_pGLFns->glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,fnt->texture);
	glEnableClientState(GL_VERTEX_ARRAY); glVertexPointer(2,GL_FLOAT,0,verts);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY); glTexCoordPointer(2,GL_FLOAT,0,tcoords);
	glDrawArrays(GL_QUADS,0,num*4);
	//delete [] verts;
	//delete [] tcoords;
}
void cMainManager::drawText(int x, int y, const QString &text,TEXT_ALIGN align)
{
	QFontMetrics fm(*m_pFonts[0]);
	
	if (align == ETA_RIGHT)
		x -= fm.width(text);
	if (align == ETA_CENTER)
		x -= fm.width(text)*0.5f;
				
	renderText(x,y,text,*m_pFonts[0]);
}
void cMainManager::UpdateEverything()
{
	updateGL();
	
	//float dt;
	//dt = gElTmr.nsecsElapsed()/1000000.f;
	//qDebug()<<"dt"<<dt;
	//dt *= 0.00001f;
//	float sec = dt*0.001f;
//	m_FrameTime += (sec - m_FrameTime)*0.5f;
//	if (m_FrameTime > 0.1f) m_FrameTime = 0.1f;
//	if (m_FrameTime < 0.f) m_FrameTime = 0.f;
	
//	dt = float(int(dt))/16.666667f;
	
	m_FrameTime = 1.f/60.f;//0.014f;
			
	m_Time = 0.001f*(float)gSimTmr.elapsed();
//	m_DeltaTime += (dt - m_DeltaTime)*0.5f;
//	//m_DeltaTime  = int(m_DeltaTime*100)/100.f;
//	if (m_DeltaTime > 2) m_DeltaTime = 2;
	m_DeltaTime = 0.9f*70.f/60.f;
	gElTmr.restart();
}
bool cMainManager::IsTileExists(const QVector2D &pos, int depth)
{
	return false;//m_pCurrentRoom->m_pTileMngr->IsTileExists(pos,depth);
}
void cMainManager::initializeGL()
{
	UTIL_DeclareCvar("debug_walls","0");
	
	if (!g_pBigDialog)
	{
		DBG_PRINT("Adding BIG-Dialog...");
		g_pBigDialog = new cBigDialog;
		//DBG_PRINT(SIGNAL(keyPressed(int)));
		connect(this,SIGNAL(keyPressed(int)),g_pBigDialog,SLOT(KeyPressed(int)));
		connect(this,SIGNAL(keyReleased(int)),g_pBigDialog,SLOT(KeyReleased(int)));
		//connect(this,SIGNAL(TestSignal(int,int)),g_pBigDialog,SLOT(TestSlot(float)));
	}
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	m_pGLFns = this->context()->functions();
	resizeGL(640,480);
	glClearColor(0.f,0.f,0.f,1.f);
	
	g_pBuf = new QOpenGLFramebufferObject(640,480,QOpenGLFramebufferObject::Depth);
	g_pBuf->format().setMipmap(true);
	////

	/*cFader *pFd = (cFader *)m_pCurrentRoom->CreateObject("env_fade");
	pFd->m_FadeSec = 0.5f;
	pFd->m_Flags |= FFADE_IN;
	connect(m_pCurrentRoom,SIGNAL(roomStart()),pFd,SLOT(Fade()));
	connect(pFd,SIGNAL(fadeDone()),this,SLOT(EnableInput()));*/
	
	//for(int i=0;i<5;i++)
	//	m_pCurrentRoom->CreateObject("ring");
	
//	m_pCurrentRoom = cGameRoom::LoadFromFile("res/maps/new_lvl_test.2dl");
//	m_pCurrentRoom->CreateObject("mav");
//	//m_pCurrentRoom->SetBlock(EBS_RIGHT,true);
//	//m_pCurrentRoom->SetBlock( EBS_LEFT,true);
//	m_pCurrentRoom->SetBlock( EBS_DOWN,true);
	
	
	m_pCurrentRoom = AddRoom("main_menu");
	m_pCurrentRoom->AddBack("res/backs/title.png");
	cChangelevel *pCHLVLTrig = 
			(cChangelevel *)m_pCurrentRoom->CreateObject("trigger_changelevel");
	pCHLVLTrig->m_NextRoom = "mav_house_floor1";
	
	cBaseAnimating *pCursorL = 
			(cBaseAnimating *)m_pCurrentRoom->CreateObject("base_anim");
	cBaseAnimating *pCursorR = 
			(cBaseAnimating *)m_pCurrentRoom->CreateObject("base_anim");
	
	pCursorL->AddAnim("res/sprites/menu/cursor.png","idle",8);
	pCursorL->m_ImageScale = QVector2D(-1,1);
	pCursorL->m_ImageSpeed = 0.5f;
	pCursorL->m_Orig = QVector2D(0,240);
	pCursorR->AddAnim("res/sprites/menu/cursor.png","idle",8);
	pCursorR->m_ImageSpeed = 0.5f;
	pCursorR->m_Orig = QVector2D(640,240);
	
	m_pCurrentRoom->ObjectSetTargetname(pCursorL,"cur_l");
	m_pCurrentRoom->ObjectSetTargetname(pCursorR,"cur_r");
	
	cBaseMenu *pMainMenu = (cBaseMenu *)m_pCurrentRoom->CreateObject("mainmenu");
	pMainMenu->m_Visible = true;
	pMainMenu->m_pFont = new QFont("Verdana", 18);
	pMainMenu->m_Align = ALIG_CENTER;
	pMainMenu->m_Orig = QVector2D(320,220);
	
	cStringTrigger *pFreeMTrig = 
			(cStringTrigger *)m_pCurrentRoom->CreateObject("string_trigger");
	pFreeMTrig->m_Filter = "#FreePlay";

	connect(pMainMenu,SIGNAL(itemSelected(QString)),
			  pFreeMTrig,SLOT(CheckString(QString)));
	
	
	
	cBaseMenu *pContrMenu = (cBaseMenu *)m_pCurrentRoom->CreateObject("controlls");
	pContrMenu->m_pFont = new QFont("Verdana", 18);
	pContrMenu->m_Align = ALIG_CENTER;
	pContrMenu->m_Orig = QVector2D(320,60);
	
	cStringTrigger *pOptTrig = 
			(cStringTrigger *)m_pCurrentRoom->CreateObject("string_trigger");
	pOptTrig->m_Filter = "#Controls";
	
	connect(pMainMenu,SIGNAL(itemSelected(QString)),
			  pOptTrig,SLOT(CheckString(QString)));
	connect(pOptTrig,SIGNAL(Activated()),
			  pMainMenu,SLOT(HideMe()));
	connect(pOptTrig,SIGNAL(Activated()),
			  pContrMenu,SLOT(ShowMe()));
	
	cStringTrigger *pOptBackTrig = 
			(cStringTrigger *)m_pCurrentRoom->CreateObject("string_trigger");
	pOptBackTrig->m_Filter = "#Cancel";
	
	connect(pContrMenu,SIGNAL(itemSelected(QString)),
			  pOptBackTrig,SLOT(CheckString(QString)));
	connect(pOptBackTrig,SIGNAL(Activated()),
			  pMainMenu,SLOT(ShowMe()));
	connect(pOptBackTrig,SIGNAL(Activated()),
			  pContrMenu,SLOT(HideMe()));
	
	cStringTrigger *pOptOkTrig = 
			(cStringTrigger *)m_pCurrentRoom->CreateObject("string_trigger");
	pOptOkTrig->m_Filter = "#OK";
	
	connect(pContrMenu,SIGNAL(itemSelected(QString)),
			  pOptOkTrig,SLOT(CheckString(QString)));
	connect(pOptOkTrig,SIGNAL(Activated()),
			  pMainMenu,SLOT(ShowMe()));
	connect(pOptOkTrig,SIGNAL(Activated()),
			  pContrMenu,SLOT(HideMe()));
	
	
	/*cFader *pFd = (cFader *)m_pCurrentRoom->CreateObject("env_fade");
	pFd->m_FadeSec = 0.5f;
	pFd->m_Flags |= FFADE_OUT | FFADE_STAYOUT;
	connect(pFd,SIGNAL(roomStart()),pFd,SLOT(Fade()));*/
	
	connect(pFreeMTrig,SIGNAL(Activated()),
			  pCHLVLTrig,SLOT(ChangeLevel()));
	/*		  this,SLOT(DisableInput()));
	connect(pFreeMTrig,SIGNAL(Activated()),
			  pFd,SLOT(Fade()));
	connect(pFd,SIGNAL(fadeDone()),
			  pCHLVLTrig,SLOT(ChangeLevel()));*/
	
//	QString str(
//"-_.,;:!?\\/\"[]{}()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя"
//						);
	
//	custom_font_s(QDir::currentPath()+"/res/fonts/dialog_font.png");
	
//	QByteArray arr = str.toUtf8();
// //	uchar text[147] = "-_.,;:!?\\/\"[]{}()0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя";
//	for(int i=0;i<arr.size();i++)
//		qDebug()<<arr[i]<<" is "<<(uchar)arr[i];
	
//	for(int i=0;i<146;i++)
//	{
//		char ch[2] = {text[i],'\0'};
//		qDebug()<<ch<<" is "<<(int)text[i];
//	}
	
//	for(int i=0;i<256;i++)
//		qDebug()<<char(i)<<" is "<<i;
	
	//qDebug()<<"’"<<(uint)'’';
	
	QMap<QByteArray,QByteArray>::iterator it;
	for(it = g_Args.begin(); it != g_Args.end(); it++)
	{
		QByteArray cmd = it.key()+' '+it.value();
		//qDebug()<<"cmd"<<cmd;
		ProcessInput(cmd);
	}
}
QOpenGLShaderProgram * cMainManager::CreateShader(QString vertex, QString pixel)
{
	DBG_PRINT(("New shader: "+vertex+", "+pixel).toUtf8());
	QOpenGLShaderProgram *pShProg = new QOpenGLShaderProgram;
	pShProg->addShaderFromSourceFile(QOpenGLShader::Vertex,vertex);
	pShProg->addShaderFromSourceFile(QOpenGLShader::Fragment,pixel);
	pShProg->link();
	//	QFile shlog("shader_log.txt");
	//	shlog.open(QFile::WriteOnly);
	//	QTextStream out(&shlog);
	//	out << pShProg->log();
	//	shlog.close();
	DBG_PRINT(("Compile log: "+pShProg->log()).toUtf8());
	
	return pShProg;
}
void cMainManager::resizeGL(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,640,480,0,-4096,4096);
	
	int act_w = w,act_h = h;
	
	//qDebug()<<"w,h"<<gpMainWND->ui->centralWidget->size().width()<<(gpMainWND->ui->centralWidget->size().height()*1.33333333f);
	
	if (w < h*1.33333333f)
		act_h = w/1.33333333f;
	else
		act_w = h*1.33333333f;
	
	m_ViewPort = QPoint(act_w,act_h);
	
	glViewport(0,0,act_w,act_h);
	
	setGeometry(
				gpMainWND->ui->centralWidget->x(),
				gpMainWND->ui->centralWidget->y(),
				w,h);
}

void cMainManager::paintGL()
{
	glViewport(0,0,640,480);
	
	g_pBuf->bind();
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	QList<int>keysLst = gKeysMap.keys();
	for(int i=keysLst.size()-1;i>=0;i--)
	{
		gKeysMap[keysLst[i]] &= ~(KEY_PRESSED | KEY_RELEASED);
		
		if ((gKeysMap[keysLst[i]] & KEY_ISDOWN) && !(gKeysMap_Old[keysLst[i]] & KEY_ISDOWN))
		{
			gKeysMap[keysLst[i]] |= KEY_PRESSED;
			emit keyPressed(keysLst[i]);
		}
		if (!(gKeysMap[keysLst[i]] & KEY_ISDOWN) && (gKeysMap_Old[keysLst[i]] & KEY_ISDOWN)) 
		{
			emit keyReleased(keysLst[i]);
			gKeysMap[keysLst[i]] |= KEY_RELEASED;
		}
		//gKeysMap[keysLst[i]] &= ~(KEY_ISDOWN | KEY_ISUP);
		
		//		if (gKeysMap[keysLst[i]] & KEY_PRESSED)
		//		{
		//			gKeysMap[keysLst[i]] |= KEY_ISDOWN;
		//			gKeysMap[keysLst[i]] &= ~KEY_ISUP;
		//		}
		//		if (gKeysMap[keysLst[i]] & KEY_RELEASED)
		//		{
		//			gKeysMap[keysLst[i]] |= KEY_ISUP;
		//			gKeysMap[keysLst[i]] &= ~KEY_ISDOWN;
		//		}
		//		
	}
	
	if (m_pCurrentRoom)
	{
		m_pCurrentRoom->RoomUpdate();
	}
	
	//qDebug()<<"m_MiddleFade"<<m_MiddleFade;
	
	if (m_MiddleFade.w() > 0.f)
	{
		glColor4f(m_MiddleFade.x(),m_MiddleFade.y(),m_MiddleFade.z(),m_MiddleFade.w());
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glVertex2f(  0,  0);
		glVertex2f(640,  0);
		glVertex2f(640,480);
		glVertex2f(  0,480);
		glEnd();
		glColor3f(1.f,1.f,1.f);
	}
	g_pBigDialog->RenderDialog();
	if (m_UpperFade.w() > 0.f)
	{
		glColor4f(m_UpperFade.x(),m_UpperFade.y(),m_UpperFade.z(),m_UpperFade.w());
		glDisable(GL_TEXTURE_2D);
		glBegin(GL_QUADS);
		glVertex2f(  0,  0);
		glVertex2f(640,  0);
		glVertex2f(640,480);
		glVertex2f(  0,480);
		glEnd();
		glColor3f(1.f,1.f,1.f);
	}
	glColor3f(1.f,1.f,1.f);
	
	g_pBuf->release();
	
	glViewport(0,0,m_ViewPort.x(),m_ViewPort.y());
	
	glBindTexture(GL_TEXTURE_2D,g_pBuf->texture());
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	
	drawTexture(QRectF(0,0,640,480),g_pBuf->texture());
	/*g_pFinalShader->bind();
	drawTexture(QRectF(0,0,640,480),g_pBuf->texture());
	g_pFinalShader->release();*/
	
	gKeysMap_Old = gKeysMap;
	
	//drawText(0,12,"fps: "+QString::number(g_Fps));
	
	g_CounterFps++;
	if (gFpsTmr.elapsed() > 1000)
	{
		g_Fps = g_CounterFps;
		g_CounterFps = 0;
		gFpsTmr.restart();
	}
	
	//drawText(0,12,QString::number(m_pCurrentRoom->m_View.orig.x()));
	//if (m_pCurrentRoom) drawText(0,24,"ROOM: "+m_pCurrentRoom->m_Name);
	
	//drawTexture(QRectF(0,0,64,64),gTestTileset);
}

void cMainManager::DisableInput()
{
	g_InputActive = false;
	
	QList<int>keys = gKeysMap.keys();
	for(int i=keys.size()-1;i>=0;i--)
	{
		gKeysMap[keys[i]] = KEY_ISUP;
	}
}



custom_font_s::custom_font_s(const QString &filename)
{
	memset(width,0,256);
	memset(texcoords,0,256*sizeof(QVector2D));
	texture = 0;
	
	QImage img(QDir::currentPath()+filename);
	if (img.isNull()) return;
	//QByteArray ref();
	uchar ref[] = "!\"(),-./[\\]_{}:;?0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyzАБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЪЫЬЭЮЯабвгдеёжзийклмнопрстуфхцчшщъыьэюя *'";
	
	//QTextCodec *codec = QTextCodec::codecForName("Windows-1251");
	//QByteArray ref = codec->fromUnicode(str);
	
	int w = img.width()/13;
	height = w;
	tex_width  = img.width();
	tex_height = img.height();
	int idx;
	for(int j=0;j<13;j++)
	{
		for(int i=0;i<13;i++)
		{
			for(int k=0;k<w;k++)
			{
				if (qGreen(img.pixel(i*w+k,j*w)) == 255)
				{
					//qDebug()<<"coords"<<i*w+k<<j*w;
					//qDebug()<<ref[j*13+i]<<"width is"<<k;
					img.setPixel(i*w+k,j*w,qRgba(0,0,0,0));
					idx = ref[j*13+i];
					width[idx] = k+1;
					texcoords[idx][0] = i*w; texcoords[idx][0] /= (float)img.width();
					texcoords[idx][1] = j*w; texcoords[idx][1] /= (float)img.height();
					break;
				}
			}
		}
	}
	texture = MGR->bindTexture(img,GL_TEXTURE_2D,GL_RGBA,QGLContext::NoBindOption);
}
