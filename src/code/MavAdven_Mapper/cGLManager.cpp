#include <cBaseTool.h>
#include "cGLManager.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QElapsedTimer>
#include <QKeyEvent>
#include <QMessageBox>
#include <QOpenGLShaderProgram>
#include <QVector2D>

#include <QDebug>
#include <math.h>

#include <keys.h>
#include <QKeyEvent>

#include <QGLFunctions>

#include <objecttypes.h>

#include <cTriggsTool.h>
#include <cBacksTool.h>
#include <cLineTool.h>
#include <cTileTool.h>
#include <cBackground.h>
#include <cTileLayer.h>
#include <QFileDialog>

#include <QWindow>


#define REVISION_2DL 2

inline int  &QPoint::operator[](int i)
{
	if (i == 0) return xp;
	return yp;
}

QElapsedTimer gElTmr;
QElapsedTimer gSimTmr;

extern MainWindow *gpMainWND;
extern QVector2D gMouse;
QVector2D gMousePrev;
QVector2D gMouseMove;

QMap<int, quint8> gKeysMap;
QMap<int, quint8> gKeysMap_Old;

cBaseTool *g_CurrentTool;

cGLManager *g_pMgr;

QString g_lastSaved;

template <class T> T min(T v1, T v2) {if (v1<v2) return v1; return v2;}
template <class T> T max(T v1, T v2) {if (v1>v2) return v1; return v2;}
template <class T> T sgn(T v) 
{
	if (v < (T)0) return (T)-1;
	if (v > (T)0) return (T) 1;
	return (T)0;
}
void fwritestr(QString *buf, FILE *pf)
{
	int sz = buf->length();
	fwrite(&sz,4,1,pf);
	fwrite(buf->toUtf8().data(),1,sz,pf);
}

