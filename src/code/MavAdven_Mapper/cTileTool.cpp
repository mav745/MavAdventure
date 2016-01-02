#include "cTileTool.h"
#include <QInputDialog>
#include <mainwindow.h>
#include <ui_mainwindow.h>
#include <cTileLayer.h>
#include <cGLManager.h>
#include <QFileDialog>
#include <QPoint>

#include <QDebug>
#include <QKeyEvent>

#include <keys.h>

#include <QOpenGLShaderProgram>
#include <algorithm>

using namespace std;

#define X_FLIPPED 0
#define Y_FLIPPED 1
#define ROTATED   2

inline int  &QPoint::operator[](int i)
{
	if (i == 0) return xp;
	return yp;
}

cTileTool *g_pTileTool;
cTSetBrowser *g_pTSetBrowser;
cTBrushBrowser *g_pTBrushBrowser;

extern QTimer gStarter;
extern QOpenGLShaderProgram *g_pTileShader;

cTileTool::cTileTool(QObject *parent) :
	cBaseTool(parent)
{
	m_Grab = false;
	m_GrabStart = QVector2D(0,0);
	g_pTileTool = this;
	m_CurLayerIdx = -1;
	connect(gpMainWND->ui->but_layerAdd    ,SIGNAL(clicked()),this,SLOT(addLayer()    ));
	connect(gpMainWND->ui->but_layerDuplicate,SIGNAL(clicked()),this,SLOT(dupLayer()    ));
	connect(gpMainWND->ui->but_layerRemove ,SIGNAL(clicked()),this,SLOT(removeLayer() ));
	connect(gpMainWND->ui->but_layerChangeD,SIGNAL(clicked()),this,SLOT(changeLayer() ));
	connect(gpMainWND->ui->but_layerChangeT,SIGNAL(clicked()),this,SLOT(changeTSet()  ));
	connect(gpMainWND->ui->but_ScrollFac   ,SIGNAL(clicked()),this,SLOT(changeScrFac()));
	connect(gpMainWND->ui->but_layerVisible,SIGNAL(clicked()),
			  this,SLOT(toggleVisibility()));
	connect(gpMainWND->ui->list_layers,SIGNAL(currentTextChanged(QString)),
			  this,SLOT(layerSelected(QString)));
	connect(gpMainWND->ui->but_layerShift,SIGNAL(clicked()),this,SLOT(shiftLayer()));
	
	g_pTileShader = MGR->CreateShader("shaders/tileset.v","shaders/tileset.p");
	g_pTileShader->bind();
	g_pTileShader->setUniformValue(g_pTileShader->uniformLocation("s_TileMap"),0);
	g_pTileShader->setUniformValue(g_pTileShader->uniformLocation("s_TileSet"),1);
	g_pTileShader->release();
	
	
	for(int k=0;k<5;k++)
	{
		m_Verts[k].push_back(      -16); m_Verts[k].push_back(      -16);
		m_Verts[k].push_back(640*(k+1)); m_Verts[k].push_back(      -16);
		m_Verts[k].push_back(640*(k+1)); m_Verts[k].push_back(480*(k+1));
		m_Verts[k].push_back(      -16); m_Verts[k].push_back(480*(k+1));
		
		m_UVs[k].push_back(      -1); m_UVs[k].push_back(      -1);
		m_UVs[k].push_back(40*(k+1)); m_UVs[k].push_back(      -1);
		m_UVs[k].push_back(40*(k+1)); m_UVs[k].push_back(30*(k+1));
		m_UVs[k].push_back(      -1); m_UVs[k].push_back(30*(k+1));

//		for(int i=-1;i<40+40*k+1;i++)
//		{
//			for(int j=-1;j<30+30*k+1;j++)
//			{
//				m_Verts[k].push_back(i*16   ); m_Verts[k].push_back(j*16   );
//				m_Verts[k].push_back(i*16+16); m_Verts[k].push_back(j*16   );
//				m_Verts[k].push_back(i*16+16); m_Verts[k].push_back(j*16+16);
//				m_Verts[k].push_back(i*16   ); m_Verts[k].push_back(j*16+16);
				
//				m_UVs[k].push_back(0); m_UVs[k].push_back(0);
//				m_UVs[k].push_back(1); m_UVs[k].push_back(0);
//				m_UVs[k].push_back(1); m_UVs[k].push_back(1);
//				m_UVs[k].push_back(0); m_UVs[k].push_back(1);
//			}
//		}
	}
}
void cTileTool::ClearALL()
{
	m_CurLayerIdx = -1;
	
	while(m_Layers.size())
	{
		cTileLayer *pLayer = m_Layers[0];
		m_Layers.removeAt(0);
		REMOVE_OBJECT(pLayer);
	}
	UpdateLayerList();
}

