#include "cSegment.h"
#include "cLine.h"
#include "cGameRoom.h"
#include "cBaseObject.h"
#include "cTileManager.h"
#include <QDebug>
#include <cMainManager.h>
#include <util.h>
#include <math.h>
#include "cSegment.h"

#include <objectTypes.h>
#include <fileBuffer.h>
#include <QFile>
#include <QMessageBox>
#include <QDir>

#define REVISION_2DL 2
#define SCROLL_SPEED 32.f//64.f
#define SUBDIVIDE 256.f

int gRoomNum;
extern int gObjNum;
//extern bool g_DebugWalls;

qint64 qPointToLongLong(const QPoint &p)
{
	qint64 v = p.x();
	v <<= 32;
	v += p.y();
	return v;
}
bool compareByDepth(cBaseRenderable* showA, cBaseRenderable* showB)
{
	return showA->m_Depth > showB->m_Depth;
}
cGameRoom::cGameRoom(QObject *parent, const QVector2D &size) :
	QObject(parent)
{
	//DBG_PRINT("Adding room...");
	m_Block[0] = m_Block[1] = m_Block[2] = m_Block[3] = false;
	//m_View = QPoint(0,0);
	m_Size = size;
	//m_Scroll[0] = m_Scroll[1] = hpint_s(0,0.f);
	//m_ScrollTail = QPointF(0,0);
	//m_AutoScroll[X] = -0.5f;
	//m_AutoScroll[1] = hpint_s(0,0.f);
//	m_pTileMngr = new cTileManager;
	m_Name.clear();// = "room"+QString::number(gRoomNum);
	m_Flags = 0;
	
	//memset(m_Backs,0,sizeof(cBackground)*7);
	//for(int i=0;i<7;i++) m_Backs[i]=0;
	gRoomNum++;
}

void cGameRoom::TracePlayer(const QVector2D &start, const QVector2D &end, traceresult_s *ptr)
{
	ptr->tracedir = (end-start).normalized();
	ptr->endpos = end;
	ptr->fraction = 1.f;
	ptr->normal = QVector2D(0,0);
	
	cSegment ray(start,end);
	//qDebug()<<"tracing";
	for(int i=m_Walls.size()-1;i>=0;i--)
	{
		//qDebug()<<i;
		m_Walls[i]->TraceAgainstMe(ray,ptr);
	}
}

void cGameRoom::RemoveRenderable(cBaseRenderable *obj)
{
	for(int i=m_pToRender.size()-1;i>=0;i--)
	{
		if (m_pToRender[i] == obj)
		{
			m_pToRender.removeAt(i);
			return;
		}
	}
}
void cGameRoom::ChangeObjectDepth(cBaseObject *pObj, int new_depth)
{
	pObj->m_Depth = new_depth;
	qSort(m_pToRender.begin(),m_pToRender.end(),compareByDepth);
}

cBaseRenderable *cGameRoom::RememberRenderable(cBaseRenderable *obj)
{
	obj->m_pMyRoom = this;
	m_pToRender.push_back(obj);
	//cBaseRenderable::SortUs(m_pToRender.data(),m_pToRender.size());
	qSort(m_pToRender.begin(),m_pToRender.end(),compareByDepth);
	return obj;
}

cGameRoom::~cGameRoom()
{
	//if (m_pTileMngr) delete m_pTileMngr;
	
	for(int i=m_Backs.size()-1;i>=0;i--)
		if (m_Backs[i]) delete m_Backs[i];
	
	for(int i=m_pObjects.size()-1;i>=0;i--)
		DeleteObject(m_pObjects[i]);
	
	for(int i=m_Lines.size()-1;i>=0;i--)
		delete m_Lines[i];
	
	m_pObjects.clear();
	m_pToRender.clear();
	m_Backs.clear();
}

cTileLayer *cGameRoom::AddLayer(QString fname, int depth)
{
	cTileLayer *pLayer = (cTileLayer *)CreateObject("tile_layer");
	pLayer->AssignTileSet(fname);
	ChangeObjectDepth(pLayer,depth);
	m_Layers.push_back(pLayer);
	//qSort(m_Layers.begin(),m_Layers.end(),depthCompare);
	//if (m_CurLayerIdx == -1)
	//	m_CurLayerIdx = 0;// m_Layers.size()-1;
			
	//UpdateLayerList();
	
	return pLayer;
}

