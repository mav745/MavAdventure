//#include "cTileLayer.h"
//#include "cTileManager.h"
//#include "cMainManager.h"
//#include <QGLFunctions>
//#include <cGameRoom.h>
//#include <util.h>
//#include <QDebug>
//#include <QOpenGLShaderProgram>
//#include <QMap>
//#include <math.h>

//#include <iostream>

//QMap<QString, QImage *>g_TMngrImages;

//#define TILE_W 16
//#define TILE_H TILE_W
////#define TILE_UV_W (TILE_W/640)
////#define TILE_VV_H (TILE_H/480)

////QMap<QString, int>g_TilesetTexs;

//QOpenGLShaderProgram *g_pTileShader;

//cTileManager::cTileManager(QObject *parent) :
//	QObject(parent)
//{
//	DBG_PRINT("Tile manager");
	
//	if (!g_pTileShader)
//	{
//		g_pTileShader = cMainManager::CreateShader("shaders/tileset.v","shaders/tileset.p");
//		g_pTileShader->bind();
//		g_pTileShader->setUniformValue(g_pTileShader->uniformLocation("s_TileMap"),0);
//		g_pTileShader->setUniformValue(g_pTileShader->uniformLocation("s_TileSet"),1);
//		g_pTileShader->setUniformValue("u_Zoom",1);
//		g_pTileShader->release();
//	}
//	for(int i=-1;i<41;i++)
//	{
//		for(int j=-1;j<31;j++)
//		{
//			m_Verts.push_back(i*16   ); m_Verts.push_back(j*16   );
//			m_Verts.push_back(i*16+16); m_Verts.push_back(j*16   );
//			m_Verts.push_back(i*16+16); m_Verts.push_back(j*16+16);
//			m_Verts.push_back(i*16   ); m_Verts.push_back(j*16+16);
			
//			m_UVs.push_back(0); m_UVs.push_back(0);
//			m_UVs.push_back(1); m_UVs.push_back(0);
//			m_UVs.push_back(1); m_UVs.push_back(1);
//			m_UVs.push_back(0); m_UVs.push_back(1);
//		}
//	}
//	//DBG_PRINT(("m_Verts.size()"+QString().number(m_Verts.size())).toUtf8());
//	//DBG_PRINT(("m_UVs.size()"+QString().number(m_UVs.size())).toUtf8());
//}
//cTileManager::~cTileManager()
//{
//	for(int i=m_Layers.size()-1;i>=0;i--)
//		delete m_Layers[i];
	
//	m_Layers.clear();
//}
//void cTileManager::SetLayerScrFac(int depth, const QVector2D &scrfc)
//{
//	for(int i=m_Layers.size()-1;i>=0;i--)
//		if (m_Layers[i]->m_Depth == depth)
//		{
//			m_Layers[i]->scroll_factor = scrfc;
//			return;
//		}
//}
//QImage *cTileManager::GetImage(QString fname)
//{
//	if (g_TMngrImages.keys().contains(fname)) return g_TMngrImages[fname];
//	return (g_TMngrImages[fname] = new QImage(fname));
//}
//void cTileManager::AddLayer(
//		int depth,
//		QString tileset_filename,
//		QString tilemap_filename)
//{
//	for(int i=m_Layers.size()-1;i>=0;i--)
//		if (m_Layers[i]->m_Depth == depth) return;
	
//	QImage *ptSet = GetImage(tileset_filename);
//	QImage *ptMap = GetImage(tilemap_filename);
	