void cTileTool::shiftLayer()
{
	if (m_CurLayerIdx == -1) return;
	
	bool ok;
	int shx = QInputDialog::getInt(
				gpMainWND,
				"Shift X",
				"In tiles",
				0, 
				-999999.0,
				 999999.0,
				1, &ok);
	if (ok)
	{
		int shy = QInputDialog::getInt(
					gpMainWND,
					"Shift Y",
					"In tiles",
					0, 
					-999999.0,
					 999999.0,
					1, &ok);
		if (ok)
		{
			m_Layers[m_CurLayerIdx]->ShiftLayer(QVector2D(shx,shy));
		}
	}
}

void cTileTool::toggleVisibility()
{
	if (m_CurLayerIdx == -1) return;
	m_Layers[m_CurLayerIdx]->m_Visible = !m_Layers[m_CurLayerIdx]->m_Visible;
}

void cTileTool::UpdateLayerList()
{
	gpMainWND->ui->list_layers->clear();
	
	int sz = m_Layers.size();
	for(int i=0;i<sz;i++)
	{
		gpMainWND->ui->list_layers->addItem(QString().number(m_Layers[i]->m_Depth));
	}
}

void cTileTool::UpdateTSetList()
{
	
}

void cTileTool::ProcessInput()
{
	if (g_pTSetBrowser) g_pTSetBrowser->ProcessInput();
	if (g_pTBrushBrowser) g_pTBrushBrowser->ProcessInput();
	
	bool shift_down   = !!(MGR->KeyState(Qt::Key_Shift  ) & KEY_ISDOWN);
	bool control_down = !!(MGR->KeyState(Qt::Key_Control) & KEY_ISDOWN);
	bool alt_down     = !!(MGR->KeyState(Qt::Key_Alt    ) & KEY_ISDOWN);
	bool space_down   = !!(MGR->KeyState(Qt::Key_Space  ) & KEY_ISDOWN);
	
	if (MGR->hasFocus())
	{
		if (shift_down && !control_down && !alt_down && !space_down)
		{
			if (m_CurLayerIdx != -1)
			{
				if (MGR->KeyState(Qt::RightButton) & KEY_PRESSED)
				{
					m_Grab = true;
					m_GrabStart = MGR->MouseCoords(true);
				}
			}
		}
		if (m_Grab)
		{
			if (MGR->KeyState(Qt::RightButton) & KEY_RELEASED)
			{
				m_Grab = false;
				QVector2D grabSize = MGR->MouseCoords(true)-m_GrabStart;
				QVector2D grabStart = m_GrabStart;
				for(int i=0;i<2;i++)
					grabStart[i] = fmod(grabStart[i],MGR->m_RoomSize[i]);
				for(int i=0;i<2;i++)
				{
					if (grabSize[i] >  128) grabSize[i] =  128;
					if (grabSize[i] < -128) grabSize[i] = -128;
				}
				QVector2D gS, gE;
				for(int i=0;i<2;i++)
				{
					if (grabSize[i] < 0)
					{
						gS[i] = grabStart[i]+grabSize[i];
						gE[i] = grabStart[i];
					}
					else
					{
						gS[i] = grabStart[i];
						gE[i] = grabStart[i]+grabSize[i];
					}
				}
				int tx,ty,sx,sy;
				g_pTBrushBrowser->clearTiles();
				for(int i=gS.x();i<gE.x();i+=16)
				{
					for(int j=gS.y();j<gE.y();j+=16)
					{
						cTileLayer *pL = m_Layers[m_CurLayerIdx];
						//pL->m_TileMap->save("map.png");
						sx = i>>4;
						sy = j>>4;
						sx %= pL->m_TileMap->width();
						sy %= pL->m_TileMap->height();
						if (sx < 0) sx += pL->m_TileMap->width();
						if (sy < 0) sy += pL->m_TileMap->height();
						tx = int(i-gS.x())>>4;
						ty = int(j-gS.y())>>4;
						
						QRgb color = pL->m_TileMap->pixel(sx,sy);
						
//						uchar *bits = pL->m_TileMap->bits();
//						//qDebug()<<"offs"<<sx<<sy<<(sx+sy*pL->m_TileMap->width())*3;
//						//qDebug()<<"size"<<pL->m_TileMap->width()<<pL->m_TileMap->height();
//						bits += (uint)(sx+sy*pL->m_TileMap->width())*3;
						tile_s *pTl = g_pTBrushBrowser->m_Tiles[tx][ty];
						pTl->index   = qRed(color);
						pTl->xflip   = qGreen(color) & (1<<X_FLIPPED);
						pTl->yflip   = qGreen(color) & (1<<Y_FLIPPED);
						pTl->rotated = qGreen(color) & (1<<ROTATED);
//						//qDebug()<<"bits"<<*(bits  )<<*(bits+1)<<*(bits+2);
//						pTl->index   = *(bits  );
//						pTl->xflip   = *(bits+1) & (1<<X_FLIPPED);
//						pTl->yflip   = *(bits+1) & (1<<Y_FLIPPED);
//						pTl->rotated = *(bits+1) & (1<<ROTATED);
						g_pTBrushBrowser->ChangeTile(tx,ty);
					}
				}
			}
		}
		if (!shift_down && !control_down && !alt_down && !space_down)
		{
			if ((MGR->KeyState(Qt::LeftButton) & KEY_PRESSED) ||
				(MGR->KeyState(Qt::RightButton) & KEY_PRESSED))
			{
				if (m_CurLayerIdx != -1)
				{
					cTileLayer *pL = m_Layers[m_CurLayerIdx];
					QPainter p;
					p.begin(pL->m_TileMap);
					QVector2D mcrds = MGR->MouseCoords(true)/16.f;
					for(int i=0;i<2;i++)
						mcrds[i] = fmod(mcrds[i],MGR->m_RoomSize[i]/16.f);
					for(int i=0;i<8;i++)
					{
						for(int j=0;j<8;j++)
						{
							tile_s *pT = g_pTBrushBrowser->m_Tiles[i][j];
							if (pT->index != 0)
							{
								int r,g,b;
								r = g = b = 0;
								if (MGR->KeyState(Qt::LeftButton) & KEY_PRESSED)
								{
									r = pT->index;
									g =	(pT->xflip << X_FLIPPED) |
											(pT->yflip << Y_FLIPPED) |
											(pT->rotated << ROTATED) ;
								}
								
								p.setPen(QColor(r,g,b));
								p.drawPoint(mcrds.x()+i,mcrds.y()+j);
							}
						}
					}
					p.end();
					pL->updateTileMapTex();
				}
			}
		}
	}
}
void cTileTool::DrawMe()
{
	MGR->m_GridSize = 1;
	
	if (m_CurLayerIdx != -1)
	{
		gpMainWND->ui->text_LayerScrFacX->setText(QString().number(
					m_Layers[m_CurLayerIdx]->m_ScrollFactor.x()));
		gpMainWND->ui->text_LayerScrFacY->setText(QString().number(
					m_Layers[m_CurLayerIdx]->m_ScrollFactor.y()));
	}
	if (g_pTSetBrowser) g_pTSetBrowser->update();
	if (g_pTBrushBrowser) g_pTBrushBrowser->update();
	
	if (MGR->hasFocus())
	{
		if (m_Grab)
		{
			QVector2D grabSize = MGR->MouseCoords(true)-m_GrabStart;
			for(int i=0;i<2;i++)
			{
				if (grabSize[i] >  128) grabSize[i] =  128;
				if (grabSize[i] < -128) grabSize[i] = -128;
			}
			glColor3f(1,1,1);
			glLineStipple(1,0x3333);
			glEnable(GL_LINE_STIPPLE);
			
			MGR->OutlineRect(m_GrabStart,m_GrabStart+grabSize);
			
			glDisable(GL_LINE_STIPPLE);
		}
		else
		{
			int tex;
			tex = MGR->bindTexture(
						*g_pTBrushBrowser->m_BrushMap,
						GL_TEXTURE_2D,GL_RGBA,QGLContext::InvertedYBindOption);
			QVector2D mcrds = MGR->MouseCoords(true)+MGR->m_View;//*MGR->m_Zoom;
			glColor4f(1.f,1.f,1.f,0.5f);
			MGR->drawTexture(QPoint(mcrds.x(),mcrds.y()),tex);
			MGR->deleteTexture(tex);
		}
		//if (m_CurLayerIdx != -1)
		//{
			//g_pTileShader->bind();
			//TODO draw tile brush 
			//g_pTileShader->release();
		//}
	}
}