cGameRoom * cGameRoom::LoadFromFile(const QString &filename)
{
	cGameRoom * pNewRoom = NULL;
	
	
	QFile pf(filename);
	if (pf.open(QFile::ReadOnly))
	{
		//MGR->EnableInput();
		
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
			return NULL;
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
			return NULL;
		}

		pNewRoom = MGR->AddRoom();
		pNewRoom->SetBlock( EBS_UP,true);
		pNewRoom->SetBlock( EBS_DOWN,true);
		pNewRoom->m_Name = filename;
		pNewRoom->m_Name.remove("res/maps/");
		pNewRoom->m_Name.remove(".2dl");
		
		for(int i=pNewRoom->m_Backs.size()-1;i>=0;i--)
			if (pNewRoom->m_Backs[i]) delete pNewRoom->m_Backs[i];
		for(int i=pNewRoom->m_pObjects.size()-1;i>=0;i--)
			pNewRoom->DeleteObject(pNewRoom->m_pObjects[i]);
		
		pNewRoom->m_pObjects.clear();
		pNewRoom->m_pToRender.clear();
		pNewRoom->m_Backs.clear();
		
		//DBG_PRINT_S("loading... 1");
		
		DS>>sz;//fread(&sz,4,1,pf); //num backs
		//DBG_PRINT_S(QString().number(sz));
		
		for(int i=0;i<sz;i++)
		{
			QVector2D Origin, ScrFac, ScrSpd;
			bool HRep, VRep;
			DS>>qstr;//freadstr(&qstr,pf);
			DS>>iv;  //depth
			DS>>Origin>>ScrFac>>ScrSpd>>HRep>>VRep;
			//DBG_PRINT_S("loading... 1."+QString().number(i));
			//DBG_PRINT_S(QDir::currentPath()+qstr);
			pNewRoom->AddBack(QDir::currentPath()+qstr,Origin,ScrSpd,ScrFac,HRep,VRep,iv);
		}
		
		//DBG_PRINT_S("loading... 2");
		
		DS>>sz;	//fread(&sz,4,1,pf); //num layers
		//qDebug()<<"num layers"<<sz;
		for(int i=0;i<sz;i++)
		{
			DS>>qstr;//freadstr(&qstr,pf);
			DS>>iv;//fread(&iv,4,1,pf); //depth
			
			cTileLayer *pL = pNewRoom->AddLayer(QDir::currentPath()+qstr,iv);
			
			DS>>pL->m_ScrollFac;	//fread(&pL->m_ScrollFactor[0],4,1,pf);
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
		
		//DBG_PRINT_S("loading... 3");
		
		DS>>sz;	//fread(&sz,4,1,pf); //num lines
		//qDebug()<<"num lines"<<sz;
		for(int i=0;i<sz;i++)
		{
			bool jumpDown;
			QPoint s,e;
			DS
					>>s
					>>e
					>>jumpDown;
			
			QVector2D vs = QVector2D(s.x(),s.y()),
						 ve = QVector2D(e.x(),e.y());
			QVector2D t = ve-vs;
			t.normalize();
			float fmax = (vs - ve).length();//fstep = 256.f/;
			float f = 0;
			float step = std::min(SUBDIVIDE,fmax-f);
			while(true)
			{
				cLine *pL = new cLine;
				pL->m_S = vs+t*f;//QVector2D(s.x(),s.y());
				pL->m_E = vs+t*(f+step);//QVector2D(e.x(),e.y());
				pL->m_JumpDown = jumpDown;
				pL->CalcMe();
				pNewRoom->m_Lines.push_back(pL);
				
				f += step;
				
				step = std::min(SUBDIVIDE,fmax-f);
				if (step == 0.f) break;
			}
			
			//qDebug()<<"line"<<pL->m_S<<pL->m_E;
			/*
			QString hex;
			quint8 *ptr = reinterpret_cast<quint8*>(&pL->m_S);
			int count = sizeof(pL->m_S);
			for(int i=0; i<count; i++)
				hex = hex + QString::number(ptr[i], 16) + " ";
			//qDebug() << hex;
			hex += ", ";
			ptr = reinterpret_cast<quint8*>(&pL->m_E);
			count = sizeof(pL->m_E);
			for(int i=0; i<count; i++)
				hex = hex + QString::number(ptr[i], 16) + " ";
			qDebug() << hex;*/
			
			
//			cSegment *pL = (cSegment *)GET_OBJECT(segment);
//			DS>>pL->m_S>>pL->m_E>>pL->m_JumpDown;
//			pL->CalcMe();
//			g_pLineTool->m_Lines.push_back(pL);
			
		}
		//DBG_PRINT_S("loading... 4");
		pNewRoom->ExpandWalls();
		//DBG_PRINT_S("loading... 5");
		//qDebug()<<"num walls"<<m_Walls.size();
		int sz2;
		DS>>sz;	//fread(&sz,4,1,pf); //numtriggers
		//DBG_PRINT_S(QString("numtriggers: ")+QString::number(sz));
		for(int i=0;i<sz;i++)
		{
			//cBaseTrigger *pT = (cBaseTrigger *)GET_OBJECT(trigger);
			DS>>sz2;	//fread(&sz2,4,1,pf); //numparts
			//qDebug()<<"numparts"<<sz2;
			QVector<float>verts;
			for(int j=0;j<sz2;j++)
			{
				//float sx,sy,ex,ey;
				QVector2D s,e;
				DS>>s;
				//DS>>sy;
				DS>>e;
				//DS>>ey;
				verts.push_back(s.x());
				verts.push_back(s.y());
				verts.push_back(e.x());
				verts.push_back(e.y());
				//pT->AddRectShape(QVector2D(sx,sy),QVector2D(ex,ey));
			}
			//g_pTriggsTool->m_Triggs.push_back(pT);
			
			DS>>qstr;	//freadstr(&qstr,pf);
			cBaseObject *pEnt = pNewRoom->CreateObject(qstr);
			if (!pEnt)
			{
				DBG_PRINT_S(QString("Unknown entity: ")+qstr);
			}
			else
			{
				cBaseTrigger *pTrig = static_cast<cBaseTrigger *>(pEnt);
				for(int j=0;j<sz2;j++)
				{
					pTrig->AddPart(
							QVector2D(verts[j*4  ],verts[j*4+1]),
							QVector2D(verts[j*4+2],verts[j*4+3]));
				}
			}
			//qDebug()<<"type"<<qstr;
			//pT->SetEntityType(qstr);
			DS>>sz2;
			//qDebug()<<"reading fields";
			QString filed, value;
			uint strsz;
			//QChar qch;
			ushort uch;
			for(int i = 0;i<sz2;i++)
			{
				filed.clear();
				value.clear();
				
				DS>>strsz;
				strsz>>=1;
				while(strsz-->0)
				{
					DS>>uch;
					filed += QChar(uch);
				}
				
				DS>>strsz;
				strsz>>=1;
				while(strsz-->0)
				{
					DS>>uch;
					value += QChar(uch);
				}
				//DS>>filed;	//freadstr(&filed,pf);
				//DS>>value;	//freadstr(&value,pf);
				if (pEnt)
				{
					pEnt->ParseKeyValue(filed,value);
				}
				//pT->m_pEntity->fields[filed]->fromString(value);
			}
		}
		//g_pTriggsTool->m_SelectedTriggs.clear();
		
		DS>>sz;	//fread(&sz,4,1,pf); //numentities
		//DBG_PRINT_S("numentities "+sz);
		//qDebug()<<"numentities"<<sz;
		for(int i=0;i<sz;i++)
		{
			int objDepth;
			QVector2D orig;
			DS>>orig>>objDepth>>qstr;	
			
			cBaseObject *pEnt = pNewRoom->CreateObject(qstr);
			if (pEnt)
			{
				//if (g_LandmarkName.isEmpty())
				if (!(pEnt->m_Flags & FL_TRANSFER))
					pEnt->m_Orig = orig;
				//else
				//	qDebug()<<"restored orig"<<pEnt->m_Orig;
				pNewRoom->ChangeObjectDepth(pEnt,objDepth);
			}
			else
			{
				DBG_PRINT_S(QString("Unknown entity: ")+qstr);
			}
			DS>>sz2;

			for(int i = 0;i<sz2;i++)
			{
				QString filed, value;
				DS>>filed;	//freadstr(&filed,pf);
				DS>>value;	//freadstr(&value,pf);
				if (pEnt)
					pEnt->ParseKeyValue(filed,value);
			}
		}
		DS>>pNewRoom->m_Size;	//fread(&m_RoomSize[0],4,1,pf);
		//fread(&m_RoomSize[1],4,1,pf);
		
		if (!DS.atEnd())
		{
			bool chkHor,chkDown,chkUp;
			DS>>chkHor>>chkDown>>chkUp;
			pNewRoom->SetBlock(EBS_LEFT,chkHor);//gpMainWND->ui->chk_HorBlock->setChecked(chkHor);
			pNewRoom->SetBlock(EBS_RIGHT,chkHor);
			pNewRoom->SetBlock(EBS_DOWN,chkDown);//gpMainWND->ui->chk_DownBlock->setChecked(chkDown);
			pNewRoom->SetBlock(EBS_UP,chkUp);//gpMainWND->ui->chk_UpBlock->setChecked(chkUp);
		}
		
		for(int i=pNewRoom->m_Layers.size()-1;i>=0;i--)
			pNewRoom->m_Layers[i]->resizeLayer(pNewRoom->m_Size);
		
		//fclose(pf);
		pf.close();
		
		//qDebug()<<"g_LandmarkName"<<g_LandmarkName;
				
		if (!g_LandmarkName.isEmpty())
		{
			cBaseObject *pEnt = pNewRoom->ObjectByTargetname(g_LandmarkName);
			//qDebug()<<"pEnt"<<pEnt;
			if (pEnt)
			{
				//qDebug()<<"g_TrasnferedList.size()"<<g_TrasnferedList.size();
				for(int i=g_TrasnferedList.size()-1;i>=0;i--)
				{
					g_TrasnferedList[i]->m_Orig += pEnt->m_Orig;
					g_TrasnferedList[i]->m_Velocity = QVector2D(0.f,0.f);
				}
			}
			g_LandmarkName.clear();
		}
		
		for(int i=pNewRoom->m_pObjects.size()-1;i>=0;i--)
			pNewRoom->m_pObjects[i]->Spawn();
		//MGR->m_pCurrentRoom = prevRoom;
	}
	
	return pNewRoom;
}