//	int tex = MGR->context()->bindTexture(
//				*ptSet,GL_TEXTURE_2D,GL_RGBA,QGLContext::InvertedYBindOption);
//	int map = MGR->context()->bindTexture(
//				*ptMap,
//				GL_TEXTURE_2D,GL_RGBA,QGLContext::InvertedYBindOption);
//	tilelayer_t *p_tlayer = new tilelayer_t(depth,tex,map);
//	p_tlayer->size = QVector2D(ptSet->size().width()/16,ptSet->size().height()/16);
//	p_tlayer->p_tileimg = ptSet;
//	p_tlayer->p_mapimg = ptMap;
//	p_tlayer->p_verts = &m_Verts;
//	p_tlayer->p_uvs = &m_UVs;
//	m_Layers.push_back(p_tlayer);
//	MGR->m_pCurrentRoom->RememberRenderable(p_tlayer);
//	//qSort(m_Layers.begin(),m_Layers.end());
//}
//void cTileManager::DeleteLayer(int depth)
//{
//	for (int i=m_Layers.size()-1;i>=0;i--)
//		if (m_Layers[i]->m_Depth == depth)
//		{
//			MGR->m_pCurrentRoom->RemoveRenderable(m_Layers[i]);
//			m_Layers.removeAt(i);
//			return;
//		}
//}
//void cTileManager::PositionLayer(int depth, const QVector2D &o)
//{
//	for (int i=m_Layers.size()-1;i>=0;i--)
//		if (m_Layers[i]->m_Depth == depth) {m_Layers[i]->orig = o;return;}
//}
//QVector2D cTileManager::LayerPosition(int depth)
//{
//	for (int i=m_Layers.size()-1;i>=0;i--)
//		if (m_Layers[i]->m_Depth == depth) return m_Layers[i]->orig;
//	return QVector2D(0,0);
//}
//void cTileManager::HideLayer(int depth)
//{
//	for (int i=m_Layers.size()-1;i>=0;i--)
//		if (m_Layers[i]->m_Depth == depth) {m_Layers[i]->visible = false;return;}
//}
//void cTileManager::ShowLayer(int depth)
//{
//	for (int i=m_Layers.size()-1;i>=0;i--)
//		if (m_Layers[i]->m_Depth == depth) {m_Layers[i]->visible = true;return;}
//}
//bool cTileManager::IsLayerVisible(int depth)
//{
//	for (int i=m_Layers.size()-1;i>=0;i--)
//		if (m_Layers[i]->m_Depth == depth) return m_Layers[i]->visible;
	
//	return false;
//}
//bool cTileManager::LayerExists(int depth)
//{
//	for (int i=m_Layers.size()-1;i>=0;i--)
//	{
//		if (m_Layers[i]->m_Depth == depth) return true;
//	}
	
//	return false;
//}
//void cTileManager::LayerSetDepth(int depth, int new_depth)
//{
//	for (int i=m_Layers.size()-1;i>=0;i--)
//	{
//		if (m_Layers[i]->m_Depth == depth)
//		{
//			m_Layers[i]->m_Depth = new_depth;
//			qSort(m_Layers.begin(),m_Layers.end());
//			return;
//		}
//	}
//}
//void cTileManager::ScrollAllLayers(QVector2D sc)
//{
//	//std::cout<<"ScrollAllLayers";
//	int sz = m_Layers.size();
//	for(int i=0;i<sz;i++)
//	{
//		for(int j=0;j<2;j++)
//		{
//			m_Layers[i]->orig[j] -= float(sc[j])*m_Layers[i]->scroll_factor[j];
//			if (m_Layers[i]->orig[j] > MGR->m_pCurrentRoom->m_Size[j]) 
//				m_Layers[i]->orig[j] -= MGR->m_pCurrentRoom->m_Size[j];
//			if (m_Layers[i]->orig[j] < 0.f) 
//				m_Layers[i]->orig[j]+= MGR->m_pCurrentRoom->m_Size[j];
//		}
////		m_Layers[i]->orig.ry() += float(sc.y())*m_Layers[i]->scroll_factor.y();
////		if (m_Layers[i]->orig.ry() > MGR->m_pCurrentRoom->m_Size.y()) 
////			m_Layers[i]->orig.ry() -= MGR->m_pCurrentRoom->m_Size.y();
////		if (m_Layers[i]->orig.ry() < 0.f) 
////			m_Layers[i]->orig.ry() += MGR->m_pCurrentRoom->m_Size.y();
//	}
//}
//void cTileManager::DrawAllLayers()
//{
//}
//tilelayer_t * cTileManager::GetLayer(int depth)
//{
//	for(int i=m_Layers.size()-1;i>=0;i--)
//	{
//		if (m_Layers[i]->m_Depth != depth) continue;
//		return m_Layers[i];
//	}
//	return NULL;
//}
//int cTileManager::GetTileHeight(QVector2D _pos, int depth)
//{
//	tilelayer_t *layer = GetLayer(depth);
//	if (!layer) return false;
	
//	QVector2D fpos = QVector2D(_pos.x(),_pos.y());
//	for(int j=0;j<2;j++)
//	{
//		fpos[j] = layer->orig[j]*layer->scroll_factor[j]+fpos[j];
//		fpos[j] = UTIL_FModulo(fpos[j],16.f);
//		while (fpos[j] < 0.f) fpos[j] += 16.f;
//	}
	
//	tileinfo_s info = GetTileInfo(_pos,layer);
	