void cTileTool::layerSelected(QString layer)
{
	int curIdx = m_CurLayerIdx;
	for(int i=m_Layers.size()-1;i>=0;i--)
	{
		if (m_Layers[i]->m_Depth == layer.toInt())
		{
			m_CurLayerIdx = i;
			if (curIdx != m_CurLayerIdx)
				g_pTBrushBrowser->clearTiles();
			return;
		}
	}
	MGR->setFocus();
}

cTileLayer *cTileTool::AddLayer(QString fname, int depth)
{
	cTileLayer *pLayer = (cTileLayer *)GET_OBJECT(tile_layer);
	pLayer->AssignTileSet(fname);
	SET_OBJECT_DEPTH(pLayer,depth);
	m_Layers.push_back(pLayer);
	qSort(m_Layers.begin(),m_Layers.end(),depthCompare);
	if (m_CurLayerIdx == -1)
		m_CurLayerIdx = 0;// m_Layers.size()-1;
	
	pLayer->resizeLayer(MGR->m_RoomSize);
	UpdateLayerList();
	
	return pLayer;
}
void cTileTool::dupLayer()
{
	if (m_CurLayerIdx == -1)
		return;
	
	bool ok;
	int depth = QInputDialog::getInt(
				gpMainWND,
				"New Layer",
				"New Layer",
				1000, 
				-2147483647,
				2147483647,
				1, &ok);
	if (ok)
	{
		QString fname = QDir::currentPath()+m_Layers[m_CurLayerIdx]->m_FName;//QFileDialog::getOpenFileName(gpMainWND,"Open Tileset",QDir::currentPath(),tr("PNG (*.png)"));
		if (!fname.isEmpty())
		{
			//qDebug()<<"m_CurLayerIdx"<<m_CurLayerIdx;
			cTileLayer *pL, *pOL;
			pOL = m_Layers[m_CurLayerIdx];
			pL = AddLayer(fname,depth);
			pL->resizeLayer(
						QVector2D(
							pOL->m_TileMap->width()*16,
							pOL->m_TileMap->height()*16));
			int size = 
					pOL->m_TileMap->width()*
					pOL->m_TileMap->height()*3;
			memcpy(pL->m_TileMap->bits(),pOL->m_TileMap->bits(),size);
			pL->updateTileMapTex();
		}
	}
	MGR->setFocus();
}
void cTileTool::addLayer()
{
	bool ok;
	int depth = QInputDialog::getInt(
				gpMainWND,
				"New Layer",
				"New Layer",
				1000, 
				-2147483647,
				2147483647,
				1, &ok);
	if (ok)
	{
		QString fname = QFileDialog::getOpenFileName(gpMainWND,"Open Tileset",QDir::currentPath(),
											  tr("PNG (*.png)"));
		if (!fname.isEmpty())
			AddLayer(fname,depth);
	}
	MGR->setFocus();
}
void cTileTool::removeLayer()
{
	if (m_CurLayerIdx != -1)
	{
		cTileLayer *pLayer = m_Layers[m_CurLayerIdx];
		m_Layers.removeAt(m_CurLayerIdx);
		if (m_Layers.size())
			m_CurLayerIdx = 0;
		else
			m_CurLayerIdx = -1;
		REMOVE_OBJECT(pLayer);
		UpdateLayerList();
		
	}
	MGR->setFocus();
}
void cTileTool::changeLayer()
{
	if (m_CurLayerIdx == -1) return;
	
	bool ok;
	int depth = QInputDialog::getInt(
				gpMainWND,
				"New Layer",
				"New Layer",
				m_Layers[m_CurLayerIdx]->m_Depth, 
				-2147483647,
				2147483647,
				1, &ok);
	if (ok)
	{
		SET_OBJECT_DEPTH(m_Layers[m_CurLayerIdx],depth);
		UpdateLayerList();
	}
	MGR->setFocus();
}
void cTileTool::changeTSet()
{
	if (m_CurLayerIdx == -1) return;
	
	QString fname = QFileDialog::getOpenFileName(gpMainWND,"Open Tileset",QDir::currentPath(),
										  tr("PNG (*.png)"));
	if (!fname.isEmpty())
	{
		m_Layers[m_CurLayerIdx]->AssignTileSet(fname);
	}
	MGR->setFocus();
}