void cGameRoom::Unload()
{
	
}
void cGameRoom::SetBlock(BLOCK_SIDE side, bool active)
{
	m_Block[side] = active;
}

void cGameRoom::ExpandWalls(void)
{
//	QVector<cBase *>objs;
//	OBJECTS_BY_CLASSNAME(par_expand,objs);
//	for(int i=objs.size()-1;i>=0;i--) REMOVE_OBJECT(objs[i]);
//	OBJECTS_BY_CLASSNAME(axial_expand,objs);
//	for(int i=objs.size()-1;i>=0;i--) REMOVE_OBJECT(objs[i]);
	
	QMap<qint64,cLine *>tSegs;
	cLine *seg;
	for(int i=m_Lines.size()-1;i>=0;i--)
	{
		seg = m_Lines[i];
		seg->m_Neighs.clear();
		seg->ExpandMe(VEC_EXPAND,this);
		if (seg->m_JumpDown)
		{
			for(int i=seg->m_Axials.size()-1;i>=0;i--)
			{
				if (seg->m_Axials[i]->m_N.x() != 0.f)
					seg->m_Axials[i]->m_ToRemove = true;
			}
			for(int i=seg->m_Exps.size()-1;i>=0;i--)
			{
				if (seg->m_Exps[i]->m_N.y() >= 0.f)
					seg->m_Exps[i]->m_ToRemove = true;
			}
		}
		tSegs.insertMulti(qPointToLongLong(seg->m_S.toPoint()),seg);
		tSegs.insertMulti(qPointToLongLong(seg->m_E.toPoint()),seg);
	}
	QVector<cLine *>vecDone;
	QList<cLine *>vecNeighs;
	for(int i=m_Lines.size()-1;i>=0;i--)
	{
		vecDone.clear();
		seg = m_Lines[i];
		vecNeighs  = tSegs.values(qPointToLongLong(seg->m_S.toPoint()));
		vecNeighs += tSegs.values(qPointToLongLong(seg->m_E.toPoint()));
		for(int j=vecNeighs.size()-1;j>=0;j--)
		{
			if (vecNeighs[j] == seg) continue;
			if (vecDone.contains(vecNeighs[j])) continue;
			
			if ((vecNeighs[j]->m_S == seg->m_E) ||
				 (vecNeighs[j]->m_E == seg->m_S) )
			{
				vecDone.push_back(      vecNeighs[j]);
				seg->m_Neighs.push_back(vecNeighs[j]);
			}
		}
	}
	cLine *oseg;
	cSegment *ex,*oex;
	for(int i=m_Lines.size()-1;i>=0;i--)
	{
		seg = m_Lines[i];
		if (!seg->m_Neighs.size())
		{
			for(int n=seg->m_Exps.size()-1;n>=0;n--)
			{
				ex = seg->m_Exps[n];
				if (dot(ex->m_N,seg->m_N) < 0.f) ex->m_ToRemove = true;
			}
		}
		else for(int j=seg->m_Neighs.size()-1;j>=0;j--)
		{
			for(int k=seg->m_Axials.size()-1;k>=0;k--)
			{
				if (seg->m_Neighs[j]->ExpCaseHasPoint(seg->m_Axials[k]->m_S))
				{
					seg->m_Axials[k]->m_ToRemove = true;
					continue;
				}
				if (seg->m_Neighs[j]->ExpCaseHasPoint(seg->m_Axials[k]->m_E))
				{
					seg->m_Axials[k]->m_ToRemove = true;
					continue;
				}
				if (seg->m_Neighs[j]->ExpCaseHasPoint(
						 (seg->m_Axials[k]->m_S + seg->m_Axials[k]->m_E)*0.5f
						 ))
				{
					seg->m_Axials[k]->m_ToRemove = true;
					continue;
				}
			}
			oseg = seg->m_Neighs[j];
			for(int n=seg->m_Exps.size()-1;n>=0;n--)
			{
				ex = seg->m_Exps[n];
				if (dot(ex->m_N,seg->m_N) < 0.f) ex->m_ToRemove = true;
				for(int m=oseg->m_Exps.size()-1;m>=0;m--)
				{
					oex = oseg->m_Exps[m];
					if (oex->m_ToRemove) continue;
					if (dot(oex->m_T,ex->m_T) > 0.f && oex->ContaintsLine(ex))
						ex->m_ToRemove = true;
				}
			}
		}
		for(int j=seg->m_Exps.size()-1;j>=0;j--)
		{
			if (seg->m_Exps[j]->m_ToRemove)
				RemoveExpand(seg->m_Exps[j]);
		}
	}
}