//	int hpos,vpos,w;
//	w = layer->p_tileimg->width()/16;
////	if (!(info.flags & (1<<ROTATED)))
////	{
////		if (info.flags & (1<<X_FLIPPED))
////			hpos = 16*(info.index % w)+15-fpos.x();
////		else
////			hpos = 16*(info.index % w)+fpos.x();
////	}
//	//qDebug()<<"X_FLIPPED"<<!!(info.flags & (1<<X_FLIPPED));
	
	
//	const uchar *tsetBits = layer->p_tileimg->constBits();
//	for(int i=0;i<16;i++)
//	{
//		const uchar *a;
//		if (!(info.flags & (1<<ROTATED)))
//		{
//			if (info.flags & (1<<X_FLIPPED))
//				hpos = 16*(info.index % w)+15-fpos.x();
//			else
//				hpos = 16*(info.index % w)+fpos.x();
			
//			vpos = 16*(info.index/w)+i;
//			if (info.flags & (1<<Y_FLIPPED)) vpos = 15-vpos;
//			a = tsetBits + (hpos + vpos*layer->p_tileimg->width())*4;
//		}
//		else
//		{
//			int rhpos, rvpos;
			
//			if (!(info.flags & (1<<X_FLIPPED))) rhpos = 15-fpos.x();
//			else rhpos = fpos.x();
//			rvpos = 16*(info.index/w)+i;
//			if (!(info.flags & (1<<Y_FLIPPED))) rvpos = 15-rvpos;
			
//			hpos = 16*(info.index % w)+rvpos;//+fpos.x();
//			vpos = rhpos;//16*(info.index/w)+i;
			
//			a = tsetBits + (hpos + vpos*layer->p_tileimg->width())*4;
//		}
//		a += 3;
//		if (*(a) != 0) return floor(_pos.y()/16)*16+i;
//	}
//	return -1;
//}
//int cTileManager::TraceVer(QVector2D _pos, int depth, float tracedist, bool up)
//{
//	float y = _pos.y();
//	int h;
//	while(1)
//	{
//		h = GetTileHeight(QVector2D(_pos.x(),y),depth);
		
//		//qDebug()<<"y"<<y<<"h"<<h<<_pos;
//		if (h != -1) return h;
		
//		if (y - _pos.y() >= tracedist) return -1;
//		y += 16;
//	}
//	return -1;
//}
//tileinfo_s cTileManager::GetTileInfo(QVector2D _pos, tilelayer_t * layer)
//{
//	QVector2D fpos = QVector2D(_pos.x(),_pos.y());
	
//	float fsz[] = 
//	{
//		layer->p_mapimg->size().width( )*16.f,
//		layer->p_mapimg->size().height()*16.f
//	};
//	for(int j=0;j<2;j++)
//	{
//		fpos[j] = layer->orig[j]*layer->scroll_factor[j]+fpos[j];
//		fpos[j] = UTIL_FModulo(fpos[j],fsz[j])/16.f;
//	}
	
//	int pos;// = 2;
//	pos  = (int)fpos.x();
//	pos += layer->p_mapimg->size().width()*((int)fpos.y());
	
//	const uchar *mapBits = layer->p_mapimg->constBits()+pos*4;

//	tileinfo_s info;
//	info.index = *(mapBits+2);
//	info.flags = *(mapBits+1);
	
//	return info;
//}
//bool  cTileManager::IsTileExists(QVector2D _pos, int depth)
//{
//	tilelayer_t *layer = GetLayer(depth);
//	if (!layer) return false;
	
//	QVector2D fpos = QVector2D(_pos.x(),_pos.y());
//	for(int j=0;j<2;j++)
//	{
//		fpos[j] = layer->orig[j]*layer->scroll_factor[j]+fpos[j];
//		fpos[j] = UTIL_FModulo(fpos[j],16.f);
//		while (fpos[j] < 0.f) fpos[j] += 16.f;
//	}
	
//	tileinfo_s info = GetTileInfo(_pos,layer);
//	//qDebug()<<"info"<<info.index;
	
//	int hpos,vpos,w;
//	w = layer->p_tileimg->width()/16;
//	hpos = 16*(info.index % w)+fpos.x();
//	vpos = 16*(info.index/w+fpos.y()/16);
//	//qDebug()<<"hpos"<<hpos<<"vpos"<<vpos<<"w"<<w;
	
//	const uchar *tsetBits = layer->p_tileimg->constBits();
//	tsetBits += (hpos + vpos*layer->p_tileimg->width())*4;
//	//qDebug()<<"colors"<<*(tsetBits)<<*(tsetBits+1)<<*(tsetBits+2)<<*(tsetBits+3);
	
//	return (*(tsetBits+3) != 0);

//}