void cTileTool::changeScrFac()
{
	if (m_CurLayerIdx == -1) return;
	
	bool ok;
	float scx = QInputDialog::getDouble(
				gpMainWND,
				"Scroll X",
				"Scroll X",
				m_Layers[m_CurLayerIdx]->m_ScrollFactor.x(), 
				-999999.0,
				 999999.0,
				2, &ok);
	if (ok)
	{
		float scy = QInputDialog::getDouble(
					gpMainWND,
					"Scroll Y",
					"Scroll Y",
					m_Layers[m_CurLayerIdx]->m_ScrollFactor.y(), 
					-999999.0,
					 999999.0,
					2, &ok);
		if (ok)
		{
			m_Layers[m_CurLayerIdx]->m_ScrollFactor = QVector2D(scx,scy);
		}
	}
}


cTSetBrowser::cTSetBrowser(QWidget *parent) : QFrame(parent)
{
	g_pTSetBrowser = this;
	m_Ofs.rx() = m_Ofs.ry() = 0;
	setFocusPolicy(Qt::WheelFocus);
	setMouseTracking(true);
	m_Disabled = false;
}
void cTSetBrowser::keyPressEvent(QKeyEvent *e)
{
	MGR->RegisterInput(e->key());
}
void cTSetBrowser::keyReleaseEvent(QKeyEvent *e)
{
	MGR->RegisterInput(e->key(),true);
}
void cTSetBrowser::mousePressEvent(QMouseEvent *me)
{
	MGR->RegisterInput(me->button());
}
void cTSetBrowser::mouseReleaseEvent(QMouseEvent *me)
{
	MGR->RegisterInput(me->button(),true);
}
void cTSetBrowser::mouseMoveEvent(QMouseEvent *me)
{
	
	m_Mouse = me->pos();
}
void cTSetBrowser::paintEvent(QPaintEvent *e)
{
	QPainter p(this);

	if (!g_pTileTool) goto ret;
	if (g_pTileTool->m_CurLayerIdx == -1) goto ret;
	
	p.drawImage(m_Ofs.x()+1,m_Ofs.y()+1,*g_pTileTool->m_Layers[g_pTileTool->m_CurLayerIdx]->m_TSetImg);
	
	int sx,sy,sw,sh;
	sx = m_Select.x();sx >>= 4;sx <<= 4;//sx++;
	sy = m_Select.y();sy >>= 4;sy <<= 4;//sy++;
	sw = 17;//m_Select.width(); sw >>= 4;sw++;sw <<= 4;sw++;
	sh = 17;//m_Select.height();sh >>= 4;sh++;sh <<= 4;sh++;
	p.setBrush(QBrush(QColor(255,255,255,64)));
	p.drawRect(sx+m_Ofs.x(),sy+m_Ofs.y(),sw,sh);
	
ret:
	if (!hasFocus())
	{
		p.setBrush(QBrush(QColor(0,0,0,64)));
		p.drawRect(0,0,128,240);
		
		p.setFont(*MGR->m_pFonts[0]);
		
		p.setPen(QColor(255,255,255));
		p.drawText(4,16,"Disabled");
	}
	
	QFrame::paintEvent(e);
	
	return;
	
}
void cTSetBrowser::ProcessInput()
{
	if (!hasFocus())
	{
		m_Disabled = true;
		return;
	}
	if (m_Disabled)
	{
		if ((MGR->KeyState(Qt::LeftButton) & KEY_RELEASED) ||
			 (MGR->KeyState(Qt::RightButton) & KEY_RELEASED) ||
			 (MGR->KeyState(Qt::MiddleButton) & KEY_RELEASED))
			m_Disabled = false;
		return;
	}
	bool shift_down   = !!(MGR->KeyState(Qt::Key_Shift  ) & KEY_ISDOWN);
	bool control_down = !!(MGR->KeyState(Qt::Key_Control) & KEY_ISDOWN);
	bool alt_down     = !!(MGR->KeyState(Qt::Key_Alt    ) & KEY_ISDOWN);
	bool space_down   = !!(MGR->KeyState(Qt::Key_Space  ) & KEY_ISDOWN);

	if (!shift_down && !control_down && !alt_down && !space_down)
	{
		if (g_pTileTool->m_CurLayerIdx != -1)
		{
//			if (MGR->KeyState(Qt::LeftButton) & KEY_PRESSED)
//			{
//				m_Select.setX(m_Mouse.x()-m_Ofs.x());
//				m_Select.setY(m_Mouse.y()-m_Ofs.y());
//				m_Select.setWidth(0);
//				m_Select.setHeight(0);
//			}
			if (MGR->KeyState(Qt::LeftButton) & KEY_ISDOWN)
			{
//				int w,h;
//				w = m_Mouse.x()-m_Ofs.x() - m_Select.x();
//				h = m_Mouse.y()-m_Ofs.y() - m_Select.y();
//				if (w > 112) w = 112;
//				if (h > 112) h = 112;
//				if (w < 0)
//				{
//					m_Select.setX(    m_Mouse.x()-m_Ofs.x());
//					m_Select.setWidth(0);
//				}
//				else
//					m_Select.setWidth(w);
	
//				if (h < 0)
//				{
//					m_Select.setY(    m_Mouse.y()-m_Ofs.y());
//					m_Select.setWidth(0);
//				}
//				else
//					m_Select.setHeight(h);
				
				m_Select.setX(m_Mouse.x()-m_Ofs.x());
				m_Select.setY(m_Mouse.y()-m_Ofs.y());
				m_Select.setWidth(0);
				m_Select.setHeight(0);
				
				if (m_Select.x() > g_pTileTool->m_Layers[g_pTileTool->m_CurLayerIdx]->m_Width-16)
					m_Select.setX(g_pTileTool->m_Layers[g_pTileTool->m_CurLayerIdx]->m_Width-16);
				if (m_Select.y() > g_pTileTool->m_Layers[g_pTileTool->m_CurLayerIdx]->m_Height-16)
					m_Select.setY(g_pTileTool->m_Layers[g_pTileTool->m_CurLayerIdx]->m_Height-16);
			}
		}
	}

	QCursor cur;
	cur.setShape(Qt::ArrowCursor);
	if (space_down)
	{
		if (!shift_down && !control_down && !alt_down)
		{
			cur.setShape(Qt::OpenHandCursor);
			if (MGR->KeyState(Qt::LeftButton) & KEY_ISDOWN)
			{
				cur.setShape(Qt::ClosedHandCursor);
				if (g_pTileTool->m_CurLayerIdx != -1)
				{
					//for(int i=0;i<2;i++)
					//{
						m_Ofs.rx() += m_Mouse.x() - m_MousePrev.x();
						m_Ofs.ry() += m_Mouse.y() - m_MousePrev.y();
						
						if (m_Ofs.x() > 0) m_Ofs.rx() = 0;
						if (m_Ofs.y() > 0) m_Ofs.ry() = 0;
					//}
					int minw,minh;
					minw = min(-(g_pTileTool->m_Layers[g_pTileTool->m_CurLayerIdx]->m_Width -128),0);
					minh = min(-(g_pTileTool->m_Layers[g_pTileTool->m_CurLayerIdx]->m_Height-240),0);
					if (m_Ofs.x() < minw) m_Ofs.rx() = minw;
					if (m_Ofs.y() < minh) m_Ofs.ry() = minh;
				}
			}
		}
	}
	setCursor(cur);

	m_MousePrev = m_Mouse;
}