void freadstr(QString *buf, FILE *pf)
{
	int sz;
	fread(&sz,4,1,pf);
	qDebug()<<"sz"<<sz;
	char *str = new char[sz+1];
	str[sz] = 0;
	fread(str,1,sz,pf); //filename
	qDebug()<<"str"<<str;
	*buf = QString(str);
	delete str;
}
cGLManager::cGLManager(QWidget *parent) :
	QGLWidget(parent)
{
	g_pMgr = this;
	
	setMouseTracking(true);
	m_pStepTimer = new QTimer;
	m_pStepTimer->setTimerType(Qt::PreciseTimer);
	m_pStepTimer->setInterval(1000/60);
	connect(m_pStepTimer,SIGNAL(timeout()),this,SLOT(UpdateEverything()));
	m_pStepTimer->setSingleShot(false);
	m_pStepTimer->start();
	m_GridSize = 2;
	m_EditMode = EEM_NOTHING;
	
	drawColorPush(QVector3D(1.f,1.f,1.f));
	drawFontPush(new QFont("Arial", 10));
	
	setFocusPolicy(Qt::WheelFocus);
	
	m_Zoom = 1;
	m_RoomSize = QVector2D(640,480);
	
	m_ResizeHor = m_ResizeVer = false;
	//g_CurrentTool = new
	
	connect(gpMainWND->ui->menuBar,SIGNAL(triggered(QAction*)),this,SLOT(menuAction(QAction*)));
}
cGLManager::~cGLManager()
{
	m_pStepTimer->stop();
	QFont *pf;
	while((pf = drawFontPop()) != NULL) delete pf;
	delete m_pStepTimer;
	
}
QOpenGLShaderProgram * cGLManager::CreateShader(QString vertex, QString pixel)
{
	//DBG_PRINT(("New shader: "+vertex+", "+pixel).toUtf8());
	QOpenGLShaderProgram *pShProg = new QOpenGLShaderProgram;
	pShProg->addShaderFromSourceFile(QOpenGLShader::Vertex,vertex);
	pShProg->addShaderFromSourceFile(QOpenGLShader::Fragment,pixel);
	pShProg->link();
//	QFile shlog("shader_log.txt");
//	shlog.open(QFile::WriteOnly);
//	QTextStream out(&shlog);
	qDebug() << pShProg->log();
//	shlog.close();
	//DBG_PRINT(("Compile log: "+pShProg->log()).toUtf8());
	
	return pShProg;
}
void cGLManager::SetObjectDepth(cBase *obj, int depth)
{
	obj->m_Depth = depth;
	qSort(m_Objects.begin(),m_Objects.end(),depthCompare);
}
void cGLManager::ProcessInput(void)
{
	if (g_CurrentTool) g_CurrentTool->ProcessInput();
	
	if (hasFocus())
	{
		bool shift_down   = !!(MGR->KeyState(Qt::Key_Shift  ) & KEY_ISDOWN);
		bool control_down = !!(MGR->KeyState(Qt::Key_Control) & KEY_ISDOWN);
		bool alt_down     = !!(MGR->KeyState(Qt::Key_Alt    ) & KEY_ISDOWN);
		bool space_down   = !!(MGR->KeyState(Qt::Key_Space  ) & KEY_ISDOWN);
		
		if (KeyState(Qt::Key_Escape) & KEY_PRESSED)
		{
			gpMainWND->close();
			return;
		}
		if (!shift_down &&  control_down && !alt_down && !space_down)
		{
			//qDebug() << KeyState(MWHEEL_UP) << KeyState(MWHEEL_DOWN);
			if ((KeyState(Qt::Key_0) & KEY_PRESSED) ||
				(KeyState(Qt::RightButton) & KEY_PRESSED))
			{
				m_Zoom = 1.f;
				
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(0,m_ViewSize.x()*m_Zoom,m_ViewSize.y()*m_Zoom,0,-4096,4096);
			}
			if (KeyState(MWHEEL_UP) & KEY_PRESSED)
			{
				m_Zoom += 0.2f;
				if (m_Zoom > 5.f) m_Zoom = 5.f;
				
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(0,m_ViewSize.x()*m_Zoom,m_ViewSize.y()*m_Zoom,0,-4096,4096);
			}
			if (KeyState(MWHEEL_DOWN) & KEY_PRESSED)
			{
				m_Zoom -= 0.2f;
				if (m_Zoom < 0.4f) m_Zoom = 0.4f;
				
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				glOrtho(0,m_ViewSize.x()*m_Zoom,m_ViewSize.y()*m_Zoom,0,-4096,4096);
			}
		}
		if (!shift_down && !control_down && !alt_down && !space_down)
		{
			if (KeyState(Qt::Key_G) & KEY_PRESSED)
				m_Vis.grid = !m_Vis.grid;
			if (KeyState(Qt::Key_A) & KEY_PRESSED)
				m_Vis.axis = !m_Vis.axis;
			if (KeyState(Qt::Key_BracketLeft) & KEY_PRESSED)
				m_GridSize -= 1;
			if (KeyState(Qt::Key_BracketRight) & KEY_PRESSED)
				m_GridSize += 1;
			if (m_GridSize < 0) m_GridSize = 0;
			if (m_GridSize > 4) m_GridSize = 4;
			/*int key = -1;
			if (KeyState(Qt::Key_1) & KEY_PRESSED) key = 1;
			if (KeyState(Qt::Key_2) & KEY_PRESSED) key = 2;
			if (KeyState(Qt::Key_3) & KEY_PRESSED) key = 3;
			if (KeyState(Qt::Key_4) & KEY_PRESSED) key = 4;
			if (KeyState(Qt::Key_5) & KEY_PRESSED) key = 5;
			if (key != -1)//((e->key() >= Qt::Key_1) && (e->key() <= Qt::Key_4))
			{
				m_Zoom = 6-key;//5-(e->key() - 0x30);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				
				glOrtho(0,640*m_Zoom,480*m_Zoom,0,-4096,4096);
			}*/
		}
		
		QCursor cur;
		cur.setShape(Qt::ArrowCursor);
		if (!space_down && !shift_down && !control_down && !alt_down)
		{
			if (m_Vis.axis && g_CurrentTool && g_CurrentTool->m_Name == "room")
			{
				if (KeyState(Qt::LeftButton) & KEY_RELEASED)
					m_ResizeHor = m_ResizeVer = false;
				
				QVector2D mcrds = MouseCoords();
				bool 
						closeH = fabs(mcrds.x() - m_RoomSize.x()) < 8.f, 
						closeV = fabs(mcrds.y() - m_RoomSize.y()) < 8.f;
				if (m_ResizeHor && m_ResizeVer)
				{
					cur.setShape(Qt::SizeFDiagCursor);
					if (KeyState(Qt::LeftButton) & KEY_PRESSED)
						m_ResizeHor = m_ResizeVer = true;
				}
				else if (closeH && closeV)
				{
					cur.setShape(Qt::SizeFDiagCursor);
					if (KeyState(Qt::LeftButton) & KEY_PRESSED)
						m_ResizeHor = m_ResizeVer = true;
				}
				else
				{
					if (closeH || m_ResizeHor)
					{
						cur.setShape(Qt::SizeHorCursor);
						if (KeyState(Qt::LeftButton) & KEY_PRESSED)
							m_ResizeHor = true;
					}
					if (closeV || m_ResizeVer)
					{
						cur.setShape(Qt::SizeVerCursor);
						if (KeyState(Qt::LeftButton) & KEY_PRESSED)
							m_ResizeVer = true;
					}
				}
			}
		}
		if (m_ResizeHor) 
		{
			float sz = m_RoomSize.x();
			m_RoomSize.setX(MouseCoords(true).x());
			//qDebug()<<"sizes"<<sz<<m_RoomSize.x();
			if (sz != m_RoomSize.x())
				emit roomSizeChanged(m_RoomSize);
		}
		if (m_ResizeVer)
		{
			float sz = m_RoomSize.y();
			m_RoomSize.setY(MouseCoords(true).y());
			if (sz != m_RoomSize.y())
				emit roomSizeChanged(m_RoomSize);
		}
		
		if (space_down)
		{
			if (!shift_down && !control_down && !alt_down)
			{
				cur.setShape(Qt::OpenHandCursor);
				if (KeyState(Qt::LeftButton) & KEY_ISDOWN)
				{
					cur.setShape(Qt::ClosedHandCursor);
					for(int i=0;i<2;i++)
					{
						m_View[i] += gMouseMove[i]*m_Zoom;
						//m_View[i] = min(0.f,m_View[i]);
					}
				}
			}
		}
		setCursor(cur);
	}
}
void cGLManager::RegisterInput(int key, bool released)
{
	if (!released)
	{
		gKeysMap[key] |= KEY_ISDOWN | KEY_PRESSED;
		gKeysMap[key] &= ~KEY_ISUP;
	}
	else
	{
		gKeysMap[key] |= KEY_ISUP | KEY_RELEASED;
		gKeysMap[key] &= ~KEY_ISDOWN;
	}
}
cBase *cGLManager::AllocObject(QString name)
{
	cBase *pObj = NULL;
		  if (name == "line"        ) pObj = new cLine       ;
	else if (name == "segment"     ) pObj = new cSegment    ;
	else if (name == "par_expand"  ) pObj = new cParExpand  ;
	else if (name == "axial_expand") pObj = new cAxialExpand;
	else if (name == "tile_layer"  ) pObj = new cTileLayer  ;
	else if (name == "background"  ) pObj = new cBackground ;
	else if (name == "trigger"     ) pObj = new cBaseTrigger;
	else if (name == "entity"      ) pObj = new cBaseEntity ;
		  
	if (pObj != NULL)
	{
		pObj->m_Classname = name;
		m_Objects.push_back(pObj);
		qSort(m_Objects.begin(),m_Objects.end(),depthCompare);
	}
	else qDebug()<<"NULL in AllocObject!";
	
	return pObj;
}
void cGLManager::RemoveObject(cBase *obj)
{
	obj->m_Flags |= FL_KILLME;
	//m_Objects.removeAt(m_Objects.indexOf(obj));
	//delete obj;
}
void cGLManager::ObjectsByClassname(QString classname, QVector<cBase *> &vec)
{
	int sz = m_Objects.size();
	for(int i=0;i<sz;i++)
	{
		if (m_Objects[i]->m_Classname == classname)
			vec.push_back(m_Objects[i]);
	}
}
void cGLManager::drawText(int x, int y, const QString &text,TEXT_ALIGN align)
{
	QFontMetrics fm(*m_pFonts[0]);
	
	if (align == ETA_RIGHT)
		x -= fm.width(text);
	if (align == ETA_CENTER)
		x -= fm.width(text)*0.5f;
				
	renderText((x+MGR->m_View.x())/MGR->m_Zoom,(y+MGR->m_View.y())/MGR->m_Zoom,text,*m_pFonts[0]);
}
QVector2D cGLManager::MouseCoords(bool snapped)
{
	QVector2D m_vm = m_Mouse*m_Zoom - m_View;
			
	if (!snapped)
		return m_vm;
	
	float grd = GridCellSize();
	float halfgrd = 0.f;//grd*0.5f;
	QPoint m_m((m_vm.x()+sgn(m_vm.x())*halfgrd)/grd,(m_vm.y()+sgn(m_vm.y())*halfgrd)/grd);
	return QVector2D(m_m.x()*grd,m_m.y()*grd);
}
void cGLManager::DrawLine(float x1,float  y1,float  x2,float  y2)
{
	DrawLine(QVector2D(x1,y1), QVector2D(x2,y2));
}
void cGLManager::FillRect(QVector2D v1, QVector2D v2)
{
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(m_View.x(),m_View.y(),0.f);
	glBegin(GL_QUADS);
	
	glVertex2f(v1.x(),v1.y());
	glVertex2f(v1.x(),v2.y());
	glVertex2f(v2.x(),v2.y());
	glVertex2f(v2.x(),v1.y());
	
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glLoadIdentity();
}
void cGLManager::OutlineRect(QVector2D v1, QVector2D v2)
{
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(m_View.x(),m_View.y(),0.f);
	glBegin(GL_LINE_LOOP);
	
	glVertex2f(v1.x(),v1.y());
	glVertex2f(v1.x(),v2.y());
	glVertex2f(v2.x(),v2.y());
	glVertex2f(v2.x(),v1.y());
	
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glLoadIdentity();
}
void cGLManager::DrawLine(QVector2D v1, QVector2D v2)
{
	glDisable(GL_TEXTURE_2D);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(m_View.x(),m_View.y(),0.f);
	glBegin(GL_LINES);
	glVertex2f(v1.x(),v1.y());
	glVertex2f(v2.x(),v2.y());
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glLoadIdentity();
}
void cGLManager::wheelEvent(QWheelEvent *we)
{
	//qDebug() << we->angleDelta();
	if (we->angleDelta().y() < 0)
		gKeysMap[MWHEEL_UP] |= KEY_PRESSED;
	else
		gKeysMap[MWHEEL_DOWN] |= KEY_PRESSED;
}
void cGLManager::mouseDoubleClickEvent(QMouseEvent *me)
{
	if (me->button() == Qt::LeftButton)
		gKeysMap[DCLICK_LEFT] |= KEY_PRESSED;
	if (me->button() == Qt::RightButton)
		gKeysMap[DCLICK_RIGHT] |= KEY_PRESSED;
}