void cGameRoom::RemoveExpand(cSegment *exp)
{
	int idx = m_Walls.indexOf(exp);
	if (idx == -1) return;
	m_Walls.removeAt(idx);
	//RemoveRenderable(exp);
	//DeleteObject(exp);
	delete exp;
}
	
cSegment *cGameRoom::CreateExpand(bool axial)
{
	cSegment *seg = NULL;
	if (axial) seg = new cParExpand;
	else seg = new cAxialExpand;
	seg->m_pMyRoom = this;
	m_Walls.push_back(seg);
	
	//RememberRenderable(seg);
	//m_pObjects.push_back(seg);
	
	return seg;
}

void cGameRoom::RemoveFlaged()
{
	for(int i=0;i<=m_Walls.size();)
	{
		if (m_Walls[i]->m_ToRemove)
		{
			delete m_Walls[i];
			m_Walls.removeAt(i);
			continue;
		}
		i++;
	}
}

cBaseObject *cGameRoom::CreateObject(QString name)
{
	//qDebug()<<"Creating"<<name<<"object";
	cBaseObject *pObj = NULL;
	for(int i=g_TrasnferList.size()-1;i>=0;i--)
	{
		if (g_TrasnferList[i]->m_ClassName == name)
		{
			pObj = (cBaseObject *)RememberRenderable(g_TrasnferList[i]);
			g_TrasnferList.removeAt(i);
			g_TrasnferedList.push_back(pObj);
			break;
		}
	}
	if (!pObj)
	{
		if      (name == "ring"                ) pObj = (cBaseObject *)RememberRenderable(new cRing         );
		else if (name == "maverick"            ) pObj = (cBaseObject *)RememberRenderable(new cMaverick     );
		else if (name == "npc_hellen"          ) pObj = (cBaseObject *)RememberRenderable(new cNpcHellen    );
		else if (name == "mainmenu"            ) pObj = (cBaseObject *)RememberRenderable(new cMainMenu     );
		else if (name == "options"             ) pObj = (cBaseObject *)RememberRenderable(new cOptionsMenu  );
		else if (name == "controlls"           ) pObj = (cBaseObject *)RememberRenderable(new cControllsMenu);
		else if (name == "string_trigger"      ) pObj = (cBaseObject *)RememberRenderable(new cStringTrigger);
		else if (name == "base_anim"           ) pObj = (cBaseObject *)RememberRenderable(new cBaseAnimating);
		else if (name == "trigger_changelevel" ) pObj = (cBaseObject *)RememberRenderable(new cChangelevel  );
		else if (name == "env_fade"            ) pObj = (cBaseObject *)RememberRenderable(new cFader        );
		else if (name == "background"          ) pObj = (cBaseObject *)RememberRenderable(new cBackground   );
		else if (name == "tile_layer"          ) pObj = (cBaseObject *)RememberRenderable(new cTileLayer    );
		else if (name == "trigger_teleport"    ) pObj = (cBaseObject *)RememberRenderable(new cTriggerTeleport);
		else if (name == "trigger_door"        ) pObj = (cBaseObject *)RememberRenderable(new cTriggerDoor  );
		else if (name == "trigger_once"        ) pObj = (cBaseObject *)RememberRenderable(new cTriggerOnce  );
		else if (name == "info_target"         ) pObj = (cBaseObject *)RememberRenderable(new cBaseObject   );
		else if (name == "info_landmark"       ) pObj = (cBaseObject *)RememberRenderable(new cBaseObject   );
		else if (name == "info_dialog"         ) pObj = (cBaseObject *)RememberRenderable(new cInfoDialog   );
		else if (name == "cycler_sprite"       ) pObj = (cBaseObject *)RememberRenderable(new cBaseAnimating);
		else if (name == "hud"                 )
		{
			pObj = (cBaseObject *)RememberRenderable(new cHud);
			ChangeObjectDepth(pObj,INT_MIN);
		}
		else if (name.left(7) == "weapon_" )
		{
			DBG_PRINT_S("creating weapon "+name);
			pObj = (cBaseObject *)cBaseWeapon::SpawnWeapon(name);
			if (!pObj) return NULL;
			
			pObj = (cBaseObject *)RememberRenderable(pObj);
		}
		else return NULL;
	}
	
	pObj->m_ClassName = name;
	
	//if (!targetname.isEmpty())
	//	m_ObjsByName[targetname] = pObj;
	
	m_ObjsByClassname.insertMulti(name,pObj);
	m_pObjects.push_back(pObj);
	
	return pObj;
}
void cGameRoom::DeleteObject(cBaseObject *obj)
{
	int idx = m_pObjects.indexOf(obj);
	if (idx == -1) return;
	RemoveRenderable(m_pObjects[idx]);
	//qDebug()<<"Deleting"<<m_pObjects[idx]->m_Name<<"object";
	m_pObjects.removeAt(idx);
	QMap<QString, cBaseObject *>::iterator it;
	for(it = m_ObjsByClassname.begin();it != m_ObjsByClassname.end();it++)
	{
		if (it.value() == obj)
		{
			m_ObjsByClassname.erase(it);
			break;
		}
	}
	for(it = m_ObjsByName.begin();it != m_ObjsByName.end();it++)
	{
		if (it.value() == obj)
		{
			m_ObjsByName.erase(it);
			break;
		}
	}
	if ( !(obj->m_Flags & FL_TRANSFER) )
		delete obj;
}
cBackground * cGameRoom::AddBack(QString fname,
		QVector2D orig,
		QVector2D speed,
		QVector2D scrFac,
		bool hwrap,
		bool vwrap,
		int depth)
{
	QImage img(fname);
	if (img.isNull())
	{
		qDebug()<<"No back";
		return NULL;
	}
	//qDebug()<<"Back loaded";
	cBackground *pBack = (cBackground *)CreateObject("background");//new cBackground;
	pBack->filename = fname;
	//DBG_PRINT_S("fname "+fname);
	pBack->texture = MGR->bindTexture(img,
												 GL_TEXTURE_2D,
											    GL_RGBA,
											    QGLContext::InvertedYBindOption);
	//DBG_PRINT_S(QString().number(pBack->texture,16));
	//qDebug()<<"texture"<<pBack->texture;
	pBack->width = img.width();
	pBack->height = img.height();
	pBack->orig = orig;
	pBack->scr_fac = scrFac;
	pBack->hwrap = hwrap;
	pBack->vwrap = vwrap;
	pBack->velocity = speed;
	ChangeObjectDepth(pBack,depth);
	
	return pBack;
}
void cGameRoom::RemoveBack(cBackground *back)
{
	for(int i=m_Backs.size()-1;i>=0;i--)
		if (m_Backs[i] == back)
		{
			m_Backs.removeAt(i);
			delete back;
			return;
		}
}
void cGameRoom::RoomUpdate()
{
//	if (!(m_Flags & FL_KILLME))
	{
		m_Scroll += m_AutoScroll;
		
		for(int i=m_Walls.size()-1;i>=0;i--) m_Walls[i]->Step();
		for(int i=m_pObjects.size()-1;i>=0;i--) m_pObjects[i]->Step();
		if (m_Scroll.x() != 0 || m_Scroll.y() != 0)
		{
			for(int i=m_pTriggers.size()-1;i>=0;i--)
			{
				m_pTriggers[i]->ScrollMe(m_Scroll);
			}
			for(int i=m_pObjects.size()-1;i>=0;i--) 
						m_pObjects[i]->m_Orig += m_Scroll*m_pObjects[i]->m_ScrollFac;
			for(int i=m_Walls.size()-1;i>=0;i--)
			{
				m_Walls[i]->m_S += m_Scroll;
				m_Walls[i]->m_E += m_Scroll;
				m_Walls[i]->CalcMe();
			}
			for(int i=m_Lines.size()-1;i>=0;i--)
			{
				m_Lines[i]->m_S += m_Scroll;
				m_Lines[i]->m_E += m_Scroll;
			}
	//		m_pTileMngr->ScrollAllLayers(m_Scroll);
			
			m_View.orig -= m_Scroll;
		}
	}
	//qDebug()<<"sz";
	int sz = m_pToRender.size();
	//qDebug()<<"for";
	for(int i=0;i<sz;i++) 
	{
		if (m_pToRender[i]->m_Visible)
			m_pToRender[i]->RenderMe();
	}
	if (UTIL_GetCvar("debug_walls")->value_i)//g_DebugWalls)
	{
//		sz = m_Walls.size();
//		for(int i=0;i<sz;i++) 
//		{
//				m_Walls[i]->RenderMe();
//		}
		sz = m_Lines.size();
		glColor3f(0.f,1.f,0.f);
		for(int i=0;i<sz;i++) 
		{
			
			MGR->drawLine(m_Lines[i]->m_S,m_Lines[i]->m_E);
				//m_Walls[i]->RenderMe();
		}
	}
	
	m_Scroll = QVector2D(0,0);
	
	if (m_Flags & FL_KILLME)
	{
		delete this;
		for(int i=g_TrasnferedList.size()-1;i>=0;i--)
			g_TrasnferedList[i]->m_Flags &= ~FL_TRANSFER;
		g_TrasnferedList.clear();
		return;
	}
	
}	