cTBrushBrowser::cTBrushBrowser(QWidget *parent) : QFrame(parent)
{
	g_pTBrushBrowser = this;
	setFocusPolicy(Qt::WheelFocus);
	setMouseTracking(true);
	m_Disabled = false;
	
	m_GridImg = new QImage("res/editor/brush_grid.png");
	m_BrushMap = new QPixmap(128,128);
	m_BrushMap->fill(QColor(0,0,0,0));
	//memset(bits,0,128*128*4);
	
	connect(&gStarter,SIGNAL(timeout()),this,SLOT(AssignMe()));
	
	for(int i=0;i<8;i++)
		for(int j=0;j<8;j++)
			m_Tiles[i][j] = new tile_s;
}
void cTBrushBrowser::AssignMe()
{
	connect(gpMainWND->ui->but_BrushClear,SIGNAL(clicked()),this,SLOT(clearTiles()));
	connect(gpMainWND->ui->but_TileMirHor,SIGNAL(clicked()),this,SLOT(mirrorTilesHor()));
	connect(gpMainWND->ui->but_TileMirVer,SIGNAL(clicked()),this,SLOT(mirrorTilesVer()));
}

void cTBrushBrowser::mirrorTilesHor()
{
	tile_s *tTiles[8][8];
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<8;j++)
		{
			tTiles[7-i][j] = new tile_s(m_Tiles[i][j]);
			tTiles[7-i][j]->xflip = !tTiles[7-i][j]->xflip;
		}
	}
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<8;j++)
		{
			delete m_Tiles[i][j];
			m_Tiles[i][j] = tTiles[i][j];
			ChangeTile(i,j);
		}
	}
}
void cTBrushBrowser::mirrorTilesVer()
{
	tile_s *tTiles[8][8];
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<8;j++)
		{
			tTiles[i][7-j] = new tile_s(m_Tiles[i][j]);
			tTiles[i][7-j]->yflip = !tTiles[i][7-j]->yflip;
		}
	}
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<8;j++)
		{
			delete m_Tiles[i][j];
			m_Tiles[i][j] = tTiles[i][j];
			ChangeTile(i,j);
		}
	}
}

