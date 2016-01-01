#include "cTileLayer.h"
#include <QImage>
#include "cGameRoom.h"
#include "cMainManager.h"
#include "util.h"
//#include "cTileTool.h"
#include <QDebug>
#include <QDir>
#include <QOpenGLShaderProgram>

#include <QGLFunctions>

#include <QPainter>

//extern cTileTool *g_pTileTool;
extern bool g_ScrollEf;

QOpenGLShaderProgram *g_pTileShader;

cTileLayer::cTileLayer(QObject *parent) : cBaseObject(parent)
{
	if (!g_pTileShader)
	{
		g_pTileShader = MGR->CreateShader("shaders/tileset.v","shaders/tileset.p");
		g_pTileShader->bind();
		g_pTileShader->setUniformValue("u_ViewSize",QVector2D(640,480));
		g_pTileShader->setUniformValue(g_pTileShader->uniformLocation("s_TileMap"),0);
		g_pTileShader->setUniformValue(g_pTileShader->uniformLocation("s_TileSet"),1);
		g_pTileShader->release();
	}
	m_Visible = true;
	m_TileMap = new QImage(40,30,QImage::Format_RGB888);
	QPainter p(m_TileMap);
	p.fillRect(0,0,40,30,QColor(0,0,0));
	m_TMapTexture = MGR->bindTexture(
				*m_TileMap//);
				,GL_TEXTURE_2D,GL_RGBA,QGLContext::InvertedYBindOption);
	//m_Classname = "tile_layer";
	m_Texture = 0;//
	m_FName.clear();
	m_Width = m_Height = 0;
	m_TSetImg = NULL;
	m_ScrollFac = QVector2D(1.f,1.f);
	m_Orig = QVector2D(0,0);
	//connect(MGR,SIGNAL(roomSizeChanged(QVector2D)),this,SLOT(resizeLayer(QVector2D)));
}
cTileLayer::~cTileLayer()
{
	delete m_TileMap;
}
void cTileLayer::updateTileMapTex(void)
{
	if (m_TMapTexture)
	{
		MGR->deleteTexture(m_TMapTexture);
		m_TMapTexture = MGR->bindTexture(
					*m_TileMap,
					GL_TEXTURE_2D,GL_RGBA,QGLContext::NoBindOption/*InvertedYBindOption*/);
	}
}

void cTileLayer::resizeLayer(QVector2D sz)
{
	sz /= 16.f;
	QImage *pnewTileMap = new QImage(sz.x(),sz.y(),QImage::Format_RGB888);
	QPainter p(pnewTileMap);
	p.setCompositionMode(QPainter::CompositionMode_Source);
	p.fillRect(0,0,sz.x(),sz.y(),QColor(0,0,0));
	p.drawImage(0,0,*m_TileMap);
	delete m_TileMap;
	m_TileMap = pnewTileMap;
	
	updateTileMapTex();
}
void cTileLayer::ShiftLayer(QVector2D sh)
{
	QImage *pnewTileMap = new QImage(m_TileMap->width(),m_TileMap->height(),QImage::Format_RGB888);
	QPainter p(pnewTileMap);
	p.setCompositionMode(QPainter::CompositionMode_Source);
	p.fillRect(0,0,m_TileMap->width(),m_TileMap->height(),QColor(0,0,0));
	p.drawImage(sh.x(),sh.y(),*m_TileMap);
	delete m_TileMap;
	m_TileMap = pnewTileMap;
	
	updateTileMapTex();
}

void cTileLayer::AssignTileSet(QString fname)
{
	if (m_Texture)
	{
		MGR->deleteTexture(m_Texture);
		delete m_TSetImg;
	}
	//fname = fname.replace("/","\\");
	//qDebug()<<"fname"<<fname;
	m_TSetImg = new QImage(fname);
	//qDebug()<<"image"<<m_TSetImg->isNull();
	m_Texture = MGR->context()->bindTexture(
					*m_TSetImg,GL_TEXTURE_2D,GL_RGBA,QGLContext::InvertedYBindOption);
	m_Width = m_TSetImg->width();
	m_Height = m_TSetImg->height();
	m_FName = fname.remove(QDir::currentPath());
}
QPoint cTileLayer::getCrdsByIndex(int idx)
{
	int w = m_Width >> 4;
	return QPoint((idx%w)<<4,(idx/w)<<4);
}
int cTileLayer::getIndexByCrds(QPoint coords)
{
	return coords.x() + coords.y()*(m_Width >> 4);
}
void cTileLayer::Step()
{
	//m_Orig = -MGR->m_View*m_ScrollFactor;
	//qDebug()<<"m_Orig"<<m_Orig;
}

void cTileLayer::RenderMe(void)
{
	if (!m_Visible) return;
	
	//GLint indc[] = {0,1,2,3};
	
	//if (g_ScrollEf)
	
	//else
	//	m_Orig = -MGR->m_View;
	
	m_Orig[0] = -m_Orig[0];
	//m_Orig[1] = -m_Orig[1];
	
	g_pTileShader->bind();
	g_pTileShader->setUniformValue("u_Zoom",1.f);
	g_pTileShader->setUniformValue("u_RoomSize",m_pMyRoom->m_Size);
	g_pTileShader->setUniformValue("u_Origin",QPoint(m_Orig.x()*m_ScrollFac.x(),m_Orig.y()*m_ScrollFac.y()));
	g_pTileShader->setUniformValue("u_TileSetSize",
											 QVector2D(
												 m_TSetImg->size().width()/16.f,
												 m_TSetImg->size().height()/16.f));
	
	MGR->m_pGLFns->glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,m_Texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	
	MGR->m_pGLFns->glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,m_TMapTexture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
	
	glColor3f(1.f,1.f,1.f);
	
	float u,w;
	float x,y;
	x = 640.f+16.f;
	y = 480.f+16.f;
	u = x/16.f;
	w = (y+16.f)/16.f;

	glBegin(GL_QUADS);
	
	glTexCoord2f(0,0);
	glVertex2f(0,-16.f);
	
	glTexCoord2f(u,0);
	glVertex2f(x,-16.f);
	
	glTexCoord2f(u,w);
	glVertex2f(x,y);
	
	glTexCoord2f(0,w);
	glVertex2f(0,y);
	
	glEnd();
	
	g_pTileShader->release();

	MGR->m_pGLFns->glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	MGR->m_pGLFns->glActiveTexture(GL_TEXTURE0);
	
	m_Orig[0] = -m_Orig[0];
	//m_Orig[1] = -m_Orig[1];
	//return;
}