void cGLManager::mousePressEvent(QMouseEvent *me){RegisterInput(me->button());}
void cGLManager::mouseReleaseEvent(QMouseEvent *me){RegisterInput(me->button(),true);}
void cGLManager::keyPressEvent(QKeyEvent *e){RegisterInput(e->key());}
void cGLManager::keyReleaseEvent(QKeyEvent *e){RegisterInput(e->key(),true);}
void cGLManager::mouseMoveEvent(QMouseEvent *me)
{
	QPoint m = me->pos();
	m_Mouse = QVector2D(m.x(),m.y());
}
void cGLManager::initializeGL()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	m_pGLFns = this->context()->functions();
	resizeGL(640,480);
	glClearColor(0.f,0.f,0.f,1.f);
	
	for(int i=0;i<5;i+=1)
	{
		//qDebug()<<"res/editor/grid_"+QString().number(pow(2,i+3))+".png";
		m_GridTexs[i] = bindTexture(
					QImage("res/editor/grid_"+QString().number(pow(2,i+3))+".png"));//,
					//GL_TEXTURE_2D,GL_RGBA,QGLContext::InvertedYBindOption);
		//qDebug()<<"m_GridTexs["<<i<<"]"<<m_GridTexs[i];
	}
	glEnable(GL_TEXTURE_2D);
}
void cGLManager::resizeGL(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0,w*m_Zoom,h*m_Zoom,0,-4096,4096);
	
	glViewport(0,0,w,h);
	
	m_ViewSize[0] = w;
	m_ViewSize[1] = h;
	
	setGeometry(
				gpMainWND->ui->glFrame->pos().x()+1,
				gpMainWND->ui->glFrame->pos().y()+1
				+gpMainWND->ui->menuBar->height(),
				w,h);
	//gpMainWND->ui->glFrame->resize(1,1);
}
int  cGLManager::KeyState(int key)
{
	if (!gKeysMap.keys().contains(key))
	{
		gKeysMap[key] = KEY_ISUP;
		//return 0;
	}
	return gKeysMap[key];
}
void cGLManager::paintGL()
{
	gMouseMove = m_Mouse - gMousePrev;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
//	QList<int>keysLst = gKeysMap.keys();
//	for(int i=keysLst.size()-1;i>=0;i--)
//	{
//		gKeysMap[keysLst[i]] &= ~(KEY_PRESSED | KEY_RELEASED);
		
//		if ((gKeysMap[keysLst[i]] & KEY_ISDOWN) && !(gKeysMap_Old[keysLst[i]] & KEY_ISDOWN))
//		{
//			gKeysMap[keysLst[i]] |= KEY_PRESSED;
//			//emit keyPressed(keysLst[i]);
//		}
//		if (!(gKeysMap[keysLst[i]] & KEY_ISDOWN) && (gKeysMap_Old[keysLst[i]] & KEY_ISDOWN)) 
//		{
//			//emit keyReleased(keysLst[i]);
//			gKeysMap[keysLst[i]] |= KEY_RELEASED;
//		}
//	}
	
	ProcessInput();
	
	float wr,hr;
	float ox,oy;
	ox = m_View.x()-1;
	oy = m_View.y()+1;
	float x,y;
	x  = -ox/128.f;
	y  = -oy/128.f;
	x -= floor(x);
	y -= floor(y);
	wr = m_Zoom*m_ViewSize.x()/128.f;
	hr = m_Zoom*m_ViewSize.y()/128.f;
	ox = min(max(ox,1.f),m_ViewSize.x()*m_Zoom-2.f);
	oy = min(max(oy,2.f),m_ViewSize.y()*m_Zoom-2.f);
	
	
	
	//TODO: draw map content
	int sz = m_Objects.size();
	for(int i=0;i<sz;i++)
	{
		if (m_Objects[i]->m_Flags & FL_KILLME)
		{
			if (m_Objects[i]->m_pEntity)
				delete m_Objects[i]->m_pEntity;
			delete m_Objects[i];
			m_Objects.removeAt(i);
			sz = m_Objects.size();
			i--;
			continue;
		}
		m_Objects[i]->RenderMe();
	}
	
	if (g_CurrentTool)
		g_CurrentTool->DrawMe();
	
	if (m_Vis.grid)
	{
		glColor3f(1,1,1);
		glBindTexture(GL_TEXTURE_2D,m_GridTexs[m_GridSize]);
		//glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glBegin(GL_QUADS);
		
		glTexCoord2f(x,y);
		glVertex2f(   .0f,   .0f);
		
		glTexCoord2f(x+wr,y);
		glVertex2f(m_Zoom*m_ViewSize.x(),   .0f);
		
		glTexCoord2f(x+wr,y+hr);
		glVertex2f(m_Zoom*m_ViewSize.x(),m_Zoom*m_ViewSize.y());
		
		glTexCoord2f(x,y+hr);
		glVertex2f(   .0f,m_Zoom*m_ViewSize.y());
		
		glEnd();
	}
	if (m_Vis.axis)
	{
		float r,d;
		r = m_RoomSize.x();
		d = m_RoomSize.y();
		
		r = min(-m_View.x()+(m_ViewSize.x()-2.f)*m_Zoom,r);
		d = min(-m_View.y()+(m_ViewSize.y()-2.f)*m_Zoom,d);
		
		//glColor3f(0.5f,0.5f,0.5f);
		//glLineWidth(2.f);
		glColor3f(0.0f,0.0f,0.0f);
		DrawLine(r+1,0+1,r+1,d+1);
		DrawLine(0+1,d+1,r+1,d+1);
		DrawLine(0+1,0+1,0+1,d+1);
		DrawLine(0+1,0+1,r+1,0+1);
		glColor3f(1.0f,1.0f,1.0f);
		DrawLine(r,0,r,d);
		DrawLine(0,d,r,d);
		DrawLine(0,0,0,d);
		DrawLine(0,0,r,0);
		//glLineWidth(1.f);
	}
//	{
//		glColor4f(0.f,1.0f,0.f,0.5f);
//		glDisable(GL_TEXTURE_2D);
//		glBegin(GL_LINES);
//		glVertex2f(ox+1,  0);
//		glVertex2f(ox+1,m_Zoom*480);
//		glEnd();
		
//		glColor4f(1.0f,0.f,0.f,0.5f);
//		glBegin(GL_LINES);
//		glVertex2f(0,oy-1);
//		glVertex2f(m_Zoom*640,oy-1);
//		glEnd();
		
//		glColor3f(1,1,1);
		
//		//glMatrixMode(GL_MODELVIEW);
//		//glLoad
////		glBegin(GL_LINES);
////		glVertex2f(640,  0);
////		glVertex2f(640,480);
////		glVertex2f(  0,480);
////		glVertex2f(640,480);
////		glEnd();
		
//		//DrawLine(QVector2D(640,0),QVector2D(640,480));
//		//DrawLine(QVector2D(0,480),QVector2D(640,480));
		
//		glEnable(GL_TEXTURE_2D);
//	}
	
	gMousePrev = m_Mouse;
	gKeysMap_Old = gKeysMap;
	
	if (!hasFocus())
	{
		glColor3f(0.5f,0.5f,0.5f);
		drawText(4,16,"Disabled");
	}
	
	QList<int>keysLst = gKeysMap.keys();
	for(int i=keysLst.size()-1;i>=0;i--)
	{
		gKeysMap[keysLst[i]] &= ~(KEY_PRESSED | KEY_RELEASED);
	}
}
void cGLManager::UpdateEverything()
{
	resizeGL(
				gpMainWND->ui->glFrame->width(),
				gpMainWND->ui->glFrame->height());
			
	updateGL();
	
	float dt;
	dt = gElTmr.nsecsElapsed()/1000000.f;
	//qDebug()<<"dt"<<dt;
	//dt *= 0.00001f;
	dt = float(int(dt))/16.666667f;
	
	m_Time += 0.9f*70.f/60.f;//0.001f*(float)gSimTmr.elapsed();
	//m_DeltaTime += (dt - m_DeltaTime)*0.5f;
	//m_DeltaTime  = int(m_DeltaTime*100)/100.f;
	//if (m_DeltaTime > 2) m_DeltaTime = 2;
	
	gElTmr.restart();
}
void cGLManager::menuAction(QAction *ac)
{
	if (ac->text() == "Save ass")
	{
		SaveAss();
		return;
	}
	else if (ac->text() == "Save")
	{
		Save();
		return;
	}
	else if (ac->text() == "Open")
	{
		Open();
		return;
	}
	else if (ac->text() == "New")
	{
		g_lastSaved.clear();
		gpMainWND->setWindowTitle("new unsaved level...");
		
		g_pBacksTool->ClearALL();
		g_pTileTool->ClearALL();
		g_pLineTool->ClearALL();
		g_pTriggsTool->ClearALL();
		g_pEntityTool->ClearALL();
		m_RoomSize = QVector2D(640,480);
		for(int i=g_pTileTool->m_Layers.size()-1;i>=0;i--)
			g_pTileTool->m_Layers[i]->resizeLayer(m_RoomSize);
		m_View = QVector2D(0,0);
		return;
	}
}
void cGLManager::Open()
{
	QString fname = QFileDialog::getOpenFileName(
				gpMainWND,"Game levels",
				QDir::currentPath(),tr("2DL (*.2dl)"));
	if (fname.isEmpty()) return;
	
	QFile pf(fname);
	if (pf.open(QFile::ReadOnly))
	{
		QDataStream DS(&pf);
		int rev;
		int sz;
		int iv;
		QString qstr;
		
		DS>>qstr;
		if (qstr != "2DL")
		{
			QMessageBox msgBox;
			msgBox.setIcon(QMessageBox::Warning);
			msgBox.setWindowTitle("Error");
			msgBox.setText("Wrong file type.");
			msgBox.exec();
			return;
		}
		//fread(&rev,4,1,pf);
		DS>>rev;
		if (rev != REVISION_2DL)
		{
			QMessageBox msgBox;
			msgBox.setIcon(QMessageBox::Warning);
			msgBox.setWindowTitle("Error");
			msgBox.setText("Wrong file revision.");
			msgBox.exec();
			return;
		}
		
		g_lastSaved = fname;
		gpMainWND->setWindowTitle(g_lastSaved);
		
		g_pBacksTool->ClearALL();
		g_pTileTool->ClearALL();
		g_pLineTool->ClearALL();
		g_pTriggsTool->ClearALL();
		g_pEntityTool->ClearALL();
		
		DS>>sz;//fread(&sz,4,1,pf); //num backs
		for(int i=0;i<sz;i++)
		{
			
			DS>>qstr;//freadstr(&qstr,pf);
			DS>>iv;//fread(&iv,4,1,pf); //depth
			cBackground *pB = g_pBacksTool->AddBack(QDir::currentPath()+qstr,iv);

			DS>>pB->m_Origin>>pB->m_ScrFac>>pB->m_ScrSpd>>pB->m_HRep>>pB->m_VRep;
		}
		
		DS>>sz;	//fread(&sz,4,1,pf); //num layers
		for(int i=0;i<sz;i++)
		{
			DS>>qstr;//freadstr(&qstr,pf);
			DS>>iv;//fread(&iv,4,1,pf); //depth
			
			cTileLayer *pL = g_pTileTool->AddLayer(QDir::currentPath()+qstr,iv);
			
			DS>>pL->m_ScrollFactor;	//fread(&pL->m_ScrollFactor[0],4,1,pf);
			//fread(&pL->m_ScrollFactor[1],4,1,pf);
			
			int w,h;
			DS>>w>>h;	//fread(&w,4,1,pf);
			//fread(&h,4,1,pf);
			
			QImage *pImg = new QImage(w,h,QImage::Format_RGB888);
			DS.readRawData((char*)pImg->bits(),pImg->byteCount());//fread(pImg->bits(),1,w*h*3,pf);
			delete pL->m_TileMap;
			pL->m_TileMap = pImg;
			pL->updateTileMapTex();
		}
		
		DS>>sz;	//fread(&sz,4,1,pf); //num lines
		for(int i=0;i<sz;i++)
		{
			cSegment *pL = (cSegment *)GET_OBJECT(segment);
			DS>>pL->m_S>>pL->m_E>>pL->m_JumpDown;
			pL->CalcMe();
			g_pLineTool->m_Lines.push_back(pL);
		}
		int sz2;
		DS>>sz;	//fread(&sz,4,1,pf); //numtriggers
		for(int i=0;i<sz;i++)
		{
			cBaseTrigger *pT = (cBaseTrigger *)GET_OBJECT(trigger);
			DS>>sz2;	//fread(&sz2,4,1,pf); //numparts
			for(int j=0;j<sz2;j++)
			{
				float sx,sy,ex,ey;
				DS>>sx;
				DS>>sy;
				DS>>ex;
				DS>>ey;
				pT->AddRectShape(QVector2D(sx,sy),QVector2D(ex,ey));
			}
			g_pTriggsTool->m_Triggs.push_back(pT);
			
			DS>>qstr;	//freadstr(&qstr,pf);
			//qDebug()<<"type"<<qstr;
			pT->SetEntityType(qstr);
			DS>>sz2;
			//qDebug()<<"reading fields";
			for(int i = 0;i<sz2/*pT->m_pEntity->fields.size()*/;i++)
			{
				QString filed, value;
				DS>>filed;	//freadstr(&filed,pf);
				DS>>value;	//freadstr(&value,pf);
				if (pT->m_pEntity->fields.contains(filed))
					pT->m_pEntity->fields[filed]->fromString(value);
			}
		}
		g_pTriggsTool->m_SelectedTriggs.clear();
		
		DS>>sz;	//fread(&sz,4,1,pf); //numentities
		for(int i=0;i<sz;i++)
		{
			cBaseEntity *pT = (cBaseEntity *)GET_OBJECT(entity);
			int dp;
			DS>>pT->m_Orig>>dp;
			SET_OBJECT_DEPTH(pT,dp);
			g_pEntityTool->m_Ents.push_back(pT);
			
			DS>>qstr;	//freadstr(&qstr,pf);
			//qDebug()<<"type"<<qstr;
			pT->SetEntityType(qstr);
			
			//qDebug()<<"reading fields";
			DS>>sz2;
			for(int i = 0;i<sz2/*pT->m_pEntity->fields.size()*/;i++)
			{
				QString filed, value;
				DS>>filed;	//freadstr(&filed,pf);
				DS>>value;	//freadstr(&value,pf);
				
				if (filed.left(1) == "@") continue;
				
				if (pT->m_pEntity->fields.contains(filed))
					 pT->m_pEntity->fields[filed]->fromString(value);
			}
		}
		
		DS>>m_RoomSize;	//fread(&m_RoomSize[0],4,1,pf);
		if (!DS.atEnd())
		{
			bool chkHor,chkDown,chkUp;
			DS>>chkHor>>chkDown>>chkUp;
			gpMainWND->ui->chk_HorBlock->setChecked(chkHor);
			gpMainWND->ui->chk_DownBlock->setChecked(chkDown);
			gpMainWND->ui->chk_UpBlock->setChecked(chkUp);
		}
		//fread(&m_RoomSize[1],4,1,pf);
		
		for(int i=g_pTileTool->m_Layers.size()-1;i>=0;i--)
			g_pTileTool->m_Layers[i]->resizeLayer(m_RoomSize);
		
		//fclose(pf);
		pf.close();
	}
}
void cGLManager::Save()
{
	if (g_lastSaved.isEmpty()) SaveAss();
	
	gpMainWND->setWindowTitle(g_lastSaved);
	
	//FILE *pf = fopen(g_lastSaved.toUtf8().data(),"wb");
	
	QFile pf(g_lastSaved+".new");
	if (pf.open(QFile::WriteOnly))
	{
		QDataStream DS(&pf);
		
		int rev = REVISION_2DL;
		int sz;
		float fv;
		int iv;
		
		DS<<QString("2DL");
		DS<<rev;
		
		sz = g_pBacksTool->m_Backs.size();
		DS<<sz;
		for(int i=0;i<sz;i++)
		{
			DS		<<g_pBacksTool->m_Backs[i]->m_Filename
				  <<g_pBacksTool->m_Backs[i]->m_Depth
				 <<g_pBacksTool->m_Backs[i]->m_Origin
				<<g_pBacksTool->m_Backs[i]->m_ScrFac
			  <<g_pBacksTool->m_Backs[i]->m_ScrSpd
			 <<g_pBacksTool->m_Backs[i]->m_HRep
			<<g_pBacksTool->m_Backs[i]->m_VRep;
		}
		sz = g_pTileTool->m_Layers.size();
		DS<<sz;
		for(int i=0;i<sz;i++)
		{
			//qDebug()<<"size"<<g_pTileTool->m_Layers[i]->m_TileMap->byteCount();
			//qDebug()<<"my size"<<g_pTileTool->m_Layers[i]->m_TileMap->width()*
//						 g_pTileTool->m_Layers[i]->m_TileMap->height()*3;
			DS		<<g_pTileTool->m_Layers[i]->m_FName
				  <<g_pTileTool->m_Layers[i]->m_Depth
				 <<g_pTileTool->m_Layers[i]->m_ScrollFactor
				<<g_pTileTool->m_Layers[i]->m_TileMap->size();
			DS.writeRawData((char*)g_pTileTool->m_Layers[i]->m_TileMap->bits(),
								 g_pTileTool->m_Layers[i]->m_TileMap->byteCount());
//			QString mapname = g_pTileTool->m_Layers[i]->m_FName;
//			mapname.resize(mapname.size()-4);
//			mapname += ".map";
//			qDebug()<<g_pTileTool->m_Layers[i]->m_TileMap->size();
//			//qDebug()<<QDir::currentPath()+mapname;
//			QFile pf2(QDir::currentPath()+mapname);
//			pf2.open(QFile::WriteOnly);
//			QDataStream DS2(&pf2);
//			DS2.writeRawData((char*)g_pTileTool->m_Layers[i]->m_TileMap->bits(),
//								 g_pTileTool->m_Layers[i]->m_TileMap->byteCount());
//			pf2.close();;
		}
		
		sz = g_pLineTool->m_Lines.size();
		DS<<sz;
		for(int i=0;i<sz;i++)
		{
			DS
					<<g_pLineTool->m_Lines[i]->m_S
				  <<g_pLineTool->m_Lines[i]->m_E
				 <<g_pLineTool->m_Lines[i]->m_JumpDown;
		}
		
		int sz2;
		sz = g_pTriggsTool->m_Triggs.size();
		DS<<sz;
		for(int i=0;i<sz;i++)
		{
			sz2 = g_pTriggsTool->m_Triggs[i]->m_Parts.size();
			DS<<sz2;
			for(int j=0;j<sz2;j++)
			{
				DS
						<<g_pTriggsTool->m_Triggs[i]->m_Parts[j]->s
					  <<g_pTriggsTool->m_Triggs[i]->m_Parts[j]->e;
			}

			DS<<g_pTriggsTool->m_Triggs[i]->m_pEntity->type_name;//fwritestr(&g_pTriggsTool->m_Triggs[i]->m_pEntity->type_name,pf);
			DS<<g_pTriggsTool->m_Triggs[i]->m_pEntity->fields.size();
			QMap<QString, value_s *>::iterator it;
			for(it  = g_pTriggsTool->m_Triggs[i]->m_pEntity->fields.begin();
				 it != g_pTriggsTool->m_Triggs[i]->m_pEntity->fields.end();
				 it ++)
			{
				QString name, val;
				name = it.key();
				if (name.startsWith('@')) continue; //don't save system fields
				
				val  = it.value()->asString();
				
				DS<<name<<val;
			}
		}
		
		sz = g_pEntityTool->m_Ents.size();
		DS<<sz;
		for(int i=0;i<sz;i++)
		{
			DS
					<<g_pEntityTool->m_Ents[i]->m_Orig
				  <<g_pEntityTool->m_Ents[i]->m_Depth;
					  
			entity_t *pEnt = g_pEntityTool->m_Ents[i]->m_pEntity;
			
			DS<<pEnt->type_name;

			DS<<pEnt->fields.size();
			QMap<QString, value_s *>::iterator it;
			for(it  = pEnt->fields.begin();
				 it != pEnt->fields.end();
				 it ++)
			{
				QString name, val;
				name = it.key();
				val  = it.value()->asString();
				
				DS<<name<<val;
			}
		}
		DS
				<<m_RoomSize
			  <<gpMainWND->ui->chk_HorBlock->isChecked()
			 <<gpMainWND->ui->chk_DownBlock->isChecked()
			<<gpMainWND->ui->chk_UpBlock->isChecked();

		pf.close();
	}
	QFile::remove(g_lastSaved+".bak");
	QFile::rename(g_lastSaved,g_lastSaved+".bak");
	QFile::rename(g_lastSaved+".new",g_lastSaved);
//	QFile pf_in(g_lastSaved+".new");
//	if (pf_in.open(QFile::ReadOnly))
//	{
//		QByteArray data = pf_in.readAll();
//		if (data.size() > 0)
//		{
//			QFile pf_out(g_lastSaved);
//			if (pf_out.open(QFile::WriteOnly))
//			{
//				QDataStream DS_out(&pf_out);
//				DS_out<<data;
//			}
//			pf_out.close();
//		}
//		pf_in.close();
//	}
	
}
void cGLManager::SaveAss()
{
	QString fname = QFileDialog::getSaveFileName(
				gpMainWND,"Game levels",
				QDir::currentPath(),tr("2DL (*.2dl)"));
	if (fname.isEmpty()) return;
	
	g_lastSaved = fname;
}