void cTBrushBrowser::clearTiles()
{
	for(int i=0;i<8;i++)
		for(int j=0;j<8;j++)
		{
			m_Tiles[i][j]->rotated = 
					m_Tiles[i][j]->xflip = 
					m_Tiles[i][j]->yflip = false;
			m_Tiles[i][j]->index = 0;
			ChangeTile(i,j);
		}
}

void cTBrushBrowser::keyPressEvent(QKeyEvent *e)
{
	MGR->RegisterInput(e->key());
}

void cTBrushBrowser::keyReleaseEvent(QKeyEvent *e)
{
	MGR->RegisterInput(e->key(),true);
}

void cTBrushBrowser::mousePressEvent(QMouseEvent *me)
{
	MGR->RegisterInput(me->button());
}

void cTBrushBrowser::mouseReleaseEvent(QMouseEvent *me)
{
	MGR->RegisterInput(me->button(),true);
}

void cTBrushBrowser::mouseMoveEvent(QMouseEvent *me)
{
	m_MousePrev = m_Mouse;
	m_Mouse = me->pos();
}

void cTBrushBrowser::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	
//	cTileLayer *pL;
//	QPoint crds;
	
	p.drawImage(0,0,*m_GridImg);
	p.drawPixmap(0,0,*m_BrushMap);
	
	/*if (g_pTileTool->m_CurLayerIdx  == -1) goto ret;
	
	pL = g_pTileTool->m_Layers[g_pTileTool->m_CurLayerIdx];
	for(int i=0;i<8;i++)
	{
		for(int j=0;j<8;j++)
		{
			crds = pL->getCrdsByIndex(m_Tiles[i][j]->index);
			int scx,scy,r;
			scx = scy = 1;
			r = 0;

			if (m_Tiles[i][j]->xflip  ) scx = -1;
			if (m_Tiles[i][j]->yflip  ) scy = -1;
			if (m_Tiles[i][j]->rotated) r   = 90;

			p.resetTransform();
			p.translate(i*16+8,j*16+8);
			p.scale(scx,scy);
			p.rotate(r);
			p.drawImage(
					QRect(-8,-8,16,16),
					*pL->m_TSetImg,
					QRect(crds.x(),crds.y(),16,16));
		}
	}*/
	
