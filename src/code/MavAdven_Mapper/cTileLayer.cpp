#include "cTileLayer.h"
#include <QImage>
#include "cGLManager.h"
#include "cTileTool.h"
#include <QDebug>
#include <QDir>
#include <QOpenGLShaderProgram>

#include <QGLFunctions>

#include <QPainter>

extern cTileTool *g_pTileTool;
extern bool g_ScrollEf;

QOpenGLShaderProgram *g_pTileShader;

cTileLayer::cTileLayer(QObject *parent) : cBase(parent)
{
	m_Visible = true;
	m_TileMap = new QImage(40,30,QImage::Format_RGB888);
	QPainter p(m_TileMap);
	p.fillRect(0,0,40,30,QColor(0,0,0));
	m_TMapTexture = MGR->bindTexture(
				*m_TileMap//);
				,GL_TEXTURE_2D,GL_RGBA,QGLContext::NoBindOption/*InvertedYBindOption*/);
	m_Classname = "tile_layer";
	m_Texture = 0;//
	m_FName.clear();
	m_Width = m_Height = 0;
	m_TSetImg = NULL;
	m_ScrollFactor = QVector2D(1.f,1.f);
	m_Orig = QVector2D(0,0);
	connect(MGR,SIGNAL(roomSizeChanged(QVector2D)),this,SLOT(resizeLayer(QVector2D)));
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
					GL_TEXTURE_2D,GL_RGB,QGLContext::NoBindOption/*InvertedYBindOption*/);
	}
}

void cTileLayer::resizeLayer(QVector2D sz)
{
	//qDebug()<<"resizeLayer"<<sz;
	sz /= 16.f;
	QImage *pnewTileMap = new QImage(sz.x(),sz.y(),QImage::Format_RGB888);
	QPainter p(pnewTileMap);
	p.setCompositionMode(QPainter::CompositionMode_Source);
	p.fillRect(0,0,sz.x()+2,sz.y()+2,QColor(0,0,0));
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


bool cTileLayer::RenderMe()
{
	if (!m_Visible) return false;
	
	GLint indc[] = {0,1,2,3};
	
	if (g_ScrollEf)
		m_Orig = -MGR->m_View*m_ScrollFactor;
	else
		m_Orig = -MGR->m_View;
	
	m_Orig[1] = -m_Orig[1];
	g_pTileShader->bind();
	g_pTileShader->setUniformValue("u_Zoom",MGR->m_Zoom);
	g_pTileShader->setUniformValue("u_RoomSize",MGR->m_RoomSize);
	g_pTileShader->setUniformValue("u_ViewSize",MGR->m_ViewSize);
	g_pTileShader->setUniformValue("u_Origin",QPoint(m_Orig.x(),m_Orig.y()));
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
	x = MGR->m_ViewSize.x()/*640.f*/*MGR->m_Zoom+16.f;
	y = MGR->m_ViewSize.y()/*480.f*/*MGR->m_Zoom+16.f;
	u = (x)/16.f;
	w = (y+16.f)/16.f;
//	float verts[] = 
//	{
//		-16,-16,
//		  x,-16,
//		  x,  y,
//		-16,  y
//	};
//	float uws[] = 
//	{
//		-1,-1,
//		 u,-1,
//		 u, w,
//		-1, w
//	};
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
	
	//m_pVerts = &g_pTileTool->m_Verts[MGR->m_Zoom];
	//m_pUVs   = &g_pTileTool->m_UVs[MGR->m_Zoom];
	
//	glEnableClientState(GL_VERTEX_ARRAY); 
//	glVertexPointer(2,GL_FLOAT,0,verts);
	
//	glEnableClientState(GL_TEXTURE_COORD_ARRAY); 
//	glTexCoordPointer(2,GL_FLOAT,0,uws);
	
//	glEnableClientState(GL_INDEX_ARRAY);
//	glIndexPointer(GL_INT,0,indc);
	
//	glColor3f(1.f,1.f,1.f);
//	glDrawArrays(GL_QUADS,0,4);
	
	g_pTileShader->release();
//	glDisableClientState(GL_VERTEX_ARRAY); 
//	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//	glDisableClientState(GL_INDEX_ARRAY);
	
	MGR->m_pGLFns->glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	MGR->m_pGLFns->glActiveTexture(GL_TEXTURE0);
	
	return true;
}