extern QVector2D g_StartOrig;
void cGameRoom::ScrollRoom(const QVector2D &scr, bool ignore_max)//QPoint const &scr)
{	
	if (m_AutoScroll.x() != 0.f || m_AutoScroll.y() != 0.f)
		return;
	
	QVector2D toScr = scr, view = m_View.orig - m_Scroll;
	
	if (m_Block[EBS_LEFT])
		if (toScr[XX] < 0 && view[XX]+toScr[XX] <= 0) 
			toScr[XX] = -view[XX];
	
	if (m_Block[EBS_RIGHT])
		if (toScr[XX] > 0 && view[XX]+toScr[XX] >= m_Size.x()-640) 
		{
			toScr[XX] = float(m_Size.x())-view[XX]-640.f;
		}
	if (m_Block[EBS_UP])
		if (toScr[YY] < 0 && view[YY]+toScr[YY] <= 0) 
			toScr[YY] = -view[YY];
	
	if (m_Block[EBS_DOWN])
		if (toScr[YY] > 0 && view[YY]+toScr[YY] >= m_Size.y()-480) 
			toScr[YY] = m_Size.y()-view[YY]-480;
	if (!ignore_max)
	{
		for(int i=0;i<2;i++)
			toScr[i] = std::min(std::max(toScr[i],-SCROLL_SPEED*MGR->m_DeltaTime),SCROLL_SPEED*MGR->m_DeltaTime);
	}

	m_Scroll -= toScr;
	g_StartOrig -= toScr;
}
void cGameRoom::ObjectSetTargetname(cBaseObject *pObj, QString name)
{
	if (m_ObjsByName.keys().contains(name))
	{
		QMap<QString, cBaseObject *>::iterator i = m_ObjsByName.begin();
		while(i != m_ObjsByName.end())
		{
			if (i.value() == pObj)
			{
				m_ObjsByName.erase(i);
				break;
			}
			++i;
		}
	}
	pObj->m_TargetName = name;
	m_ObjsByName.insertMulti(name,pObj);
	//qDebug()<<"room name"<<m_Name;
}
cBaseObject * cGameRoom::ObjectByTargetname(QString name)
{
	//qDebug()<<"m_ObjsByName"<<m_ObjsByName;
	if (!m_ObjsByName.contains(name)) return NULL;
	
	return m_ObjsByName.values(name)[0];
}
QList<cBaseObject *> cGameRoom::ObjectsByTargetname(QString name)
{
	return m_ObjsByName.values(name);
}
QList<cBaseObject *> cGameRoom::ObjectsByClassname(QString name)
{
	return m_ObjsByClassname.values(name);
}
void cBackground::Step()
{
	
}
void cBackground::Draw()
{
	//return;
	
	if (!visible) return;
	//qDebug()<<"cBackground::Draw()";
	
	orig += velocity*MGR->m_DeltaTime;
	QVector2D tOrig = orig+m_Orig*scr_fac;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,texture);
	
	glColor3f(1.f,1.f,1.f);
	//0  1
	//2  3
	//4  5
	//6  7
	float verts[8];
	float   uvs[8];
	if (hwrap) 
	{
		verts[0] = verts[6] =   0.f;
		verts[2] = verts[4] = 640.f;
		uvs[0] = uvs[6] = (-floor(tOrig.x())      )/width;
	   uvs[2] = uvs[4] = (-floor(tOrig.x())+640.f)/width;
	}
	else
	{
		verts[0] = verts[6] = floor(tOrig.x())      ;
		verts[2] = verts[4] = floor(tOrig.x())+width;
		uvs[0] = uvs[6] = 0.f;
		uvs[2] = uvs[4] = 1.f;
	}
	
	if (vwrap) 
	{
		verts[1] = verts[3] =   0.f;
		verts[5] = verts[7] = 480.f;
		uvs[1] = uvs[3] = (floor(tOrig.y())+480.f)/height;
	   uvs[5] = uvs[7] = (floor(tOrig.y())      )/height;
	}
	else
	{
		verts[1] = verts[3] = floor(tOrig.y())       ;
		verts[5] = verts[7] = floor(tOrig.y())+height;
		uvs[1] = uvs[3] = 1.f;
		uvs[5] = uvs[7] = 0.f;
	}
	
	glBegin(GL_QUADS);
	for(int i=0;i<8;i+=2)
	{
		glTexCoord2f(uvs[i]  ,uvs[i+1]  );
		glVertex2f  ((int)verts[i],(int)verts[i+1]);
	}
	glEnd();
}
roomview_s::roomview_s()
{
	orig = QVector2D(0,0);
	block[0] = block[1] = block[2] = block[3] = !false;
	block_val[0] = 0;
	block_val[1] = 640;
	block_val[2] = 0;
	block_val[3] = 480;
}