//ret:
	if (!hasFocus())
	{
		p.resetTransform();
		
		p.setBrush(QBrush(QColor(0,0,0,64)));
		p.drawRect(0,0,128,128);
		
		p.setFont(*MGR->m_pFonts[0]);
		
		p.setPen(QColor(255,255,255));
		p.drawText(4,16,"Disabled");
	}
	QFrame::paintEvent(e);
}
void cTBrushBrowser::ChangeTile(int tx, int ty)
{
	if (g_pTileTool->m_CurLayerIdx  == -1) return;
	
	
	cTileLayer *pL = g_pTileTool->m_Layers[g_pTileTool->m_CurLayerIdx];
	
	QPainter p(m_BrushMap);
	
//	int dx,dy;
//	dx = tx << 4;
//	dy = ty << 4;
	
	tile_s *pTl = m_Tiles[tx][ty];
	
	QPoint crds = pL->getCrdsByIndex(pTl->index);
	int scx,scy,r;
	scx = scy = 1;
	r = 0;
	
	if (pTl->xflip  ) scx = -1;
	if (pTl->yflip  ) scy = -1;
	if (pTl->rotated) r   = 90;

	p.resetTransform();
	p.translate(tx*16+8,ty*16+8);
	p.scale(scx,scy);
	p.rotate(r);
	p.setCompositionMode(QPainter::CompositionMode_Source);
	//p.fillRect(QRect(-8,-8,16,16),QColor(0,0,0,0));
	p.drawImage(
			QRect(-8,-8,16,16),
			*pL->m_TSetImg,
			QRect(crds.x(),crds.y(),16,16));
	
}
void cTBrushBrowser::ProcessInput()
{
	if (!hasFocus())
	{
		m_Disabled = true;
		return;
	}
	if (m_Disabled)
	{
		if ((MGR->KeyState(Qt::LeftButton) & KEY_RELEASED) ||
			 (MGR->KeyState(Qt::RightButton) & KEY_RELEASED) ||
			 (MGR->KeyState(Qt::MiddleButton) & KEY_RELEASED))
			m_Disabled = false;
		return;
	}
	
	bool shift_down   = !!(MGR->KeyState(Qt::Key_Shift  ) & KEY_ISDOWN);
	bool control_down = !!(MGR->KeyState(Qt::Key_Control) & KEY_ISDOWN);
	bool alt_down     = !!(MGR->KeyState(Qt::Key_Alt    ) & KEY_ISDOWN);
	bool space_down   = !!(MGR->KeyState(Qt::Key_Space  ) & KEY_ISDOWN);
	
	if (g_pTileTool->m_CurLayerIdx  != -1)
	{
		cTileLayer *pL = g_pTileTool->m_Layers[g_pTileTool->m_CurLayerIdx];
		
		int tx,ty;
		tx = m_Mouse.x()>>4;
		ty = m_Mouse.y()>>4;
		//qDebug()<<"tx, ty"<<tx<<ty;
		
		if (tx >= 0 && tx < 8 && ty >= 0 && ty < 8)
		{
			if (!space_down && !shift_down && !control_down && !alt_down)
			{
				if (MGR->KeyState(Qt::LeftButton) & KEY_ISDOWN)
				{
					int sx,sy;
					sx = g_pTSetBrowser->m_Select.x() >> 4;
					sy = g_pTSetBrowser->m_Select.y() >> 4;
					
					m_Tiles[tx][ty]->index = pL->getIndexByCrds(QPoint(sx,sy));
					ChangeTile(tx,ty);
				}
				if (MGR->KeyState(Qt::RightButton) & KEY_ISDOWN)
				{
					m_Tiles[tx][ty]->index = 0;
					m_Tiles[tx][ty]->rotated = 
							m_Tiles[tx][ty]->xflip = 
							m_Tiles[tx][ty]->yflip = false;
					ChangeTile(tx,ty);
				}
			}
			if (!space_down && shift_down && !control_down && !alt_down)
			{
				if (MGR->KeyState(Qt::LeftButton) & KEY_PRESSED)
				{
					m_Tiles[tx][ty]->xflip = !m_Tiles[tx][ty]->xflip;
					ChangeTile(tx,ty);
				}
			}
			if (!space_down && !shift_down && !control_down && alt_down)
			{
				if (MGR->KeyState(Qt::LeftButton) & KEY_PRESSED)
				{
					m_Tiles[tx][ty]->yflip = !m_Tiles[tx][ty]->yflip;
					ChangeTile(tx,ty);
				}
			}
			if (!space_down && !shift_down && control_down && !alt_down)
			{
				if (MGR->KeyState(Qt::LeftButton) & KEY_PRESSED)
				{
					m_Tiles[tx][ty]->rotated = !m_Tiles[tx][ty]->rotated;
					ChangeTile(tx,ty);
				}
			}
		}
	}

	QCursor cur;
	cur.setShape(Qt::ArrowCursor);
	if (space_down)
	{
		if (!shift_down && !control_down && !alt_down)
		{
			cur.setShape(Qt::OpenHandCursor);
			if (MGR->KeyState(Qt::LeftButton) & KEY_ISDOWN)
			{
				cur.setShape(Qt::ClosedHandCursor);
			}
		}
	}
	setCursor(cur);
}
